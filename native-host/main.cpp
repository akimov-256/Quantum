#include <io.h>
#include <fcntl.h>
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>

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
    // while (nMsg->readMessage(message))      // Loop to read messages.
    // {
    //     qDebug() << message;                // Print the read message.

    //     QJsonObject reply;                  // Prepare the response message.
    //     reply["status"] = "ok";             // Populate the status field in the response.

    //     if (!nMsg->sendMessage(reply))      // Send the reply and handle failures.
    //     {
    //         qDebug() << "Error sending reply";
    //         break;
    //     }
    // }

    nMsg->run();

    return app.exec();
}