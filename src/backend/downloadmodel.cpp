#include "downloadmodel.h"

DownloadModel::DownloadModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int DownloadModel::rowCount(const QModelIndex &) const
{
    return m_filteredDownloads.size();
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 ||
        index.row() >= m_filteredDownloads.size())
    {
        return {};
    }

    const auto &download = m_filteredDownloads.at(index.row());

    switch (role)
    {
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
    beginResetModel();

    m_currentCategory = category;

    m_filteredDownloads.clear();

    for (const auto &download : *m_downloads) {
        if (category == 0 ||
            static_cast<int>(download.category) + 1 == category)
        {
            m_filteredDownloads.push_back(download);
        }
    }

    endResetModel();
}

QHash<int, QByteArray> DownloadModel::roleNames() const
{
    return {
        { FileNameRole, "fileName" },
        { ProgressRole, "progress" },
        { SpeedRole, "speed" },
        { FileSizeRole, "fileSize" },
        { DownloadedRole, "downloaded" },
        { StatusRole, "status" }
    };
}