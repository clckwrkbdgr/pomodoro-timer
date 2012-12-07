#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("kp580bm1");
	app.setApplicationName("pomodoro");

	MainWindow wnd;
	return app.exec();
}
