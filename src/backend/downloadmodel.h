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
        IDRole = Qt::UserRole + 1,
        FileNameRole,
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
    void removeRow(int row);
    void setCategory(int category);

    QHash<int, QByteArray> roleNames() const override;

private:
    QList<downloadInformations> *m_downloads = nullptr;
    QList<QString> m_filteredDownloadIDs;
    int m_currentCategory;

signals:
};

#endif // DOWNLOADMODEL_H
