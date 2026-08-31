#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
    , m_webServer(new QTcpServer(this))
    , manager(new QNetworkAccessManager(this))
    , m_fileNameHandler(new FileNameHandler(this))
    , m_databaseManager(new DatabaseManager(this))
{
    m_downloadModel.setDownloads(&m_downloads);

    StartWebServer();
}

void Backend::StartWebServer()
{
    connect(
        m_webServer,
        &QTcpServer::newConnection,
        this,
        [this]()
        {
            QTcpSocket *socket =
                m_webServer->nextPendingConnection();

            connect(
                socket,
                &QTcpSocket::readyRead,
                this,
                [ socket, this ]()
                {
                    QByteArray request = socket->readAll();

                    int bodyStart = request.indexOf("\r\n\r\n");

                    if (bodyStart == -1)
                        return;

                    QByteArray body = request.mid(bodyStart + 4);

                    QJsonDocument doc = QJsonDocument::fromJson(body);
                    if (!doc.isObject())
                        return;
                    QJsonObject json = doc.object();

                    emit urlRecieved(json["url"].toString());
                });
        }
        );

    if (!m_webServer->listen(
            QHostAddress::LocalHost,
            8421))
    {
        qWarning()
        << "Could not start web server:"
        << m_webServer->errorString();
    }
    else
    {
        qDebug()
        << "Quantum web server running on port 8421";
    }
}

bool Backend::downloadRequested(const QString &fileUrl, const QString &fileName, const QString &filePath, const int &connections, const QString &SHA256)
{
    const QString path = filePath + "/" + fileName;         // Build the complete path.
    const bool exists = QFileInfo::exists(path);            // Check if the file already exists in the directory.
    if (exists)
    {
        auto reply = QMessageBox::warning(nullptr, "Replace File", "The file \"" + fileName + "\" already exists in the target directory.\nDo you want to replace it?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No)
            return false;                                   // Show the warning dialog and do not start the download if the user selected no.
    }

    CreateDownload(fileUrl, fileName, filePath,             // Create the download normally if the user selected otherwise.
                       connections, SHA256);
    return true;
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
    m_databaseManager->insertDownload(info);                // Insert new download to database.

    downloader->download(info);

    emit countChanged();

    connect(downloader, &Downloader::progressChanged, this, [this, id = info.ID](qint64 bytesReceived, qint64 bytesTotal) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].fileByteSize = bytesTotal;
        m_downloads[row].currentSize = bytesReceived;
        if (bytesTotal > 0)
            m_downloads[row].progress = static_cast<double>(bytesReceived) * 100.0 / bytesTotal;
        m_downloadModel.updateDownload(m_downloads[row].ID);
    });

    connect(downloader, &Downloader::speedChanged, this, [this, id = info.ID](qint64 bps) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].speed = bps;
        m_downloadModel.updateDownload(m_downloads[row].ID);
    });

    connect(downloader, &Downloader::downloadFinished, this, [this, id = info.ID, downloader](bool success, const QString &message) {
        int row = rowForId(id);
        if (row == -1) return;

        m_downloads[row].status = success ? "Completed" : "Failed";
        emit countChanged();
        m_downloadModel.updateDownload(m_downloads[row].ID);
        qDebug() << message;

        // Clean up
        downloader->deleteLater();
        m_activeDownloaders[id] = nullptr;
    });
}

void Backend::getHeadInfo(const QString &fileUrl, const QString &targetPath)
{
    m_headReqCompleted= false;
    emit headReqCompletedChanged();

    m_fileSize = 0;
    emit fileSizeChanged();

    m_fileName.clear();
    emit fileNameChanged();

    // Update GetHeadInfo status
    m_isHeadReqActive = true;
    emit isHeadReqActiveChanged();

    QUrl url = QUrl::fromUserInput(fileUrl);
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute     // Restrict HTTP/1.1 on HEAD requests.
                         , false);
    request.setHeader(QNetworkRequest::UserAgentHeader,             // Mimic a browser so modern servers do not ignore the request.
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/120.0.0.0 Safari/537.36");
    QUrl reqUrl = request.url();
    request.setRawHeader("Referer",                                 // Add a referer to handle particular server exceptions.
        (reqUrl.scheme() + "://" + reqUrl.host() + "/").toUtf8());
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

        if (m_fileName.isEmpty())
            m_fileName = "download";
        else
        {
            const QString filePath = targetPath + "/" + m_fileName;
            m_fileName = m_fileNameHandler.CheckFile(filePath);
        }

        emit fileNameChanged();

        // Update GetHeadInfo status
        m_isHeadReqActive = false;
        emit isHeadReqActiveChanged();

        m_headReqCompleted = true;
        emit headReqCompletedChanged();

        reply->deleteLater();
    });
}

DownloadCategory Backend::detectCategory(const QString &fileName) {
    QString extension = QFileInfo(fileName)
    .completeSuffix()
        .toLower();

    if (extension == "zip"    ||
        extension == "rar"    ||
        extension == "7z"     ||
        extension == "tar"    ||
        extension == "gz"     ||
        extension == "bz2"    ||
        extension == "xz"     ||
        extension == "zst"    ||
        extension == "lz"     ||
        extension == "lzma"   ||
        extension == "cab"    ||
        extension == "arj"    ||
        extension == "z"      ||
        extension == "tar.gz" ||
        extension == "tgz"    ||
        extension == "tbz2"   ||
        extension == "txz"    ||
        extension == "taz"    ||
        extension == "jar"    ||
        extension == "war"    ||
        extension == "ear"    ||
        extension == "apk"    ||
        extension == "aar") {
        return DownloadCategory::Compressed;
    }

    if (extension == "pdf"      ||
        extension == "doc"      ||
        extension == "docx"     ||
        extension == "xls"      ||
        extension == "xlsx"     ||
        extension == "xlsm"     ||
        extension == "ppt"      ||
        extension == "pptx"     ||
        extension == "ppsx"     ||
        extension == "odt"      ||
        extension == "ods"      ||
        extension == "odp"      ||
        extension == "odg"      ||
        extension == "txt"      ||
        extension == "rtf"      ||
        extension == "log"      ||
        extension == "md"       ||
        extension == "markdown" ||
        extension == "html"     ||
        extension == "htm"      ||
        extension == "xml"      ||
        extension == "xhtml"    ||
        extension == "csv"      ||
        extension == "tsv"      ||
        extension == "json"     ||
        extension == "yml"      ||
        extension == "yaml"     ||
        extension == "epub"     ||
        extension == "mobi"     ||
        extension == "azw"      ||
        extension == "azw3"     ||
        extension == "fb2"      ||
        extension == "tex"      ||
        extension == "pages"    ||
        extension == "numbers"  ||
        extension == "key"      ||
        extension == "wps") {
        return DownloadCategory::Documents;
    }

    if (extension == "mp3"  ||
        extension == "wav"  ||
        extension == "flac" ||
        extension == "aac"  ||
        extension == "ogg"  ||
        extension == "m4a"  ||
        extension == "m4b"  ||
        extension == "wma"  ||
        extension == "alac" ||
        extension == "aiff" ||
        extension == "aif"  ||
        extension == "au"   ||
        extension == "snd"  ||
        extension == "opus" ||
        extension == "amr"  ||
        extension == "mid"  ||
        extension == "midi" ||
        extension == "mpga" ||
        extension == "mka"  ||
        extension == "ra"   ||
        extension == "rmi"  ||
        extension == "ape"  ||
        extension == "wv"   ||
        extension == "dsd"  ||
        extension == "dff"  ||
        extension == "dsf") {
        return DownloadCategory::Music;
    }

    if (extension == "mp4"  ||
        extension == "mkv"  ||
        extension == "avi"  ||
        extension == "mov"  ||
        extension == "wmv"  ||
        extension == "flv"  ||
        extension == "webm" ||
        extension == "m4v"  ||
        extension == "mpg"  ||
        extension == "mpeg" ||
        extension == "m2ts" ||
        extension == "ts"   ||
        extension == "vob"  ||
        extension == "3gp"  ||
        extension == "3g2"  ||
        extension == "ogv"  ||
        extension == "rm"   ||
        extension == "rmvb" ||
        extension == "divx" ||
        extension == "asf"  ||
        extension == "mts"  ||
        extension == "m2v"  ||
        extension == "f4v"  ||
        extension == "m4p"  ||
        extension == "mpv"  ||
        extension == "mpe"  ||
        extension == "m1v"  ||
        extension == "m2p"  ||
        extension == "mp2v" ||
        extension == "m4e") {
        return DownloadCategory::Videos;
    }

    if (extension == "exe"        ||
        extension == "msi"        ||
        extension == "msp"        ||
        extension == "msm"        ||
        extension == "appx"       ||
        extension == "appxbundle" ||
        extension == "msix"       ||
        extension == "msixbundle" ||
        extension == "com"        ||
        extension == "scr"        ||
        extension == "cpl") {
        return DownloadCategory::Programs;
    }

    return DownloadCategory::Other;
}

void Backend::buttonClicked(const QString id) {
    if (rowForId(id) < 0 || rowForId(id) >= m_activeDownloaders.size())
        return;

    Downloader *downloader = m_activeDownloaders.value(id, nullptr);
    if (!downloader)
        return;

    int currentRow = rowForId(id);
    if (currentRow == -1)
        return;

    if (m_downloads[currentRow].status == "Paused")
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
    m_downloadModel.updateDownload(m_downloads[currentRow].ID);
}

void Backend::cancelClicked(const QString id)
{
    auto reply = QMessageBox::warning(nullptr, "Cancel Download", "This download will be canceled.\nAre you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;


    if (rowForId(id) < 0 || rowForId(id) >= m_activeDownloaders.size())
        return;

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

void Backend::openRequested(const QString id)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloads[rowForId(id)].savePath));
}

void Backend::removeRequested(const QString id)
{
    auto reply = QMessageBox::warning(nullptr, "Remove Download", "This download will be removed.\nAre you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No)
        return;

    m_downloadModel.removeRow(rowForId(id));
}

void Backend::pauseAll() {
    for (auto &download : m_downloads)
    {
        if (m_currentCategory != 0 &&
            static_cast<int>(download.category) + 1!= m_currentCategory)
        {
            continue;
        }

        const QString id = download.ID;

        Downloader *downloader =
            m_activeDownloaders.value(id, nullptr);

        if (!downloader)
            continue;

        if (download.status != "Paused" &&
            download.status != "Completed" &&
            download.status != "Failed")
        {
            downloader->downloadPause();

            int row = rowForId(id);

            if (row != -1)
            {
                download.status = "Paused";
                m_downloadModel.updateDownload(id);
            }
        }
    }

    emit countChanged();
}

void Backend::resumeAll() {
    for (auto &download : m_downloads)
    {
        if (m_currentCategory != 0 &&
            static_cast<int>(download.category) + 1 != m_currentCategory)
        {
            continue;
        }

        const QString id = download.ID;
        Downloader *downloader = m_activeDownloaders.value(id, nullptr);

        if (!downloader)
            continue;

        if (download.status == "Paused")
        {
            downloader->downloadResume(downloader->downloadInfo());
            download.status = "Downloading...";
        }

        emit countChanged();
        m_downloadModel.updateDownload(download.ID);
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
    for (const auto &download : m_downloads)
    {
        if (m_currentCategory != 0 &&
            static_cast<int>(download.category) + 1 != m_currentCategory)
        {
            continue;
        }

        if (download.status == "Paused")
            count++;
    }
    return count;
}

int Backend::activeCount() const
{
    int count = 0;
    for (const auto &download : m_downloads)
    {
        if (m_currentCategory != 0 &&
            static_cast<int>(download.category) + 1 != m_currentCategory)
        {
            continue;
        }

        if (download.status == "Downloading..." || download.status == "Starting...")
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

bool Backend::headReqCompleted() const
{
    return m_headReqCompleted;
}

void Backend::setHeadReqCompleted(bool val)
{
    m_headReqCompleted = val;
    emit headReqCompletedChanged();
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