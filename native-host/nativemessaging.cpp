#include "nativemessaging.h"

#include <QJsonObject>

NativeMessaging::NativeMessaging() {}

bool NativeMessaging::readMessage(QJsonObject &message)
{
    quint32 messageLength = 0;              // Create the message length variable.

    if (!std::cin.read(                     // Try to read the first 4 byte section of the message
            reinterpret_cast<char *>        // to and convert it then assign it to the message length.
                (&messageLength),
            sizeof(messageLength)))
        return false;                       // If failed return false.

    QByteArray buffer                       // Create the buffer.
        (messageLength, Qt::Uninitialized);

    if (!readExactly(                       // Read the second section "the actual message" and assign
            buffer.data(),                  // it to the buffer variable.
            messageLength))
        return false;                       // If failed return false.

    QJsonParseError error;                  // Create the error variable to handle failures later.

    QJsonDocument doc                       // Convert the buffer to a json document and collect errors.
        = QJsonDocument::fromJson(buffer, &error);
    if (error.error                         // Check if convertion failed and return false.
            != QJsonParseError::NoError)
    {
        qDebug() << error.errorString();
        return false;
    }

    if (!doc.isObject())                    // Check if the result document is a json object (safe guard).
        return false;

    message = doc.object();                 // Assign the result object to the message parameter passed to the function.

    return true;                            // True if read is successful.
}

bool NativeMessaging::readExactly(char *buffer, qint64 size)
{
    qint64 total = 0;                       // Keep track of the recieved bytes so far.
    while (total < size) {                  // Loop as long as there are bytes left.
        std::cin.read                       // Read based on the current progress.
            (buffer + total, size - total);
        if (std::cin.gcount() <= 0)         // Safe guard if the amount of read bytes is 0 or below.
            return false;
        total += std::cin.gcount();         // Add the amount of read bytes to the total.
    }
    return true;                            // Return true when the read is successful.
}