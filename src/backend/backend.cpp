#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
    , manager(new QNetworkAccessManager(this))
{
    m_downloadModel.setDownloads(&m_downloads);
}

void Backend::CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const QString &SHA256)
{
    downloadInformations info;

    info.fileName = fileName;
    if (filePath.isEmpty())
        info.savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    else
        info.savePath = filePath;
    info.url = fileUrl;
    info.SHA256 = SHA256;
    info.chunkCount = 8;
    info.status = "Starting...";
    info.progress = 0;

    m_downloads.append(info);
    int row = m_downloads.size() - 1;
    m_downloadModel.addDownload(row);

    Downloader *downloader = new Downloader(this);
    m_activeDownloaders.append(downloader);

    downloader->download(info);

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
        m_downloadModel.updateDownload(row);
        qDebug() << message;
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