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
	startCommand = settings.value("commands/start", startCommand).toString();
	endCommand = settings.value("commands/end", endCommand).toString();
}

void Settings::save() const
{
	QSettings settings;
	settings.setValue("pomodoro/pomodorolength", pomodoroLength);
	settings.setValue("pomodoro/breaklength", breakLength);
	settings.setValue("commands/start", startCommand);
	settings.setValue("commands/end", endCommand);
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

const QString & Settings::getStartCommand() const
{
	return startCommand;
}

void Settings::setStartCommand(const QString & value)
{
	startCommand = value;
}

const QString & Settings::getEndCommand() const
{
	return endCommand;
}

void Settings::setEndCommand(const QString & value)
{
	endCommand = value;
}

