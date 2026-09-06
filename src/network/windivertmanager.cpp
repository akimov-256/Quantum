#include "windivertmanager.h"

WinDivertManager::WinDivertManager(QObject *parent)
    : QObject{parent}
    , m_WinDivertWorker(new QThread)
{}

bool WinDivertManager::start()
{
    m_handle = WinDivertOpen(           // Populate the handle variable.
        "true",
        WINDIVERT_LAYER_NETWORK,
        0,
        0);

    if (!m_handle)                      // Guard if the handle is unvalid.
    {
        qDebug()
            << "Failed to open WinDivert handle: "
            << GetLastError();
        return false;
    }

    m_running = true;                   // Mark WinDivert as running.
    moveToThread(m_WinDivertWorker);    // Move WinDivertManager to a different thread.

    connect(m_WinDivertWorker,          // Start the running loop when the thread is ready.
            &QThread::started,
            this,
            &WinDivertManager::run);

    connect(m_WinDivertWorker,          // Stop WinDivertManager when the thread terminates.
            &QThread::finished,
            this,
            &WinDivertManager::stop);

    m_WinDivertWorker->start();         // Start WinDivert thread.

    return true;
}

void WinDivertManager::run()
{
    char packet[65535];
    UINT packetLength;
    WINDIVERT_ADDRESS address{};

    while (m_running)
    {
        if (!WinDivertRecv(             // Recieve the packet and handle failures.
                m_handle,
                packet,
                sizeof(packet),
                &packetLength,
                &address
                ))
        {
            qDebug()                    // Print the error.
                << "WinDivertRecv failed with error: "
                << GetLastError();
            continue;
        }

        qDebug()                        // Print out the packet info.
            << "Captured packet: "
            << packetLength
            << " bytes\n";

        WinDivertSend(                  // Return the packet to the original destination.
            m_handle,
            packet,
            packetLength,
            nullptr,
            &address);
    }
}

void WinDivertManager::stop()
{
    m_running = false;                  // Mark WinDivert as not running.

    if (m_handle)                       // Check if the handle is valid.
    {
        if (WinDivertClose(m_handle))   // Try closing the handle.
        {
            m_handle = nullptr;         // If successful free the handle variable.

            if (m_WinDivertWorker)      // If thread is still running quit it and wait.
            {
                m_WinDivertWorker->quit();
                m_WinDivertWorker->wait();
            }
        }
        else
        {
            qDebug()                    // If failed print the error code.
                << "Failed to close WinDivert handle: "
                << GetLastError();
        }
    }

    delete m_WinDivertWorker;           // Delete the thread pointer.
}

WinDivertManager::~WinDivertManager()   // Stop the handle when the destructor is called.
{
    stop();                             // Call the stop function.
}
