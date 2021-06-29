#include <QCoreApplication>

#include "jobresult.hpp"

#include "application.hpp"

int main(int argc, char* argv[])
{
    qRegisterMetaType<JobResult>();

    QCoreApplication app(argc, argv);
    Application application;
    return app.exec();
}
