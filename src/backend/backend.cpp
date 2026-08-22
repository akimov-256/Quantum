#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
    , manager(new QNetworkAccessManager(this))
{
    m_downloadModel.setDownloads(&m_downloads);
}

void Backend::CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const int &connections, const QString &SHA256)
{
    downloadInformations info;

    info.fileName = fileName;
    if (filePath.isEmpty())
        info.savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    else
        info.savePath = filePath;
    info.category = detectCategory(fileName);
    info.savePath += "/" + fileName;
    info.url = fileUrl;
    info.SHA256 = SHA256;
    info.chunkCount = connections;
    info.status = "Starting...";
    info.progress = 0;
    info.ID = QUuid::createUuid().toString(QUuid::WithoutBraces);

    m_downloads.append(info);
    int row = m_downloads.size() - 1;
    m_downloadModel.addDownload(row);

    Downloader *downloader = new Downloader(this);
    m_activeDownloaders.insert(info.ID, downloader);

    downloader->download(info);

    emit countChanged();

    connect(downloader, &Downloader::progressChanged, this, [this, id = info.ID](qint64 bytesReceived, qint64 bytesTotal) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].fileByteSize = bytesTotal;
        m_downloads[row].currentSize = bytesReceived;
        if (bytesTotal > 0)
            m_downloads[row].progress = static_cast<double>(bytesReceived) * 100.0 / bytesTotal;
        m_downloadModel.updateDownload(row);
    });

    connect(downloader, &Downloader::speedChanged, this, [this, id = info.ID](qint64 bps) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].speed = bps;
        m_downloadModel.updateDownload(row);
    });

    connect(downloader, &Downloader::downloadFinished, this, [this, id = info.ID, downloader](bool success, const QString &message) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].status = success ? "Completed" : "Failed";
        emit countChanged();
        m_downloadModel.updateDownload(row);
        qDebug() << message;

        // Clean up
        downloader->deleteLater();
        m_activeDownloaders[id] = nullptr;
    });
}

void Backend::GetHeadInfo(const QString &fileUrl)
{
    // Update GetHeadInfo status
    m_isHeadReqActive = true;
    emit isHeadReqActiveChanged();

    QUrl url = QUrl::fromUserInput(fileUrl);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->head(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        // Get file size
        m_fileSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();

        emit fileSizeChanged();

        // Get file name
        QByteArray disposition = reply->rawHeader("Content-Disposition");

        QRegularExpression re(R"(filename\*?=(?:UTF-8''|")?([^";]+))");
        QRegularExpressionMatch match = re.match(QString::fromUtf8(disposition));

        if (match.hasMatch())
            m_fileName = QUrl::fromPercentEncoding(match.captured(1).toUtf8());
        else
            m_fileName = QFileInfo(reply->url().path()).fileName();

        if (m_fileName.isEmpty()) m_fileName = "download";

        emit fileNameChanged();

        // Update GetHeadInfo status
        m_isHeadReqActive = false;
        emit isHeadReqActiveChanged();

        reply->deleteLater();
    });
}

DownloadCategory Backend::detectCategory(const QString &fileName) {
    QString extension = QFileInfo(fileName)
    .suffix()
        .toLower();

    if (extension == "zip" ||
        extension == "rar" ||
        extension == "7z") {
        return DownloadCategory::Compressed;
    }

    if (extension == "pdf" ||
        extension == "docx" ||
        extension == "txt") {
        return DownloadCategory::Documents;
    }

    if (extension == "mp3" ||
        extension == "wav" ||
        extension == "flac") {
        return DownloadCategory::Music;
    }

    if (extension == "mp4" ||
        extension == "mkv" ||
        extension == "avi") {
        return DownloadCategory::Videos;
    }

    if (extension == "exe" ||
        extension == "msi") {
        return DownloadCategory::Programs;
    }

    return DownloadCategory::Other;
}

void Backend::buttonClicked(const int row) {
    if (row < 0 || row >= m_activeDownloaders.size())
        return;

    QString id = m_downloads[row].ID;
    Downloader *downloader = m_activeDownloaders.value(id, nullptr);
    if (!downloader)
        return;

    int currentRow = rowForId(id);
    if (currentRow == -1)
        return;

    if (downloader->downloadInfo().status == "Paused")
    {
        downloader->downloadResume(downloader->downloadInfo());
        m_downloads[currentRow].status = "Downloading...";
    }
    else
    {
        downloader->downloadPause();
        m_downloads[currentRow].status = "Paused";
    }

    emit countChanged();
    m_downloadModel.updateDownload(currentRow);
}

void Backend::cancelClicked(const int row)
{
    if (row < 0 || row >= m_activeDownloaders.size())
        return;

    QString id = m_downloads[row].ID;
    Downloader *downloader = m_activeDownloaders.value(id, nullptr);
    if (!downloader)
        return;

    downloader->downloadStop();

    int currentRow = rowForId(id);
    if (currentRow == -1)
        return;

    m_activeDownloaders.remove(id);
    m_downloadModel.removeRow(currentRow);
    emit countChanged();
}

void Backend::openRequested(const int row)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloads[row].savePath));
}

void Backend::removeRequested(const int row)
{
    m_downloadModel.removeRow(row);
}

void Backend::pauseAll() {
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        QString id = m_downloads[i].ID;
        Downloader *downloader = m_activeDownloaders.value(id, nullptr);

        if (downloader->downloadInfo().status != "Paused")
        {
            downloader->downloadPause();
            m_downloads[i].status = "Paused";
        }

        emit countChanged();
        m_downloadModel.updateDownload(i);
    }
}

void Backend::resumeAll() {
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        QString id = m_downloads[i].ID;
        Downloader *downloader = m_activeDownloaders.value(id, nullptr);

        if (downloader->downloadInfo().status == "Paused")
        {
            downloader->downloadResume(downloader->downloadInfo());
            m_downloads[i].status = "Downloading...";
        }

        emit countChanged();
        m_downloadModel.updateDownload(i);
    }
}

QRect Backend::availableScreenGeometry() const
{
    QScreen *screen = QGuiApplication::primaryScreen();
    return screen ? screen->availableGeometry() : QRect(0, 0, 1200, 700);
}

QString Backend::coloredSvg(const QString &path, const QString &color)
{
    QString resourcePath = path;
    if (resourcePath.startsWith("qrc:"))
        resourcePath = resourcePath.mid(3); // "qrc:/..." -> ":/..."

    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to load SVG:" << resourcePath;
        return QString();
    }

    QString svgText = QString::fromUtf8(file.readAll());
    file.close();

    static QRegularExpression fillRe(R"(fill="#[0-9a-fA-F]{3,8}")");
    svgText.replace(fillRe, "fill=\"" + color + "\"");

    QByteArray base64 = svgText.toUtf8().toBase64();
    return "data:image/svg+xml;base64," + QString::fromLatin1(base64);
}

void Backend::setCategory(int category)
{
    m_currentCategory = category;

    m_downloadModel.setCategory(category);
}

int Backend::pausedCount() const
{
    int count = 0;
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        QString id = m_downloads[i].ID;
        if (m_activeDownloaders.value(id, nullptr)->downloadInfo().status == "Paused")
            count++;
    }
    return count;
}

int Backend::activeCount() const
{
    int count = 0;
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        QString id = m_downloads[i].ID;
        if (m_activeDownloaders.value(id, nullptr)->downloadInfo().status != "Paused")
            count++;
    }
    return count;
}

QString Backend::fileName() const
{
    return m_fileName;
}

qint64 Backend::fileSize() const
{
    return m_fileSize;
}

DownloadModel *Backend::downloadModel()
{
    return &m_downloadModel;
}

bool Backend::isHeadReqActive() const
{
    return m_isHeadReqActive;
}

int Backend::downloadCount() const
{
    int count = 0;
    for (downloadInformations dInfo : m_downloads)
    {
        if (dInfo.status == "Downloading..." || dInfo.status == "Starting...")
            count++;
    }
    return count;
}

int Backend::completedCount() const
{
    int count = 0;
    for (downloadInformations dInfo : m_downloads)
    {
        if (dInfo.status == "Completed")
            count++;
    }
    return count;
}

int Backend::rowForId(const QString &id) const
{
    for (int i = 0; i < m_downloads.size(); i++)
        if (m_downloads[i].ID == id)
            return i;
    return -1;
}