#ifndef WORKER_HPP
#define WORKER_HPP

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include <message.hpp>
#include <jobresult.hpp>
#include "job.hpp"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void abortAllJobs() const;

private slots:
    void readMessages();

private:
    void handleJobRequest(Message& message);
    void handleAllJobsAbort(Message& message);
    void sendRegister();
    void sendJobResult(JobResult jobResult);
    void sendUnregister();
    Job* createJob(JobRequest const& jobRequest);

private:
    QTcpSocket m_socket;
    QDataStream m_socketReader;
    int m_receivedJobsCounter;
    int m_sentJobsCounter;
};

#endif // WORKER_HPP
