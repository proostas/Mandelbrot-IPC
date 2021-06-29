#include "job.hpp"

Job::Job(const JobRequest& jobRequest, QObject *parent) :
    QObject(parent),
    m_abort(false),
    m_jobRequest(jobRequest)
{
}

void Job::run()
{
    JobResult jobResult(m_jobRequest.areaSize.width());
    jobResult.areaSize = m_jobRequest.areaSize;
    jobResult.pixelPositionY = m_jobRequest.pixelPosistionY;
    jobResult.moveOffset = m_jobRequest.moveOffset;
    jobResult.scaleFactor = m_jobRequest.scaleFactor;

    double imageHalfWidth = m_jobRequest.areaSize.width()/2.0;
    double imageHalfHeight = m_jobRequest.areaSize.height()/2.0;

    for (int imageX = 0; imageX < m_jobRequest.areaSize.width(); ++imageX) {
        if (m_abort.load()) {
            return;
        }

        int iteration = 0;
        double x0 = (imageX - imageHalfWidth) * m_jobRequest.scaleFactor + m_jobRequest.moveOffset.x();
        double y0 = (m_jobRequest.pixelPosistionY - imageHalfHeight) * m_jobRequest.scaleFactor - m_jobRequest.moveOffset.y();
        double x = 0.0;
        double y = 0.0;
        do {
            double nextX = (x*x) - (y*y) + x0;
            y = 2.0 * x * y + y0;
            x = nextX;
            iteration++;
        } while(iteration < m_jobRequest.iterationMax && (x*x) + (y*y) < 4.0);
        jobResult.values[imageX] = iteration;
    }
    emit jobCompleted(jobResult);
}

void Job::abort()
{
    m_abort.store(true);
}
