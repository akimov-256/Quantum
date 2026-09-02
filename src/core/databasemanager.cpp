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
        "path TEXT NOT NULL,"
        "temp_path TEXT NOT NULL,"
        "size INTEGER NOT NULL,"
        "downloaded INTEGER NOT NULL,"
        "progress INTEGER NOT NULL,"
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
        "INSERT INTO downloads (id, name, url, path, temp_path, size, downloaded, progress, connections, category, sha256, start_date)"
        "values (:id, :name, :url, :path, :temp_path, :size, :downloaded, :progress, :connections, :category, :sha256, :start_date)");

    // Bind values.
    // Set the start date as the date of insertion.
    query.bindValue(":id", info.ID);
    query.bindValue(":name", info.fileName);
    query.bindValue(":url", info.url);
    query.bindValue(":path", info.savePath);
    query.bindValue(":temp_path", info.tempPath);
    query.bindValue(":size", info.fileByteSize);
    query.bindValue(":downloaded", info.currentSize);
    query.bindValue(":progress", info.progress);
    query.bindValue(":connections", info.chunkCount);
    query.bindValue(":category", info.category);
    query.bindValue(":sha256", info.SHA256);
    query.bindValue(":start_date", QDateTime::currentSecsSinceEpoch());

    if (!query.exec())
        qDebug() << "Database insert error: " << query.lastError().text();
}

void DatabaseManager::updateDownload(const downloadInformations &info)
{
    QSqlQuery query;                        // Create the query variable.

    query.prepare("UPDATE downloads SET "   // Prepare the query.
                  "downloaded = :downloaded, "
                  "progress = :progress, "
                  "status = :status "
                  "WHERE id = :id");

    // Bind values.
    query.bindValue(":downloaded", info.currentSize);
    query.bindValue(":progress", info.progress);
    query.bindValue(":status", info.status);
    query.bindValue(":id", info.ID);

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
                  "path, temp_path, "
                  "size, connections, "
                  "downloaded, progress, "
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
        download.savePath = query.value("path").toString();
        download.tempPath = query.value("temp_path").toString();
        download.fileByteSize = query.value("size").toLongLong();
        download.currentSize = query.value("downloaded").toLongLong();
        download.progress = query.value("progress").toInt();
        download.chunkCount = query.value("connections").toInt();
        download.category = static_cast<DownloadCategory>(query.value("category").toInt());
        download.SHA256 = query.value("sha256").toString();
        download.status = query.value("status").toString();

        result.push_back(download);         // Add the extracted download to the result list.
    }

    return result;                          // Return the result vector.
}