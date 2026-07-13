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
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(bool isHeadReqActive READ isHeadReqActive NOTIFY isHeadReqActiveChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const QString &SHA256);
    Q_INVOKABLE void GetHeadInfo(const QString &fileUrl);
    QString fileName() const;
    qint64 fileSize() const;
    bool isHeadReqActive() const;
signals:
    void fileNameChanged();
    void fileSizeChanged();
    void isHeadReqActiveChanged();

private:
    QNetworkAccessManager *manager;
    Downloader m_downloader;
    QString m_fileName;
    qint64 m_fileSize = 0;
    bool m_isHeadReqActive = false;
};

#endif // BACKEND_H
