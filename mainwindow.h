#pragma once
#include <QtGui/QWidget>
#include <QtGui/QSystemTrayIcon>
#include "ui_mainwindow.h"
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
	void on_breakLength_clicked();
	void on_startSound_clicked();
	void on_endSound_clicked();
	void on_autorestart_clicked();

	void changeState(int event);
	void updateDescription(const Settings & settings);
	void toggleVisibility();
	void activateFromTray(QSystemTrayIcon::ActivationReason reason);
private:
	Ui::MainWindow ui;
	Pomodoro * pomodoro;
	QMap<QString, QPixmap> icons;
	QSystemTrayIcon * tray;
	void saveWindowState();
	void restoreWindowState();

	QMap<int, QString> eventIcons;
	QMap<int, QString> eventExternalCommands;
	QMap<int, QString> eventNames;
};
