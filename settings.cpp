#include <QtCore/QSettings>
#include "settings.h"

Settings::Settings()
	: pomodoroLength(25 * MINUTE),
	pomodoroCycleSize(4),
	shortBreakLength(5 * MINUTE),
	longBreakLength(15 * MINUTE)
{
}

void Settings::load()
{
	QSettings settings;
	pomodoroLength = settings.value("pomodoro/pomodorolength", pomodoroLength).toInt();
	pomodoroCycleSize = settings.value("pomodoro/pomodorocyclesize", pomodoroCycleSize).toInt();
	shortBreakLength = settings.value("pomodoro/shortbreaklength", shortBreakLength).toInt();
	longBreakLength = settings.value("pomodoro/longbreaklength", longBreakLength).toInt();
}

void Settings::save()
{
	QSettings settings;
	settings.setValue("pomodoro/pomodorolength", pomodoroLength);
	settings.setValue("pomodoro/pomodorocyclesize", pomodoroCycleSize);
	settings.setValue("pomodoro/shortbreaklength", shortBreakLength);
	settings.setValue("pomodoro/longbreaklength", longBreakLength);
}

int Settings::getPomodoroLength() const
{
	return pomodoroLength;
}

void Settings::setPomodoroLength(int value)
{
	pomodoroLength = value;
}

int Settings::getPomodoroCycleSize() const
{
	return pomodoroCycleSize;
}

void Settings::setPomodoroCycleSize(int value)
{
	pomodoroCycleSize = value;
}

int Settings::getShortBreakLength() const
{
	return shortBreakLength;
}

void Settings::setShortBreakLength(int value)
{
	shortBreakLength = value;
}

int Settings::getLongBreakLength() const
{
	return longBreakLength;
}

void Settings::setLongBreakLength(int value)
{
	longBreakLength = value;
}

