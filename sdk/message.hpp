#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QByteArray>
#include <QDataStream>
#include <QMetaType>

struct Message {
    enum class Type {
        WORKER_REGISTER,
        WORKER_UNREGISTER,
        ALL_JOBS_ABORT,
        JOB_REQUEST,
        JOB_RESULT,
    };
    Message(Type const type = Type::WORKER_REGISTER, QByteArray const& data = QByteArray())
        : type(type),
          data(data)
    {
    }
    ~Message() {}

    Type type;
    QByteArray data;
};

inline QDataStream& operator<<(QDataStream& out, Message const& message)
{
    out << static_cast<qint8>(message.type)
        << message.data;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, Message& message)
{
    qint8 type;
    in >> type;
    message.type = static_cast<Message::Type>(type);
    in >> message.data;
    return in;
}

Q_DECLARE_METATYPE(Message)

#endif
