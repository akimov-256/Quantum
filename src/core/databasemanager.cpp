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

    if (!query.exec(sql))
        qDebug() << "Database query error: " << query.lastError().text();
    else
        qDebug() << "Tables Created";
}