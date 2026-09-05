#ifndef WINDIVERTMANAGER_H
#define WINDIVERTMANAGER_H

#include <QObject>

class WinDivertManager : public QObject
{
    Q_OBJECT
public:
    explicit WinDivertManager(QObject *parent = nullptr);

signals:
};

#endif // WINDIVERTMANAGER_H
