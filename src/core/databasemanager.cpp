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
    query.bindValue(":start_date", QDateTime::currentDateTime().toSecsSinceEpoch());

    if (!query.exec())
        qDebug() << "Database insert error: " << query.lastError().text();
}