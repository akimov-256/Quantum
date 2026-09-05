#ifndef WINDIVERTMANAGER_H
#define WINDIVERTMANAGER_H

#include "third-party/WinDivert/include/windivert.h"

#include <QObject>
#include <qDebug>

class WinDivertManager : public QObject
{
    Q_OBJECT
public:
    explicit WinDivertManager(QObject *parent = nullptr);
    ~WinDivertManager();

    bool start();
    void stop();

signals:

private:
    // Functions
    void run();

    // Variables
    void *m_handle = nullptr;
    bool m_running = false;
};

#endif // WINDIVERTMANAGER_H
