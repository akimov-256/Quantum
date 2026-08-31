#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject{parent}
    , m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    const QString path =                    // Create the database path.
        QDir::currentPath() + "/" + "database.db";

    m_db.setDatabaseName(path);             // Set the database path.

    if (!m_db.open())                       // Open database and handle failures.
        qDebug() << "Database error: " << m_db.lastError().text();

    initDatabase();
}

void DatabaseManager::initDatabase()
{
    QSqlQuery query;                        // Create the query variable.

    QString sql =                           // Create the query text.
        "CREATE TABLE IF NOT EXISTS downloads ("
        "id TEXT NOT NULL,"
        "name TEXT NOT NULL,"
        "url TEXT NOT NULL,"
        "size INTEGER NOT NULL,"
        "connections INTEGER NOT NULL,"
        "status TEXT,"
        "sha256 TEXT,"
        "start_date INTEGER NOT NULL,"
        "end_date INTEGER"
        ");";

    if (!query.exec(sql))                   // Execute the qurey and handle failures.
        qDebug() << "Database query error: " << query.lastError().text();
}

void DatabaseManager::insertDownload(const downloadInformations &info)
{
    QSqlQuery query;                        // Create the query variable.

    query.prepare(                          // Prepare the query.
        "INSERT INTO downloads (id, name, url, size, connections, sha256, start_date)"
        "values (:id, :name, :url, :size, :connections, :sha256, :start_date)");

    // Bind values.
    query.bindValue(":id", info.ID);
    query.bindValue(":name", info.fileName);
    query.bindValue(":url", info.url);
    query.bindValue(":size", info.fileByteSize);
    query.bindValue(":connections", info.chunkCount);
    query.bindValue(":sha256", info.SHA256);
    query.bindValue(":start_date", QDateTime::currentSecsSinceEpoch());

    if (!query.exec())
        qDebug() << "Database insert error: " << query.lastError().text();
}

void DatabaseManager::downloadFinished(const QString &id, const QString &status)
{
    QSqlQuery query;

    query.prepare("UPDATE downloads SET "
                  "status = :status, "
                  "end_date = :end_date "
                  "WHERE id = :id");

    query.bindValue(":status", status);
    query.bindValue(":end_date", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":id", id);

    if (!query.exec())
        qDebug() << "Database update error: " << query.lastError().text();
}