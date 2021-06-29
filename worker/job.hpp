#ifndef JOB_HPP
#define JOB_HPP

#include <QObject>
#include <QRunnable>
#include <QAtomicInteger>

#include <jobrequest.hpp>
#include <jobresult.hpp>

class Job : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Job(JobRequest const& jobRequest, QObject *parent = nullptr);
    void run() override;

signals:
    void jobCompleted(JobResult jobResult) const;

public slots:
    void abort();

private:
    QAtomicInteger<bool> m_abort;
    JobRequest m_jobRequest;
};

#endif // JOB_HPP
