#ifndef BACKEND_H
#define BACKEND_H

#include "src/models/downloadstatus.h"
#include "src/core/downloader.h"
#include <QObject>
#include <QStandardPaths>
#include <QNetworkAccessManager>

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const QString &SHA256);
    Q_INVOKABLE void GetHeadInfo(const QString &fileUrl);
    QString fileName() const;
signals:
    void fileNameChanged();

private:
    QNetworkAccessManager *manager;
    Downloader m_downloader;
    QString m_fileName;
};

#endif // BACKEND_H
