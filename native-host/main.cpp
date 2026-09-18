#include <io.h>
#include <fcntl.h>
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QObject>
#include <QLocalSocket>

#include "nativemessaging.h"

void forwardUrlToBackend(const QString &url, NativeMessaging *nMsg)
{
    QLocalSocket socket;                    // Create the local socket.
    socket.connectToServer("QuantumIPC");   // Connect to the main app IPC.

    if (!socket.waitForConnected(1000)) {   // Handle connection failures.
        nMsg->sendMessage(
            {{"status", "error"},
            {"message", "Quantum is not running"}});
        return;
    }

    QByteArray payload                      // Create the message body.
        = QJsonDocument(QJsonObject{{"url", url}})
            .toJson(QJsonDocument::Compact);
    quint32 len                             // Get the message lenght.
        = static_cast<quint32>(payload.size());

    socket.write(                           // Write the size of the message body
        reinterpret_cast<char *>(&len),     // in the first 4 bytes of the message.
        sizeof(len));
    socket.write(payload);                  // Write the message body next.
    socket.waitForBytesWritten();           // Wait for the writing to finish on the socket
    socket.disconnectFromServer();          // Disconnect on finish.

    nMsg->sendMessage({{"status", "ok"}});  // Send an "ok" status reply.
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

#ifdef _WIN32                               // Handle windows binary mode.
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    NativeMessaging *nMsg                   // Create native messaging class pointer.
        = new NativeMessaging();
    QNetworkAccessManager manager;          // Create network manager.

    QObject::connect(nMsg, &::NativeMessaging::messageRecieved, [&](QJsonObject message) {
        const QString url                   // Get the url from the recieved message.
            = message.value("url").toString();
        if (url.isEmpty())
        {
            nMsg                            // Handle empty url failure.
                ->sendMessage({{"status", "error"}, {"message", "no url provided"}});
            return;
        }

        forwardUrlToBackend(url, nMsg);     // Call the forward function.
    });

    QObject::connect(                       // Close the native host app on web integration connection lost.
        nMsg,
        &NativeMessaging::connectionLost,
        &app, &QCoreApplication::quit);

    nMsg->start();                          // Execute the running loop of the native host message handling.

    return app.exec();
}