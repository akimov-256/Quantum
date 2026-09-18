#ifndef NATIVEHOSTSOCKET_H
#define NATIVEHOSTSOCKET_H

#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>

class NativeHostSocket : public QObject
{
    Q_OBJECT
public:
    explicit NativeHostSocket(QObject *parent = nullptr);

    void StartWebServer();

private:
    QLocalServer *m_localServer = nullptr;
    QHash<QLocalSocket*, QByteArray> m_ipcBuffers;

signals:
    void urlRecieved(const QString &url);
};

#endif // NATIVEHOSTSOCKET_H
