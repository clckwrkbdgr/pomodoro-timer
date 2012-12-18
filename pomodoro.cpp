#include <QtDebug>
#include <QtCore/QTimer>
#include "pomodoro.h"

Pomodoro::Pomodoro(const Settings & settings, QObject * parent)
	: QObject(parent), finishedPomodoroCount(0), pomodoroCount(0), cycleCount(0)
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
	emit stateChanged(STARTED);
}

void Pomodoro::startBreak()
{
	++finishedPomodoroCount;
	++pomodoroCount;

	bool isShortBreak = true;
	if(finishedPomodoroCount >= settings.getPomodoroCycleSize()) {
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
	restartTimer(settings.getShortBreakLength(), SLOT(getReady()));
}

void Pomodoro::startLongBreak()
{
	++cycleCount;
	emit stateChanged(LONG_BREAK);
	restartTimer(settings.getLongBreakLength(), SLOT(getReady()));
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
		debugSettings.setPomodoroCycleSize(3);
		debugSettings.setShortBreakLength(100);
		debugSettings.setLongBreakLength(300);
	}
	void pomodoroStarting() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		pomodoro.startOrInterrupt();
		QCOMPARE(spy.count(), 1);
		QCOMPARE(spy.at(0).first().toInt(), int(Pomodoro::STARTED));
	}
	void firstShortBreak() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::STARTED, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::SHORT_BREAK, spy);
	}
	void getsReadyAfterShortBreak() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::STARTED, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::SHORT_BREAK, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
	}
	void longBreakAfterPomodoroCycle() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		for(int i = 0; i < debugSettings.getPomodoroCycleSize(); ++i) {
			pomodoro.startOrInterrupt();
			checkTheOnlyEventIs(Pomodoro::STARTED, spy);
			waitForSignal.exec();
			bool wasLastCycle = (i == debugSettings.getPomodoroCycleSize() - 1);
			if(wasLastCycle) {
				checkTheOnlyEventIs(Pomodoro::LONG_BREAK, spy);
			} else {
				checkTheOnlyEventIs(Pomodoro::SHORT_BREAK, spy);
			}
			waitForSignal.exec();
			checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
		}
	}
	void newPomodoroCycleAfterLongBreak() {
		Pomodoro pomodoro(debugSettings);
		QSignalSpy spy(&pomodoro, SIGNAL(stateChanged(int)));
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		for(int i = 0; i < debugSettings.getPomodoroCycleSize(); ++i) {
			pomodoro.startOrInterrupt();
			checkTheOnlyEventIs(Pomodoro::STARTED, spy);
			waitForSignal.exec();
			bool wasLastCycle = (i == debugSettings.getPomodoroCycleSize() - 1);
			if(wasLastCycle) {
				checkTheOnlyEventIs(Pomodoro::LONG_BREAK, spy);
			} else {
				checkTheOnlyEventIs(Pomodoro::SHORT_BREAK, spy);
			}
			waitForSignal.exec();
			checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
		}

		pomodoro.startOrInterrupt();
		checkTheOnlyEventIs(Pomodoro::STARTED, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::SHORT_BREAK, spy);
		waitForSignal.exec();
		checkTheOnlyEventIs(Pomodoro::BREAK_ENDED, spy);
	}
	void totalPomodoroCountAndCycleCountIncrease() {
		Pomodoro pomodoro(debugSettings);
		QEventLoop waitForSignal;
		connect(&pomodoro, SIGNAL(stateChanged(int)), &waitForSignal, SLOT(quit()));

		Q_ASSERT(debugSettings.getPomodoroCycleSize() == 3);
		QCOMPARE(pomodoro.totalPomodorosTaken(), 0);
		QCOMPARE(pomodoro.completeCyclesTaken(), 0);

		pomodoro.startOrInterrupt();
		waitForSignal.exec(); // Wait for short break.
		QCOMPARE(pomodoro.totalPomodorosTaken(), 1);
		QCOMPARE(pomodoro.completeCyclesTaken(), 0);
		waitForSignal.exec(); // Wait for break end.

		pomodoro.startOrInterrupt();
		waitForSignal.exec(); // Wait for short break.
		QCOMPARE(pomodoro.totalPomodorosTaken(), 2);
		QCOMPARE(pomodoro.completeCyclesTaken(), 0);
		waitForSignal.exec(); // Wait for break end.

		pomodoro.startOrInterrupt();
		waitForSignal.exec(); // Wait for long break; one cycle is complete.
		QCOMPARE(pomodoro.totalPomodorosTaken(), 3);
		QCOMPARE(pomodoro.completeCyclesTaken(), 1);
		waitForSignal.exec(); // Wait for break end.

		pomodoro.startOrInterrupt();
		waitForSignal.exec(); // Wait for short break.
		QCOMPARE(pomodoro.totalPomodorosTaken(), 4);
		QCOMPARE(pomodoro.completeCyclesTaken(), 1);
		waitForSignal.exec(); // Wait for break end.
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
