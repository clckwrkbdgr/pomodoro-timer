#pragma once
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include "settings.h"

class Pomodoro : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Pomodoro)
public:
	enum { NONE, ON_RUN, BREAK, BREAK_ENDED, INTERRUPTED };

	Pomodoro(const Settings & settings, QObject * parent = 0);
	virtual ~Pomodoro() {}

	const Settings & getSettings() const;
	void setSettings(const Settings & newSettings);

	int totalPomodorosTaken() const { return pomodoroCount; }
public slots:
	void startOrInterrupt();
signals:
	void stateChanged(int event);
	void timeLeftChanged(int msecs_left);

private slots:
	void startPomodoro();
	void startBreak();
	void getReady();
	void interruptPomodoro();
	void tick();
private:
	void restartTimer(int interval, const char * slot);
	Settings settings;
	int finishedPomodoroCount;
	int pomodoroCount;
	QTimer * pomodoroTimer;
	QTimer metronome;
	QTime time_left;
};

