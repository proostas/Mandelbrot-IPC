#include "mandelbrotcalculator.hpp"

#include <QDebug>
#include <QThread>

const int JOB_RESULT_THRESHOLD = 10;

MandelbrotCalculator::MandelbrotCalculator(QObject *parent) :
    QTcpServer(parent),
    m_moveOffset(),
    m_scaleFactor(),
    m_areaSize(),
    m_iterationMax(),
    m_receivedJobResults(0),
    m_workerClients(),
    m_jobRequests(),
    m_timer()
{
    listen(QHostAddress::Any, 5000);
}

MandelbrotCalculator::~MandelbrotCalculator()
{
    while (!m_workerClients.isEmpty()) {
        removeWorkerClient(m_workerClients.firstKey());
    }
}

void MandelbrotCalculator::generatePicture(QSize areaSize, QPointF moveOffset, double scaleFactor, int iterationMax)
{
    if (areaSize.isEmpty())
        return;

    m_timer.start();
    clearJobs();

    m_areaSize = areaSize;
    m_moveOffset = moveOffset;
    m_scaleFactor = scaleFactor;
    m_iterationMax = iterationMax;

    // note the reverse order
    for (int pixelPositionY = m_areaSize.height()-1; pixelPositionY >= 0; pixelPositionY--) {
        m_jobRequests.push_back(createJobRequest(pixelPositionY));
    }

    for (WorkerClient* client : m_workerClients.keys()) {
        sendJobRequests(*client, client->cpuCoreCount() * 2);
    }
}

void MandelbrotCalculator::process(WorkerClient* workerClient, JobResult jobResult)
{
    if (jobResult.areaSize != m_areaSize || jobResult.moveOffset != m_moveOffset || jobResult.scaleFactor != m_scaleFactor) {
        return;
    }

    m_receivedJobResults++;
    m_jobResults.append(jobResult);

    if (m_jobResults.size() >= JOB_RESULT_THRESHOLD || m_receivedJobResults == m_areaSize.height()) {
        emit pictureLinesGenerated(m_jobResults);
        m_jobResults.clear();
    }

    if (m_receivedJobResults < m_areaSize.height()) {
        sendJobRequests(*workerClient);
    } else {
        qDebug() << "Generated in" << m_timer.elapsed() << "ms";
    }
}

void MandelbrotCalculator::removeWorkerClient(WorkerClient* workerClient)
{
    qDebug() << "Removing workerClient";
    QThread* thread = m_workerClients.take(workerClient);
    thread->quit();
    thread->wait(1000);
    delete thread;
    delete workerClient;
}

void MandelbrotCalculator::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "Connected workerClient";
    QThread* thread = new QThread(this);
    WorkerClient* client = new WorkerClient(socketDescriptor);
    int workerClientsCount = m_workerClients.keys().size();
    m_workerClients.insert(client, thread);
    client->moveToThread(thread);

    connect(this, &MandelbrotCalculator::abortAllJobs, client, &WorkerClient::abortJob);

    connect(client, &WorkerClient::unregistered, this, &MandelbrotCalculator::removeWorkerClient);
    connect(client, &WorkerClient::jobCompleted, this, &MandelbrotCalculator::process);

    connect(thread, &QThread::started, client, &WorkerClient::start);
    thread->start();

    // trigger picture generation upon the first client connection
    if (workerClientsCount == 0 && m_workerClients.size() == 1) {
        generatePicture(m_areaSize, m_moveOffset, m_scaleFactor, m_iterationMax);
    }
}

std::unique_ptr<JobRequest> MandelbrotCalculator::createJobRequest(int pixelPositionY)
{
    auto jobRequest = std::make_unique<JobRequest>();
    jobRequest->pixelPosistionY = pixelPositionY;
    jobRequest->moveOffset = m_moveOffset;
    jobRequest->scaleFactor = m_scaleFactor;
    jobRequest->areaSize = m_areaSize;
    jobRequest->iterationMax = m_iterationMax;
    return jobRequest;
}

void MandelbrotCalculator::sendJobRequests(WorkerClient& client, int jobRequestCount)
{
    QList<JobRequest> listJobRequest;
    for (int i = 0; i < jobRequestCount; ++i) {
        if (m_jobRequests.empty()) {
            break;
        }

        auto jobRequest = std::move(m_jobRequests.back());
        m_jobRequests.pop_back();
        listJobRequest.append(*jobRequest);
    }

    if (!listJobRequest.empty()) {
        emit client.sendJobRequests(listJobRequest);
    }
}

void MandelbrotCalculator::clearJobs()
{
    m_receivedJobResults = 0;
    m_jobRequests.clear();
    emit abortAllJobs();
}
