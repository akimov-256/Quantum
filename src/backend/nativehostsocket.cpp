#include "nativehostsocket.h"

NativeHostSocket::NativeHostSocket(QObject *parent)
    : QObject{parent}
{}

void NativeHostSocket::StartWebServer()
{
    m_localServer = new QLocalServer(this);
    QLocalServer::removeServer("QuantumIPC");   // clear a stale pipe from a crashed previous run

    connect(m_localServer, &QLocalServer::newConnection, this, [this]() {
        QLocalSocket *socket = m_localServer->nextPendingConnection();

        connect(socket, &QLocalSocket::readyRead, this, [socket, this]() {
            QByteArray &buf = m_ipcBuffers[socket];
            buf.append(socket->readAll());

            while (buf.size() >= 4) {
                quint32 len;
                memcpy(&len, buf.constData(), sizeof(len));

                if (buf.size() < int(sizeof(len) + len))
                    break;   // full message hasn't arrived yet, wait for more

                QByteArray payload = buf.mid(sizeof(len), len);
                buf.remove(0, sizeof(len) + len);

                QJsonDocument doc = QJsonDocument::fromJson(payload);
                if (doc.isObject())
                    emit urlRecieved(doc.object()["url"].toString());
            }
        });

        connect(socket, &QLocalSocket::disconnected, this, [socket, this]() {
            m_ipcBuffers.remove(socket);
            socket->deleteLater();
        });
    });

    if (!m_localServer->listen("QuantumIPC"))
        qWarning() << "Could not start local IPC server:" << m_localServer->errorString();
    else
        qDebug() << "Quantum IPC server listening on pipe QuantumIPC";
}