#include "downloadmodel.h"

DownloadModel::DownloadModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int DownloadModel::rowCount(const QModelIndex &) const
{
    if (!m_downloads)
        return 0;
    return m_downloads->size();
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    if (!m_downloads)
        return {};
    if (!index.isValid())
        return {};
    if (index.row() < 0 || index.row() >= m_downloads->size())
        return {};

    const auto &download = m_downloads->at(index.row());

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