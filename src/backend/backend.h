#ifndef BACKEND_H
#define BACKEND_H

#include "src/models/downloadstatus.h"
#include "src/core/downloader.h"
#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void CreateDownload(const QString fileUrl, const QString fileName, const QString filePath, const QString SHA256);

signals:

private:
    Downloader m_downloader;
};

#endif // BACKEND_H
