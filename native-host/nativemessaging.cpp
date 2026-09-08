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

    if (!std::cin.read(                     // Read the second section "the actual message" and assign
            buffer.data(),                  // it to the buffer variable.
            messageLength))
        return false;                       // If failed return false.

    QJsonParseError error;                  // Create the error variable to handle failures later.

    QJsonDocument doc                       // Convert the buffer to a json document.
        = QJsonDocument::fromJson(buffer);
    if (error.error                         // Check if convertion failed and return false.
            != QJsonParseError::NoError)
        return false;

    if (!doc.isObject())                    // Check if the result document is a json object (safe guard).
        return false;

    message = doc.object();                 // Assign the result object to the message parameter passed to the function.

    return true;                            // True if read is successful.
}