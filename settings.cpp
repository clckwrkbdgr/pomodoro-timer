#include <QtCore/QSettings>
#include "settings.h"

Settings::Settings()
	: pomodoroLength(25 * MINUTE),
	breakLength(5 * MINUTE)
{
}

void Settings::load()
{
	QSettings settings;
	pomodoroLength = settings.value("pomodoro/pomodorolength", pomodoroLength).toInt();
	breakLength = settings.value("pomodoro/breaklength", breakLength).toInt();
	startSound = settings.value("sounds/start", startSound).toString();
	endSound = settings.value("sounds/end", endSound).toString();
}

void Settings::save() const
{
	QSettings settings;
	settings.setValue("pomodoro/pomodorolength", pomodoroLength);
	settings.setValue("pomodoro/breaklength", breakLength);
	settings.setValue("sounds/start", startSound);
	settings.setValue("sounds/end", endSound);
}

int Settings::getPomodoroLength() const
{
	return pomodoroLength;
}

void Settings::setPomodoroLength(int value)
{
	pomodoroLength = value;
}

int Settings::getBreakLength() const
{
	return breakLength;
}

void Settings::setBreakLength(int value)
{
	breakLength = value;
}

const QString & Settings::getStartSound() const
{
	return startSound;
}

void Settings::setStartSound(const QString & value)
{
	startSound = value;
}

const QString & Settings::getEndSound() const
{
	return endSound;
}

void Settings::setEndSound(const QString & value)
{
	endSound = value;
}

