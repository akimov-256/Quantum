#ifndef NATIVEMESSAGING_H
#define NATIVEMESSAGING_H

#include <QObject>
#include <iostream>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>

class NativeMessaging : public QObject
{
    Q_OBJECT

public:
    NativeMessaging(QObject *parent = nullptr);

    void run();

    bool sendMessage(const QJsonObject &message);

signals:
    void messageRecieved(QJsonObject message);
    void connectionLost();

private:
    // Functions
    bool readMessage(QJsonObject &message);
    static bool readExactly(char *buffer, qint64 size);
};

#endif // NATIVEMESSAGING_H
