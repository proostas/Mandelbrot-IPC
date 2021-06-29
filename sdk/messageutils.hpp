#ifndef MESSAGEUTILS_HPP
#define MESSAGEUTILS_HPP

#include <memory>
#include <vector>

#include <QByteArray>
#include <QTcpSocket>
#include <QDataStream>

#include "message.hpp"

namespace MessageUtils {

inline void sendMessage(QTcpSocket& socket, Message::Type messageType, QByteArray& data, bool forceFlush = false)
{
    Message message(messageType, data);

    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);
    stream << message;
    socket.write(byteArray);
    if (forceFlush) {
        socket.flush();
    }
}

inline void sendMessage(QTcpSocket& socket, Message::Type messageType, bool forceFlush = false)
{
    QByteArray data;
    sendMessage(socket, messageType, data, forceFlush);
}

inline std::unique_ptr<std::vector<std::unique_ptr<Message>>> readMessages(QDataStream& stream)
{
    auto messages = std::make_unique<std::vector<std::unique_ptr<Message>>>();
    bool commitTransaction = true;
    while (commitTransaction && stream.device()->bytesAvailable() > 0) {
        stream.startTransaction();
        auto message = std::make_unique<Message>();
        stream >> *message;
        commitTransaction = stream.commitTransaction();
        if (commitTransaction) {
            messages->push_back(std::move(message));
        }
    }
    return messages;
}

} // namespace MessageUtils

#endif // MESSAGEUTILS_HPP
