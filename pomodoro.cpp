#include <QtDebug>
#include <QtCore/QTimer>
#include "pomodoro.h"

const int SECOND = 1000;
const int MINUTE = 60 * SECOND;

const int POMODORO_LENGTH = 1 * SECOND;
const int POMODORO_CYCLE_SIZE = 2;
const int SHORT_BREAK_LENGTH = 1 * SECOND;
const int LONG_BREAK_LENGTH = 3 * SECOND;
/*
const int POMODORO_LENGTH = 25 * MINUTES;
const int POMODORO_CYCLE_SIZE = 4;
const int SHORT_BREAK_LENGTH = 5 * MINUTES;
const int LONG_BREAK_LENGTH = 20 * MINUTES;
*/

Pomodoro::Pomodoro(QObject * parent)
	: QObject(parent), finishedPomodoroCount(0)
{
	pomodoroTimer = new QTimer(this);
}

void Pomodoro::restartTimer(int interval, const char * slot)
{
	pomodoroTimer->setInterval(interval);
	pomodoroTimer->setSingleShot(true);
	pomodoroTimer->disconnect();
	connect(pomodoroTimer, SIGNAL(timeout()), this, slot);
	pomodoroTimer->start();
}

void Pomodoro::startOrInterrupt()
{
	if(pomodoroTimer->isActive()) {
		interruptPomodoro();
	} else {
		startPomodoro();
	}
}

void Pomodoro::startPomodoro()
{
	restartTimer(POMODORO_LENGTH, SLOT(startBreak()));
	emit stateChanged(STARTED);
}

void Pomodoro::startBreak()
{
	++finishedPomodoroCount;
	bool isShortBreak = true;
	if(finishedPomodoroCount >= POMODORO_CYCLE_SIZE) {
		isShortBreak = false;
		finishedPomodoroCount = 0;
	}

	if(isShortBreak) {
		startShortBreak();
	} else {
		startLongBreak();
	}
}

void Pomodoro::startShortBreak()
{
	emit stateChanged(SHORT_BREAK);
	restartTimer(SHORT_BREAK_LENGTH, SLOT(getReady()));
}

void Pomodoro::startLongBreak()
{
	emit stateChanged(LONG_BREAK);
	restartTimer(LONG_BREAK_LENGTH, SLOT(getReady()));
}

void Pomodoro::getReady()
{
	emit stateChanged(BREAK_ENDED);
}

void Pomodoro::interruptPomodoro()
{
	emit stateChanged(INTERRUPTED);
	pomodoroTimer->stop();
}


