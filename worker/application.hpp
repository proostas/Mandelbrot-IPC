#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QObject>
#include <QThread>

#include "worker.hpp"

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = nullptr);
    ~Application();

private:
    Worker m_worker;
    QThread m_workerThread;
};

#endif // APPLICATION_HPP
