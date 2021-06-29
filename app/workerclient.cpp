#include "workerclient.hpp"
#include <messageutils.hpp>

WorkerClient::WorkerClient(int socketDescriptor) :
    QObject(),
    m_socketDescriptor(socketDescriptor),
    m_socket(this),
    m_socketReader(&m_socket)
{
    connect(this, &WorkerClient::sendJobRequests, this, &WorkerClient::doSendJobRequests);
}

int WorkerClient::cpuCoreCount() const
{
    return m_cpuCoreCount;
}

void WorkerClient::start()
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &WorkerClient::readMessages);
    m_socket.setSocketDescriptor(m_socketDescriptor);
}

void WorkerClient::abortJob()
{
    MessageUtils::sendMessage(m_socket, Message::Type::ALL_JOBS_ABORT, true);
}

void WorkerClient::readMessages()
{
    auto messages = MessageUtils::readMessages(m_socketReader);
    for (auto& message : *messages) {
        switch (message->type) {
        case Message::Type::WORKER_REGISTER:
            handleWorkerRegiestered(*message);
            break;
        case Message::Type::WORKER_UNREGISTER:
            handleWorkerUnregistered(*message);
            break;
        case Message::Type::JOB_RESULT:
            handleJobResult(*message);
            break;
        default:
            break;
        }
    }
}

void WorkerClient::doSendJobRequests(QList<JobRequest> requests)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << requests;

    MessageUtils::sendMessage(m_socket, Message::Type::JOB_REQUEST, data);
}

void WorkerClient::handleWorkerRegiestered(Message& message)
{
    QDataStream in(&message.data, QIODevice::ReadOnly);
    in >> m_cpuCoreCount;
}

void WorkerClient::handleWorkerUnregistered(Message& message)
{
    emit unregistered(this);
}

void WorkerClient::handleJobResult(Message& message)
{
    QDataStream in(&message.data, QIODevice::ReadOnly);
    JobResult jobResult;
    in >> jobResult;
    emit jobCompleted(this, jobResult);
}
