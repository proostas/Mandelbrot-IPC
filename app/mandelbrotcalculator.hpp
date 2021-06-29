#ifndef MANDELBROTCALCULATOR_HPP
#define MANDELBROTCALCULATOR_HPP

#include <memory>
#include <vector>

#include <QTcpServer>
#include <QList>
#include <QThread>
#include <QMap>
#include <QElapsedTimer>

#include "workerclient.hpp"
#include <jobrequest.hpp>
#include <jobresult.hpp>

class MandelbrotCalculator : public QTcpServer
{
    Q_OBJECT
public:
    explicit MandelbrotCalculator(QObject *parent = nullptr);
    ~MandelbrotCalculator();

signals:
    void pictureLinesGenerated(QList<JobResult> jobResults) const;
    void abortAllJobs() const;

public slots:
    void generatePicture(QSize areaSize, QPointF moveOffset, double scaleFactor, int iterationMax);

private slots:
    void process(WorkerClient* workerClient, JobResult jobResult);
    void removeWorkerClient(WorkerClient* workerClient);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    std::unique_ptr<JobRequest> createJobRequest(int pixelPositionY);
    void sendJobRequests(WorkerClient& client, int jobRequestCount = 1);
    void clearJobs();

private:
    QPointF m_moveOffset;
    double m_scaleFactor;
    QSize m_areaSize;
    int m_iterationMax;
    int m_receivedJobResults;
    QList<JobResult> m_jobResults;
    QMap<WorkerClient*, QThread*> m_workerClients;
    std::vector<std::unique_ptr<JobRequest>> m_jobRequests;
    QElapsedTimer m_timer;
};

#endif // MANDELBROTCALCULATOR_HPP
