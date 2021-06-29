#include <QApplication>
#include <QList>

#include <jobrequest.hpp>
#include <jobresult.hpp>
#include <workerclient.hpp>

#include "dialog.hpp"

int main(int argc, char* argv[])
{
    qRegisterMetaType<QList<JobRequest>>();
    qRegisterMetaType<QList<JobResult>>();
    qRegisterMetaType<WorkerClient*>();

	QApplication app(argc, argv);

    Dialog dialog;
    dialog.show();
	return app.exec();
}
