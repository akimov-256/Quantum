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

private:
    // Functions
    static bool readExactly(char *buffer, qint64 size);
};

#endif // NATIVEMESSAGING_H
