#ifndef NATIVEMESSAGING_H
#define NATIVEMESSAGING_H

#include <QObject>
#include <iostream>
#include <QJsonObject>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>

class NativeMessaging : public QThread
{
    Q_OBJECT

public:
    NativeMessaging(QObject *parent = nullptr);

    void run() override;

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
