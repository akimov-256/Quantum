#include "windivertmanager.h"

WinDivertManager::WinDivertManager(QObject *parent)
    : QObject{parent}
{}

bool WinDivertManager::start()
{
    m_handle = WinDivertOpen(       // Populate the handle variable.
        "tcp",
        WINDIVERT_LAYER_NETWORK,
        0,
        0);

    if (!m_handle)                  // Guard if the handle is unvalid.
    {
        qDebug()
            << "Failed to open WinDivert handle: "
            << GetLastError();
        return false;
    }

    m_running = true;               // Mark WinDivert as running.
    run();                          // Call the run function.

    return true;
}

void WinDivertManager::stop()
{
    m_running = false;                  // Mark WinDivert as not running.

    if (m_handle)                       // Check if the handle is valid.
    {
        if (WinDivertClose(m_handle))   // Try closing the handle.
        {
            m_handle = nullptr;         // If successful free the handle variable.
        }
        else
        {
            qDebug()                    // If failed print the error code.
                << "Failed to close WinDivert handle: "
                << GetLastError();
        }
    }
}

WinDivertManager::~WinDivertManager()   // Stop the handle when the destructor is called.
{
    stop();                             // Call the stop function.
}
