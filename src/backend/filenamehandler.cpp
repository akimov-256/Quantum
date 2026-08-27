#include "filenamehandler.h"

FileNameHandler::FileNameHandler(QObject *parent)
    : QObject{parent}
{}

QString FileNameHandler::CheckFile(const QString &filePath)
{
    m_currentFileInfo = QFileInfo(filePath);            // Set the global file info to the current file.

    if (!m_currentFileInfo.exists())
        return m_currentFileInfo.fileName();            // Return the original file name if it does not exist.

    QString newName = m_currentFileInfo.baseName();     // Create the new filename properties.
    QString newPath = m_currentFileInfo.path()
                + "/" + newName + "." +
                m_currentFileInfo.completeSuffix();

    QFileInfo newFileInfo(newPath);                     // Create FileInfo for the new name.

    while (newFileInfo.exists())
    {
        newName = m_currentFileInfo.baseName() + "_"    // Create the new name with a numbered suffix.
            + QString::number(m_currentDuplicate)
            + "." + m_currentFileInfo.completeSuffix();
        newPath = m_currentFileInfo.path()              // Construct the new path based on the duplicate name.
            + "/" + newName;

        m_currentDuplicate++;                           // Increment current duplicate after each rename.

        newFileInfo = QFileInfo(newPath);               // Pass the new path to the new name's FileInfo.
    }

    m_currentDuplicate = 1;                             // Reset the current duplicate variable.
    return newName;                                     // Return the name copy with the numbered suffix.
}
