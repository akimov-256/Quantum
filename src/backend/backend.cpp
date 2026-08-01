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
    info.savePath += "/" + fileName;
    info.url = fileUrl;
    info.SHA256 = SHA256;
    info.chunkCount = connections;
    info.status = "Starting...";
    info.progress = 0;

    m_downloads.append(info);
    int row = m_downloads.size() - 1;
    m_downloadModel.addDownload(row);

    Downloader *downloader = new Downloader(this);
    m_activeDownloaders.append(downloader);

    downloader->download(info);

    emit countChanged();

    connect(downloader, &Downloader::progressChanged, this, [this, row](qint64 bytesReceived, qint64 bytesTotal) {
        m_downloads[row].fileByteSize = bytesTotal;
        m_downloads[row].currentSize = bytesReceived;
        if (bytesTotal > 0)
            m_downloads[row].progress = static_cast<double>(bytesReceived) * 100.0 / bytesTotal;
        m_downloadModel.updateDownload(row);
    });

    connect(downloader, &Downloader::speedChanged, this, [this, row](qint64 bps) {
        m_downloads[row].speed = bps;
        m_downloadModel.updateDownload(row);
    });

    connect(downloader, &Downloader::downloadFinished, this, [this, row, downloader](bool success, const QString &message) {
        m_downloads[row].status = success ? "Completed" : "Failed";
        emit countChanged();
        m_downloadModel.updateDownload(row);
        qDebug() << message;

        // Clean up
        downloader->deleteLater();
        m_activeDownloaders[row] = nullptr;
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

void Backend::buttonClicked(const int row) {
    if (row < 0 || row >= m_activeDownloaders.size())
        return;

    Downloader *downloader = m_activeDownloaders[row];
    if (!downloader)
        return;

    if (downloader->downloadInfo().status == "Paused")
    {
        downloader->downloadResume(downloader->downloadInfo());
        m_downloads[row].status = "Downloading...";
    }
    else
    {
        downloader->downloadPause();
        m_downloads[row].status = "Paused";
    }

    emit countChanged();
    m_downloadModel.updateDownload(row);
}

void Backend::pauseAll() {
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        Downloader *downloader = m_activeDownloaders[i];

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
        Downloader *downloader = m_activeDownloaders[i];

        if (downloader->downloadInfo().status == "Paused")
        {
            downloader->downloadResume(downloader->downloadInfo());
            m_downloads[i].status = "Downloading...";
        }

        emit countChanged();
        m_downloadModel.updateDownload(i);
    }
}

int Backend::pausedCount() const
{
    int count = 0;
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        if (m_activeDownloaders[i]->downloadInfo().status == "Paused")
            count++;
    }
    return count;
}

int Backend::activeCount() const
{
    int count = 0;
    for (int i = 0; i < m_activeDownloaders.size(); i++)
    {
        if (m_activeDownloaders[i]->downloadInfo().status != "Paused")
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