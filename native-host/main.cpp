#include <io.h>
#include <fcntl.h>
#include <QCoreApplication>
#include <QJsonObject>
#include <QDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QObject>

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

        QNetworkRequest request(            // Create request to the HTTP localhost server.
            QUrl("http://127.0.0.1:8421/download"));
        request.setHeader(                  // Add headers to the request.
            QNetworkRequest::ContentTypeHeader,
            "application/json");

        const QJsonObject                   // Create the reply body.
            body{{"url", url}};
        QNetworkReply *reply                // Send the body and create a pointer to the reply.
            = manager.post(request,
                QJsonDocument(body).toJson());

        // Connect the reply finish to a lambda function.
        QObject::connect(reply, &QNetworkReply::finished, [reply, &nMsg]() {
            if (reply->error() == QNetworkReply::NoError)
                nMsg->sendMessage(          // On reply success send a ok message to native host.
                    {{"status", "ok"}});
            else
                nMsg->sendMessage(          // On reply failure send the error to native host.
                    {{"status", "error"},
                        {"message", reply->errorString()}});

            reply->deleteLater();           // Free the reply pointer.
        });
    });

    QObject::connect(                       // Close the native host app on web integration connection lost.
        nMsg,
        &NativeMessaging::connectionLost,
        &app, &QCoreApplication::quit);

    nMsg->run();                            // Execute the running loop of the native host message handling.

    return app.exec();
}