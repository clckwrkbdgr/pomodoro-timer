#include "settings.h"

Settings::Settings(QObject * parent)
	: QObject(parent),
	pomodoroLength(1 * SECOND),
	pomodoroCycleSize(4),
	shortBreakLength(1 * SECOND),
	longBreakLength(3 * SECOND)
{
}

int Settings::getPomodoroLength() const
{
	return pomodoroLength;
}

Settings * Settings::setPomodoroLength(int value)
{
	pomodoroLength = value;
	return this;
}

int Settings::getPomodoroCycleSize() const
{
	return pomodoroCycleSize;
}

Settings * Settings::setPomodoroCycleSize(int value)
{
	pomodoroCycleSize = value;
	return this;
}

int Settings::getShortBreakLength() const
{
	return shortBreakLength;
}

Settings * Settings::setShortBreakLength(int value)
{
	shortBreakLength = value;
	return this;
}

int Settings::getLongBreakLength() const
{
	return longBreakLength;
}

Settings * Settings::setLongBreakLength(int value)
{
	longBreakLength = value;
	return this;
}

