#include <QCoreApplication>
#include <QJsonObject>

#include "nativemessaging.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    NativeMessaging *nMsg;                  // Create native messaging class.

    QJsonObject message;                    // Create the message json object.
    while (nMsg->readMessage(message))      // Loop to read messages.
    {
        qDebug() << message;                // Print the read message.
    }

    return 0;
}