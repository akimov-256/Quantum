#include "databasemanager.h"

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject{parent}
    , m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    const QString dataDir =                 // Get the data directory path.
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir;
    dir.mkpath(dataDir);                    // Ensure the directory exists.

    const QString path =                    // Create the database path inside the data directory.
        dataDir + "/database.qtm";

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
        "connections_progress STRING,"
        "file_parts STRING,"
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
        "INSERT INTO downloads (id, name, url, path, temp_path, size, downloaded, progress, connections, connections_progress, file_parts, category, sha256, start_date)"
        "values (:id, :name, :url, :path, :temp_path, :size, :downloaded, :progress, :connections, :connections_progress, :file_parts, :category, :sha256, :start_date)");

    // Bind values.
    // Set the start date as the date of insertion.
    // Serialize connections progress vector.
    query.bindValue(":id", info.ID);
    query.bindValue(":name", info.fileName);
    query.bindValue(":url", info.url);
    query.bindValue(":path", info.savePath);
    query.bindValue(":temp_path", info.tempPath);
    query.bindValue(":size", info.fileByteSize);
    query.bindValue(":downloaded", info.currentSize);
    query.bindValue(":progress", info.progress);
    query.bindValue(":connections", info.chunkCount);
    QString conProgressSerialized = serializeVector(info.chunkProgress);
    query.bindValue(":connections_progress", conProgressSerialized);
    QString filePartsSerialized = serializeParts(info.fileParts);
    query.bindValue(":file_parts", filePartsSerialized);
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
                  "size = :size, "
                  "connections_progress = :connections_progress, "
                  "file_parts = :file_parts, "
                  "progress = :progress, "
                  "status = :status "
                  "WHERE id = :id");

    // Bind values.
    // Serialize connections progress vector.
    // Serialize file parts list.
    query.bindValue(":downloaded", info.currentSize);
    query.bindValue(":size", info.fileByteSize);
    QString conProgressSerialized = serializeVector(info.chunkProgress);
    query.bindValue(":connections_progress", conProgressSerialized);
    QString filePartsSerialized = serializeParts(info.fileParts);
    query.bindValue(":file_parts", filePartsSerialized);
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
                  "connections_progress, "
                  "file_parts, "
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
        // Deserialize connectons progress and assign it to the according property.
        // If the donwload status is not completed or paused then make it paused,
        // because mostly the app was closed in the middle of a download.
        download.ID = query.value("id").toString();
        download.fileName = query.value("name").toString();
        download.url = query.value("url").toString();
        download.savePath = query.value("path").toString();
        download.tempPath = query.value("temp_path").toString();
        download.fileByteSize = query.value("size").toLongLong();
        download.currentSize = query.value("downloaded").toLongLong();
        download.progress = query.value("progress").toInt();
        download.chunkCount = query.value("connections").toInt();
        QString conProgressSerialized = query.value("connections_progress").toString();
        download.chunkProgress = deserializeVector(conProgressSerialized);
        QString filePartsSerialized = query.value("file_parts").toString();
        download.fileParts = deserializeParts(filePartsSerialized);
        download.category = static_cast<DownloadCategory>(query.value("category").toInt());
        download.SHA256 = query.value("sha256").toString();
        QString status = query.value("status").toString();
        if (status != "Completed" || status != "Paused")
            status = "Paused";
        download.status = status;

        result.push_back(download);         // Add the extracted download to the result list.
    }

    return result;                          // Return the result vector.
}

QString DatabaseManager::serializeVector(const QVector<qint64> &vector)
{
    QStringList stringList;                 // Create the serialized numbers list.

    for (const qint64 &number : vector)     // Loop through the vector contents.
    {
        QString serialized =                // Convert each number to a string.
            QString::number(number);

        stringList.append(serialized);      // Add the serialized number to the serialized numbers list.
    }

    QString result = stringList.join(",");  // Join all the serialized number to a result string.

    return result;                          // Return the result string.
}

QVector<qint64> DatabaseManager::deserializeVector(const QString &serialized)
{
    QVector<qint64> result;                     // Create the result vector.

    QStringList stringList =                    // Create the serialized numbers list.
        serialized.split(",");

    for (const QString &string : stringList)    // Loop through the serialized numbers list.
    {
        qint64 number = string.toLongLong();    // Convert each string back to a longlong number.

        result.append(number);                  // Add the converted number to the result list.
    }

    return result;                              // Return the result vector.
}

QString DatabaseManager::serializeParts(const QList<Part> &parts)
{
    QStringList partsSerialized;                // Create the serialized strings list.

    for (const Part &part : parts)              // Loop through download parts.
    {
        QStringList contentSerialized;          // Create the serialized content strings list.

        // Populate the list with the part properties.
        contentSerialized.append(QString::number(part.start));
        contentSerialized.append(QString::number(part.end));
        if (part.used)
            contentSerialized.append("1");
        else
            contentSerialized.append("0");
        if(part.done)
            contentSerialized.append("1");
        else
            contentSerialized.append("0");

        // Convert the list to a string by joining the serialized contents with ",".
        QString res = contentSerialized.join(",");
        // Append the string to the global parts list.
        partsSerialized.append(res);
    }

    // Convert the list to a string by joining the serialized parts with "|".
    QString result = partsSerialized.join("|");

    return result;                              // Return the result string.
}

QList<Part> DatabaseManager::deserializeParts(const QString &serialized)
{
    if (serialized.isEmpty())                   // Guard incase the input is empty.
        return  {};

    QList<Part> result;                         // Create the result list.

    QStringList parts = serialized.split("|");  // Separate each part from the serialized input.

    for (const QString &partStr : parts)
    {
        QStringList contents =                  // Separate each part property from the current part.
            partStr.split(",");

        Part part;                              // Create the part.

        // Assign each property from the serialized string back to the part.
        part.start = contents[0].toLongLong();
        part.end = contents[1].toLongLong();
        part.used = contents[2] == "1";
        part.done = contents[3] == "1";

        result.append(part);                    // Add the part to the result list.
    }

    return result;                              // Return the result list.
}