#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "src/models/downloadstatus.h"

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDir>
#include <QDateTime>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);

    void insertDownload(const downloadInformations &info);
    void downloadFinished(const QString &id, const QString &status);

private:
    // Functions
    void initDatabase();

    // Variables
    QSqlDatabase m_db;              // Define the database variable

signals:
};

#endif // DATABASEMANAGER_H
