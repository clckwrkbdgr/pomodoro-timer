#include <QtDebug>
#include <QtCore/QTimer>
#include "pomodoro.h"

Pomodoro::Pomodoro(const Settings & settings, QObject * parent)
	: QObject(parent), finishedPomodoroCount(0), pomodoroCount(0)
{
	this->settings = settings;
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
	restartTimer(settings.getPomodoroLength(), SLOT(startBreak()));
	emit stateChanged(ON_RUN);
}

void Pomodoro::startBreak()
{
	++finishedPomodoroCount;
	++pomodoroCount;
	emit stateChanged(BREAK);
	restartTimer(settings.getBreakLength(), SLOT(getReady()));
}

void Pomodoro::getReady()
{
	emit stateChanged(BREAK_ENDED);
	if(settings.getAutorestart()) {
		startPomodoro();
	}
}

void Pomodoro::interruptPomodoro()
{
	emit stateChanged(INTERRUPTED);
	pomodoroTimer->stop();
}

const Settings & Pomodoro::getSettings() const
{
	return settings;
}

void Pomodoro::setSettings(const Settings & newSettings)
{
	settings = newSettings;
	interruptPomodoro();
}

//! Use `qmake "CONFIG+=POMODORO_TEST" to use debug mode.
#ifdef POMODORO_TEST
#include <QtTest/QtTest>
class TestPomodoro : public QObject {
	Q_OBJECT
private slots:
	void initTestCase() {
		debugSettings.setPomodoroLength(200);
		debugSettings.setBreakLength(100);
	}
	void pomodoroStarting() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		pomodoro.startOrInterrupt();
		QCOMPARE(spy.count(), 1);
		QCOMPARE(spy.at(0).first().toInt(), int(Pomodoro::ON_RUN));
	}
	void firstBreak() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::ON_RUN, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK, spy);
	}
	void getsReadyAfterBreak() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::ON_RUN, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
	}
	void shouldRestartAfterBreakIfAutorestartIsOn() {
		Settings newSettings = debugSettings;
		newSettings.setAutorestart(true);
		Pomodoro pomodoro(newSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::ON_RUN, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK, spy);
		waitForSignal.exec();
		QCOMPARE(spy.count(), 2);
		QCOMPARE(spy.takeFirst().first().toInt(), int(Pomodoro::BREAK_ENDED));
		QCOMPARE(spy.takeFirst().first().toInt(), int(Pomodoro::ON_RUN));
	}
	void newBreakAfterNewPomodoro() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::ON_RUN, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::ON_RUN, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
	}
	void totalPomodoroCountAndCycleCountIncrease() {
		Pomodoro pomodoro(debugSettings);
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		QCOMPARE(pomodoro.totalPomodorosTaken(), 0);

		for(int i = 0; i < 4; ++i) {
			pomodoro.startOrInterrupt();
			waitForSignal.exec(); // Wait for break.
			QCOMPARE(pomodoro.totalPomodorosTaken(), i + 1);
			waitForSignal.exec(); // Wait for break end.
		}
	}
private:
	Settings debugSettings;
	void checkTheOnlyEventIs(int event, QSignalSpy & spy) {
		QCOMPARE(spy.count(), 1);
		QCOMPARE(spy.takeFirst().first().toInt(), event);
	}
};

QTEST_MAIN(TestPomodoro)
#include "pomodoro.moc"
#endif//POMODORO_TEST
