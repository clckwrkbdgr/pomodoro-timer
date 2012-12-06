#pragma once
#include <QtCore/QObject>
class QTimer;

enum { NONE, STARTED, SHORT_BREAK, LONG_BREAK, BREAK_ENDED, INTERRUPTED };

class Pomodoro : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Pomodoro)
public:
	Pomodoro(QObject * parent = 0);
	virtual ~Pomodoro() {}
signals:
	void stateChanged(int event);
public slots:
	void startOrInterrupt();
private slots:
	void startPomodoro();
	void startBreak();
	void startShortBreak();
	void startLongBreak();
	void getReady();
	void interruptPomodoro();
private:
	void restartTimer(int interval, const char * slot);
	int finishedPomodoroCount;
	QTimer * pomodoroTimer;
};
