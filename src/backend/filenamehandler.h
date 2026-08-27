#ifndef FILENAMEHANDLER_H
#define FILENAMEHANDLER_H

#include <QObject>
#include <QFileInfo>

class FileNameHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileNameHandler(QObject *parent = nullptr);

    QString CheckFile(const QString &filePath);


private:
    QFileInfo m_currentFileInfo;
    int m_currentDuplicate = 1;

signals:
};

#endif // FILENAMEHANDLER_H
