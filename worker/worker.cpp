#include "worker.hpp"

#include <QThread>
#include <QDebug>
#include <QHostAddress>
#include <QThreadPool>

#include <jobrequest.hpp>
#include <messageutils.hpp>

Worker::Worker(QObject *parent) :
    QObject(parent),
    m_socket(this),
    m_socketReader(&m_socket),
    m_receivedJobsCounter(0),
    m_sentJobsCounter(0)
{
    connect(&m_socket, &QTcpSocket::connected, [this] {
        qDebug() << "Connected";
        sendRegister();
    });
    connect(&m_socket, &QTcpSocket::disconnected, [this] {
        qDebug() << "Disconnected";
    });
    connect(&m_socket, &QTcpSocket::readyRead, this, &Worker::readMessages);

    m_socket.connectToHost(QHostAddress::LocalHost, 5000);
}

Worker::~Worker()
{
    sendUnregister();
}

void Worker::readMessages()
{
    auto messages = MessageUtils::readMessages(m_socketReader);
    for (auto& message : *messages) {
        switch (message->type) {
        case Message::Type::JOB_REQUEST:
            handleJobRequest(*message);
            break;
        case Message::Type::ALL_JOBS_ABORT:
            handleAllJobsAbort(*message);
            break;
        default:
            break;
        }
    }
}

void Worker::handleJobRequest(Message& message)
{
    QDataStream in(&message.data, QIODevice::ReadOnly);
    // QUESTION: do we send a list of JobRequest's?
    QList<JobRequest> requests;
    in >> requests;
    m_receivedJobsCounter += requests.size();
    for (JobRequest const& jobRequest : requests) {
        QThreadPool::globalInstance()->start(createJob(jobRequest));
    }
}

void Worker::handleAllJobsAbort(Message& message)
{
    emit abortAllJobs();
    QThreadPool::globalInstance()->clear();
    m_receivedJobsCounter = 0;
    m_sentJobsCounter = 0;
}

void Worker::sendRegister()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << QThread::idealThreadCount();
    MessageUtils::sendMessage(m_socket, Message::Type::WORKER_REGISTER, data);
}

void Worker::sendJobResult(JobResult jobResult)
{
    m_sentJobsCounter++;
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << jobResult;
    MessageUtils::sendMessage(m_socket, Message::Type::JOB_RESULT, data);
}

void Worker::sendUnregister()
{
    MessageUtils::sendMessage(m_socket, Message::Type::WORKER_UNREGISTER, true);
}

Job* Worker::createJob(const JobRequest& jobRequest)
{
    Job* job = new Job(jobRequest);
    connect(this, &Worker::abortAllJobs, job, &Job::abort);
    connect(job, &Job::jobCompleted, this, &Worker::sendJobResult);
    return job;
}
