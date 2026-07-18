#ifndef BACKEND_H
#define BACKEND_H

#include "src/models/downloadstatus.h"
#include "src/backend/downloadmodel.h"
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
    Q_PROPERTY(DownloadModel* downloadModel READ downloadModel CONSTANT)
    Q_PROPERTY(int downloadCount READ downloadCount NOTIFY countChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY countChanged)

public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const QString &SHA256);
    Q_INVOKABLE void GetHeadInfo(const QString &fileUrl);
    QString fileName() const;
    qint64 fileSize() const;
    DownloadModel *downloadModel();
    bool isHeadReqActive() const;
    int downloadCount() const;
    int completedCount() const;
signals:
    void fileNameChanged();
    void fileSizeChanged();
    void isHeadReqActiveChanged();
    void countChanged();

private:
    QNetworkAccessManager *manager;
    QList<Downloader*> m_activeDownloaders;
    QString m_fileName;
    qint64 m_fileSize = 0;
    bool m_isHeadReqActive = false;
    QList<downloadInformations> m_downloads;
    DownloadModel m_downloadModel;
};

#endif // BACKEND_H
