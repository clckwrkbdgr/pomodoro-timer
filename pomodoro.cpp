#include <QtDebug>
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


void Pomodoro::startOrInterrupt()
{
	if(pomodoroTimer.isActive()) {
		interruptPomodoro();
	} else {
		startPomodoro();
	}
	qDebug() << tr("Start or interrupt") << getStatus();
}

void Pomodoro::startPomodoro()
{
	pomodoroTimer.setInterval(POMODORO_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(startBreak()));
	pomodoroTimer.start();
	qDebug() << tr("Start pomodoro") << getStatus();
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
	qDebug() << tr("startBreak") << getStatus();
}

void Pomodoro::startShortBreak()
{
	emit stateChanged(SHORT_BREAK);
	// TODO Simplify timer work.
	pomodoroTimer.setInterval(SHORT_BREAK_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(getReady()));
	pomodoroTimer.start();
	qDebug() << tr("startShortBreak") << getStatus();
}

void Pomodoro::startLongBreak()
{
	emit stateChanged(LONG_BREAK);
	// @todo Move SM stuff to another class.
	pomodoroTimer.setInterval(LONG_BREAK_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(getReady()));
	pomodoroTimer.start();
	qDebug() << tr("startLongBreak") << getStatus();
}

void Pomodoro::getReady()
{
	emit stateChanged(BREAK_ENDED);
	qDebug() << tr("getReady") << getStatus();
}

void Pomodoro::interruptPomodoro()
{
	emit stateChanged(INTERRUPTED);
	pomodoroTimer.stop();
	qDebug() << tr("interruptPomodoro") << getStatus();
}

QString Pomodoro::getStatus()
{
	return tr("Pomodoro: %2 for %3, finished: %1")
		.arg(finishedPomodoroCount)
		.arg(pomodoroTimer.isActive())
		.arg(pomodoroTimer.interval())
		;
}


