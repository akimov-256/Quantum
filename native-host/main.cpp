#include <io.h>
#include <fcntl.h>
#include <QCoreApplication>
#include <QJsonObject>

#include "nativemessaging.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

#ifdef _WIN32                               // Handle windows binary mode.
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    NativeMessaging *nMsg                   // Create native messaging class pointer.
        = new NativeMessaging();

    QJsonObject message;                    // Create the message json object.
    while (nMsg->readMessage(message))      // Loop to read messages.
    {
        qDebug() << message;                // Print the read message.
    }

    return 0;
}