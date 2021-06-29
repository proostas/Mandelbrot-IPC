#ifndef WORKERCLIENT_HPP
#define WORKERCLIENT_HPP

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QMetaType>

#include <jobrequest.hpp>
#include <jobresult.hpp>
#include <message.hpp>

class WorkerClient : public QObject
{
    Q_OBJECT
public:
    explicit WorkerClient(int socketDescriptor);
    int cpuCoreCount() const;

signals:
    void unregistered(WorkerClient* workerClient) const;
    void jobCompleted(WorkerClient* workerClient, JobResult jobResult) const;
    void sendJobRequests(QList<JobRequest> requests) const;

public slots:
    void start();
    void abortJob();

private slots:
    void readMessages();
    void doSendJobRequests(QList<JobRequest> requests);

private:
    void handleWorkerRegiestered(Message& message);
    void handleWorkerUnregistered(Message& message);
    void handleJobResult(Message& message);


private:
    int m_socketDescriptor;
    int m_cpuCoreCount;
    QTcpSocket m_socket;
    QDataStream m_socketReader;
};

Q_DECLARE_METATYPE(WorkerClient*)

#endif // WORKERCLIENT_HPP
