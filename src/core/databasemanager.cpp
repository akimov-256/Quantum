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
        "category INTEGER NOT NULL,"
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
        "INSERT INTO downloads (id, name, url, size, connections, category, sha256, start_date)"
        "values (:id, :name, :url, :size, :connections, :category, :sha256, :start_date)");

    // Bind values.
    // Set the start date as the date of insertion.
    query.bindValue(":id", info.ID);
    query.bindValue(":name", info.fileName);
    query.bindValue(":url", info.url);
    query.bindValue(":size", info.fileByteSize);
    query.bindValue(":connections", info.chunkCount);
    query.bindValue(":category", info.category);
    query.bindValue(":sha256", info.SHA256);
    query.bindValue(":start_date", QDateTime::currentSecsSinceEpoch());

    if (!query.exec())
        qDebug() << "Database insert error: " << query.lastError().text();
}

void DatabaseManager::downloadFinished(const QString &id, const QString &status)
{
    QSqlQuery query;                        // Create the query variable.

    query.prepare("UPDATE downloads SET "   // Prepare the query.
                  "status = :status, "
                  "end_date = :end_date "
                  "WHERE id = :id");

    // Bind values.
    // Set the end date as the date of modification.
    query.bindValue(":status", status);
    query.bindValue(":end_date", QDateTime::currentSecsSinceEpoch());
    query.bindValue(":id", id);

    if (!query.exec())                      // Execute query and handle failures.
        qDebug() << "Database update error: " << query.lastError().text();
}

void DatabaseManager::removeDownload(const QString &id)
{
    QSqlQuery query;                        // Create the query variable.

    query.prepare("DELETE FROM downloads "  // Prepare the query.
                  "WHERE id = :id");

    // Bind values.
    query.bindValue(":id", id);

    if (!query.exec())                      // Execute query and handle failures.
        qDebug() << "Database update error:" << query.lastError().text();
}

QVector<downloadInformations> DatabaseManager::getDownloads()
{
    QSqlQuery query;                        // Create the query variable.

    QString sql = "SELECT id, name, url, "  // Create the query statement.
                  "size, connections, "
                  "category, sha256, status "
                  "FROM downloads";

    if (!query.exec(sql))                   // Execute query and handle failures.
        qDebug() << "Database extract error: " << query.lastError().text();

    QVector<downloadInformations> result;   // Create the downloads vector.

    while(query.next())
    {
        downloadInformations download;      // Create the current download info.

        // Get each property and assign it to the download info.
        download.ID = query.value("id").toString();
        download.fileName = query.value("name").toString();
        download.url = query.value("url").toString();
        download.fileByteSize = query.value("size").toLongLong();
        download.chunkCount = query.value("connections").toInt();
        download.category = static_cast<DownloadCategory>(query.value("category").toInt());
        download.SHA256 = query.value("sha256").toString();
        download.status = query.value("status").toString();

        result.push_back(download);         // Add the extracted download to the result list.
    }

    return result;                          // Return the result vector.
}