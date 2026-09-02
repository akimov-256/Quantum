#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "src/models/downloadstatus.h"

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QDateTime>
#include <QStringList>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);

    void insertDownload(const downloadInformations &info);
    void updateDownload(const downloadInformations &info);
    void removeDownload(const QString &id);
    QVector<downloadInformations> getDownloads();

    // Helper functions
    QString serializeVector(const QVector<qint64> &vector);
    QVector<qint64> deserializeString(const QString &serialized);

private:
    // Functions
    void initDatabase();

    // Variables
    QSqlDatabase m_db;              // Define the database variable

signals:
};

#endif // DATABASEMANAGER_H
