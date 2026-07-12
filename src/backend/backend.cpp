#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
{}

void Backend::CreateDownload(const QString fileUrl, const QString fileName, const QString filePath, const QString SHA256)
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