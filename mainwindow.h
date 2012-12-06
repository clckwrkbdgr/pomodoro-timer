#pragma once
#include <QtGui/QWidget>
#include "ui_mainwindow.h"
class Sounds;
class Pomodoro;
class Settings;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow)
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private slots:
	void changeState(int event);
	void on_pomodoroLength_clicked();
	void on_pomodoroCycleSize_clicked();
	void on_shortBreakLength_clicked();
	void on_longBreakLength_clicked();
	void updateDescription(const Settings & settings);
private:
	Ui::MainWindow ui;
	Pomodoro * pomodoro;
	Sounds * sounds;
	void saveWindowState();
	void restoreWindowState();
};
