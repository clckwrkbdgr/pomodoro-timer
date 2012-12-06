#pragma once
#include <QtGui/QWidget>
#include "ui_mainwindow.h"
class QLineEdit;
class Sounds;
class Pomodoro;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow)
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private slots:
	void changeState(int event);
private:
	// TODO Changing setting in runtime.
	Ui::MainWindow ui;
	Pomodoro * pomodoro;
	QLineEdit * editTest;
	Sounds * sounds;
};
