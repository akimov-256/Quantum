#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
    , manager(new QNetworkAccessManager(this))
{}

void Backend::CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const QString &SHA256)
{    
    downloadInformations info;

    info.fileName = fileName;
    if(filePath.isEmpty())
        info.savePath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    else
        info.savePath = filePath;
    info.url = fileUrl;
    info.SHA256 = SHA256;
    info.chunkCount = 8;

    m_downloader.download(info);
}

void Backend::GetHeadInfo(const QString &fileUrl)
{
    QUrl url = QUrl::fromUserInput(fileUrl);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->head(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray disposition = reply->rawHeader("Content-Disposition");

        QRegularExpression re(R"(filename\*?=(?:UTF-8''|")?([^";]+))");
        QRegularExpressionMatch match = re.match(QString::fromUtf8(disposition));

        if (match.hasMatch())
            m_fileName = QUrl::fromPercentEncoding(match.captured(1).toUtf8());
        else
            m_fileName = QFileInfo(reply->url().path()).fileName();

        if (m_fileName.isEmpty()) m_fileName = "download";

        emit fileNameChanged();

        reply->deleteLater();
    });
}

QString Backend::fileName() const
{
    return m_fileName;
}