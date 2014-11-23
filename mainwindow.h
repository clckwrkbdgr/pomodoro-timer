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
	void on_description_linkActivated(const QString &);

	void changeState(int event);
	void updateDescription(const Settings & settings);
	void toggleVisibility();
	void activateFromTray(QSystemTrayIcon::ActivationReason reason);
	void changeTimeLeft(int msecs_left);
private:
	Ui::MainWindow ui;
	Pomodoro * pomodoro;
	QMap<QString, QPixmap> icons;
	QSystemTrayIcon * tray;
	QString stateName;
	void saveWindowState();
	void restoreWindowState();

	QMap<int, QString> eventIcons;
	QMap<int, QString> eventExternalCommands;
	QMap<int, QString> eventNames;
};
