#ifndef NATIVEMESSAGING_H
#define NATIVEMESSAGING_H

#include <QObject>
#include <iostream>

class NativeMessaging
{
public:
    NativeMessaging();

    static bool readMessage(QJsonObject &message);
    static bool sendMessage(const QJsonObject &message);
};

#endif // NATIVEMESSAGING_H
