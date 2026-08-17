#ifndef BACKEND_H
#define BACKEND_H

#include "src/models/downloadstatus.h"
#include "src/backend/downloadmodel.h"
#include "src/core/downloader.h"
#include <QObject>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QRect>
#include <QGuiApplication>
#include <QScreen>
#include <QFile>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QMessageBox>

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(qint64 fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(bool isHeadReqActive READ isHeadReqActive NOTIFY isHeadReqActiveChanged)
    Q_PROPERTY(DownloadModel* downloadModel READ downloadModel CONSTANT)
    Q_PROPERTY(int downloadCount READ downloadCount NOTIFY countChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY countChanged)
    Q_PROPERTY(int pausedCount READ pausedCount NOTIFY countChanged FINAL)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY countChanged FINAL)

public:
    explicit Backend(QObject *parent = nullptr);

    Q_INVOKABLE void CreateDownload(const QString &fileUrl, const QString &fileName, const QString &filePath, const int &connections, const QString &SHA256);
    Q_INVOKABLE void GetHeadInfo(const QString &fileUrl);
    Q_INVOKABLE void buttonClicked(const int row);
    Q_INVOKABLE void cancelClicked(const int row);
    Q_INVOKABLE void openRequested(const int row);
    Q_INVOKABLE void removeRequested(const int row);
    Q_INVOKABLE void pauseAll();
    Q_INVOKABLE void resumeAll();
    Q_INVOKABLE QRect availableScreenGeometry() const;
    Q_INVOKABLE QString coloredSvg(const QString &path, const QString &color);

    QString fileName() const;
    qint64 fileSize() const;
    DownloadModel *downloadModel();
    bool isHeadReqActive() const;
    int downloadCount() const;
    int completedCount() const;
    int pausedCount() const;
    int activeCount() const;
    int rowForId(const QString &id) const;

signals:
    void fileNameChanged();
    void fileSizeChanged();
    void isHeadReqActiveChanged();
    void countChanged();

private:
    QNetworkAccessManager *manager;
    QHash<QString, Downloader*> m_activeDownloaders;
    QString m_fileName;
    qint64 m_fileSize = 0;
    bool m_isHeadReqActive = false;
    QList<downloadInformations> m_downloads;
    DownloadModel m_downloadModel;

};

#endif // BACKEND_H
