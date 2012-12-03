#pragma once

#include <QtGui/QWidget>
#include <QtCore/QTimer>

class QTextBrowser;
class QUrl;
class QLineEdit;
class Mix_Chunk;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private slots:
	void startOrInterrupt();
	void startPomodoro();
	void startBreak();
	void startShortBreak();
	void startLongBreak();
	void getReady();
	void interruptPomodoro();
private:
	QString getStatus();
	int finishedPomodoroCount;
	QLineEdit * editTest;
	QTimer pomodoroTimer;
	Mix_Chunk * soundStart;
	Mix_Chunk * soundEnd;
};
