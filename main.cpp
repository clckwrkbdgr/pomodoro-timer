#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("kp580bm1");
	app.setApplicationName("pomodoro");

	QStringList args = app.arguments();
	bool single_shot = args.contains("--single-shot");
	MainWindow wnd(single_shot);
	return app.exec();
}
