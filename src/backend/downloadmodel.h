#ifndef DOWNLOADMODEL_H
#define DOWNLOADMODEL_H

#include "src/models/downloadstatus.h"
#include <QObject>
#include <QAbstractListModel>

class DownloadModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        ProgressRole,
        SpeedRole,
        FileSizeRole,
        DownloadedRole,
        StatusRole
    };

    explicit DownloadModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setDownloads(QList<downloadInformations> *downloads);
    void addDownload(int row);
    void updateDownload(int row);

    QHash<int, QByteArray> roleNames() const override;

private:
    QList<downloadInformations> *m_downloads = nullptr;

signals:
};

#endif // DOWNLOADMODEL_H
