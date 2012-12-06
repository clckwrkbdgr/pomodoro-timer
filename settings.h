#pragma once
#include <QtCore/QObject>

class Settings : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Settings)
public:
	enum { SECOND = 1000, MINUTE = 60 * SECOND };

	Settings(QObject * parent = 0);
	virtual ~Settings() {}

	int getPomodoroLength() const;
	Settings * setPomodoroLength(int value);
	int getPomodoroCycleSize() const;
	Settings * setPomodoroCycleSize(int value);
	int getShortBreakLength() const;
	Settings * setShortBreakLength(int value);
	int getLongBreakLength() const;
	Settings * setLongBreakLength(int value);
private:
	int pomodoroLength;
	int pomodoroCycleSize;
	int shortBreakLength;
	int longBreakLength;
};

