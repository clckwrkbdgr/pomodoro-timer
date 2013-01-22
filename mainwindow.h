#pragma once
#include <QtGui/QWidget>
#include <QtGui/QSystemTrayIcon>
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
	void on_pomodoroLength_clicked();
	void on_shortBreakLength_clicked();
	void on_startSound_clicked();
	void on_endSound_clicked();

	void changeState(int event);
	void updateDescription(const Settings & settings);
	void toggleVisibility();
	void activateFromTray(QSystemTrayIcon::ActivationReason reason);
private:
	Ui::MainWindow ui;
	Pomodoro * pomodoro;
	Sounds * sounds;
	QMap<QString, QPixmap> icons;
	QSystemTrayIcon * tray;
	void saveWindowState();
	void restoreWindowState();

	QMap<int, QString> eventIcons;
	QMap<int, QString> eventSounds;
	QMap<int, QString> eventNames;
};
