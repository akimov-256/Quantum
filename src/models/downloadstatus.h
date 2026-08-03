#ifndef DOWNLOADSTATUS_H
#define DOWNLOADSTATUS_H

#include <QString>
#include <QMetaType>
#include <QList>
#include <QFile>

class Downloader;


struct Part {
    qint64 start;
    qint64 end;
    bool used = false;
    bool done = false;
};

struct downloadInformations
{
    // Display info:
    QString fileName;
    QString status;
    QString Date;
    QString fileSize;
    qint64 currentSize = 0;
    qint64 speed = 0;
    float disk;
    float progress;

    // Technical info:
    QString ID;
    QString url;
    QString savePath;
    QString tempPath;
    qint64 fileByteSize;
    int chunkCount;
    QVector<qint64> chunkProgress;
    QString SHA256;
    QList<Part> fileParts;
};

Q_DECLARE_METATYPE(downloadInformations)

#endif // DOWNLOADSTATUS_H
