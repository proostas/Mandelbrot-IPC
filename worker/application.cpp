#include "application.hpp"

Application::Application(QObject *parent) :
    QObject(parent),
    m_worker(),
    m_workerThread(this)
{
    m_worker.moveToThread(&m_workerThread);
    m_workerThread.start();
}

Application::~Application()
{
    m_workerThread.quit();
    m_workerThread.wait(100);
}
