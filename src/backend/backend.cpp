#include "backend.h"

Backend::Backend(QObject *parent)
    : QObject{parent}
{}

void Backend::CreateDownload(const QString fileUrl, const QString fileName, const QString filePath, const QString SHA256)
{
    downloadInformations info;

    info.fileName = fileName;
    info.savePath = filePath;
    info.url = fileUrl;
    info.SHA256 = SHA256;

    m_downloader.download(info);
}