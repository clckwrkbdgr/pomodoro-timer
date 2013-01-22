#pragma once
#include <QtCore/QObject>
#include "settings.h"
class QTimer;

// TODO Remove short/long break distiction.
// TODO Remove cycle counting.

class Pomodoro : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Pomodoro)
public:
	enum { NONE, STARTED, SHORT_BREAK, BREAK_ENDED, INTERRUPTED };

	Pomodoro(const Settings & settings, QObject * parent = 0);
	virtual ~Pomodoro() {}

	const Settings & getSettings() const;
	void setSettings(const Settings & newSettings);

	int totalPomodorosTaken() const { return pomodoroCount; }
public slots:
	void startOrInterrupt();
signals:
	void stateChanged(int event);

private slots:
	void startPomodoro();
	void startBreak();
	void startShortBreak();
	void getReady();
	void interruptPomodoro();
private:
	void restartTimer(int interval, const char * slot);
	Settings settings;
	int finishedPomodoroCount;
	int pomodoroCount;
	QTimer * pomodoroTimer;
};

