#pragma once
#include <QtCore/QObject>
#include "settings.h"
class QTimer;


class Pomodoro : public QObject {
	Q_OBJECT
	Q_DISABLE_COPY(Pomodoro)
public:
	enum { NONE, STARTED, SHORT_BREAK, LONG_BREAK, BREAK_ENDED, INTERRUPTED };

	Pomodoro(const Settings & settings, QObject * parent = 0);
	virtual ~Pomodoro() {}

	const Settings & getSettings() const;
	void setSettings(const Settings & newSettings);
public slots:
	void startOrInterrupt();
signals:
	void stateChanged(int event);

private slots:
	void startPomodoro();
	void startBreak();
	void startShortBreak();
	void startLongBreak();
	void getReady();
	void interruptPomodoro();
private:
	void restartTimer(int interval, const char * slot);
	Settings settings;
	int finishedPomodoroCount;
	QTimer * pomodoroTimer;
};

