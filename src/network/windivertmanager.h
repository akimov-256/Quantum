#ifndef WINDIVERTMANAGER_H
#define WINDIVERTMANAGER_H

#include <QObject>
#include <qDebug>
#include <QThread>
#include <QAtomicInt>

#include "third-party/WinDivert/include/windivert.h"

class WinDivertManager : public QObject
{
    Q_OBJECT
public:
    explicit WinDivertManager(QObject *parent = nullptr);
    ~WinDivertManager();

    bool start();
    void stop();

signals:

private slots:
    void run();

private:
    // Functions

    // Variables
    void *m_handle = nullptr;
    QThread *m_WinDivertWorker;
    QAtomicInteger<bool> m_running = false;
};

#endif // WINDIVERTMANAGER_H
