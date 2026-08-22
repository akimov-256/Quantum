#include "downloadmodel.h"

DownloadModel::DownloadModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int DownloadModel::rowCount(const QModelIndex &) const
{
    return m_filteredDownloadIDs.size();
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (!m_downloads || !index.isValid())
        return {};

    if (index.row() < 0 ||
        index.row() >= m_filteredDownloadIDs.size())
    {
        return {};
    }

    QString id = m_filteredDownloadIDs.at(index.row());

    const auto it = std::find_if(
        m_downloads->begin(),
        m_downloads->end(),
        [id](const downloadInformations &download) {
            return download.ID == id;
        }
        );

    if (it == m_downloads->end())
        return {};

    const auto &download = *it;

    switch (role)
    {
    case IDRole:
        return download.ID;
    case FileNameRole:
        return download.fileName;
    case ProgressRole:
        return download.progress;
    case SpeedRole:
        return download.speed;
    case FileSizeRole:
        return download.fileByteSize;
    case DownloadedRole:
        return download.currentSize;
    case StatusRole:
        return download.status;
    }

    return {};
}

void DownloadModel::setDownloads(QList<downloadInformations> *downloads)
{
    beginResetModel();

    m_downloads = downloads;

    endResetModel();
}

void DownloadModel::addDownload(int row)
{
    beginInsertRows(QModelIndex(), row, row);
    endInsertRows();
}

void DownloadModel::updateDownload(int row)
{
    emit dataChanged(index(row), index(row));
}

void DownloadModel::removeRow(int row)
{
    if (!m_downloads || row < 0 || row >= m_downloads->size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_downloads->remove(row);
    endRemoveRows();
}

void DownloadModel::setCategory(int category)
{
    if (!m_downloads)
        return;

    beginResetModel();

    m_currentCategory = category;
    m_filteredDownloadIDs.clear();

    for (const auto &download : *m_downloads) {
        if (category == 0 ||
            download.category + 1 == category)
        {
            m_filteredDownloadIDs.append(download.ID);
        }
    }

    endResetModel();
}

QHash<int, QByteArray> DownloadModel::roleNames() const
{
    return {
        { IDRole, "id" },
        { FileNameRole, "fileName" },
        { ProgressRole, "progress" },
        { SpeedRole, "speed" },
        { FileSizeRole, "fileSize" },
        { DownloadedRole, "downloaded" },
        { StatusRole, "status" }
    };
}