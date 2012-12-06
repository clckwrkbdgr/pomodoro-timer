#include <QtDebug>
#include <QtCore/QSettings>
#include <QtGui/QPushButton>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtGui/QInputDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QFormLayout>
#include <QtCore/QFileInfo>
#include <QtGui/QSound>
#include "sounds.h"
#include "mainwindow.h"

const QString TEXT = MainWindow::tr(
		"Pomodoro lasts for <a href='#1'>%1</a> minutes followed up by a short break for <a href='#2'>%2</a> minutes.<br>"
		"Every <a href='#3'>%3</a> pomodoros a long break for <a href='#4'>%4</a> minutes should be taken.<br>"
		"<br>"
		"Pomodoro runs by a click on the tray: its changed to <a href='#5'>%5</a>.<br>"
		"Every quarter of the pomodoro length a quarter of then the tray icon turns <a href='#6'>%6</a>.<br>"
		"When pomodoro is over, it should be all <a href='#7'>%7</a> and the <a href='#8'>%8</a> will bell.<br>"
		"When break is over, the tray icon starts flashing <a href='#9'>%9</a>.<br>"
		"When you're ready to start, click the tray icon again: it will run another pomodoro.<br>"
		"If you want to interrupt current pomodoro, click on the tray icon.<br>"
		"It will run a short break after which interrupted pomodoro will be started anew.<br>"
		);
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

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	QHBoxLayout * hbox = new QHBoxLayout();
	editTest = new QLineEdit();
	hbox->addWidget(editTest);
	QPushButton * button;
	button = new QPushButton(tr("Start/interrupt"));
	connect(button, SIGNAL(clicked()), this, SLOT(startOrInterrupt()));
	hbox->addWidget(button);
	setLayout(hbox);

	sounds = new Sounds(this);
	sounds->loadSound("start", "beep-start.wav");
	sounds->loadSound("end", "beep-end.wav");

	finishedPomodoroCount = 0;
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("mainwindow/maximized", windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}
}

void MainWindow::startOrInterrupt()
{
	if(pomodoroTimer.isActive()) {
		interruptPomodoro();
	} else {
		startPomodoro();
	}
	qDebug() << tr("Start or interrupt") << getStatus();
}

void MainWindow::startPomodoro()
{
	editTest->setText(tr("Started"));
	pomodoroTimer.setInterval(POMODORO_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(startBreak()));
	pomodoroTimer.start();
	qDebug() << tr("Start pomodoro") << getStatus();
}

void MainWindow::startBreak()
{
	++finishedPomodoroCount;
	bool isShortBreak = true;
	if(finishedPomodoroCount >= POMODORO_CYCLE_SIZE) {
		isShortBreak = false;
		finishedPomodoroCount = 0;
	}

	sounds->playSound("start");
	if(isShortBreak) {
		startShortBreak();
	} else {
		startLongBreak();
	}
	qDebug() << tr("startBreak") << getStatus();
}

void MainWindow::startShortBreak()
{
	editTest->setText(tr("Short break"));
	// TODO Simplify timer work.
	pomodoroTimer.setInterval(SHORT_BREAK_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(getReady()));
	pomodoroTimer.start();
	qDebug() << tr("startShortBreak") << getStatus();
}

void MainWindow::startLongBreak()
{
	editTest->setText(tr("Long break"));
	// @todo Move SM stuff to another class.
	pomodoroTimer.setInterval(LONG_BREAK_LENGTH);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(getReady()));
	pomodoroTimer.start();
	qDebug() << tr("startLongBreak") << getStatus();
}

void MainWindow::getReady()
{
	editTest->setText(tr("Get ready"));
	qDebug() << tr("getReady") << getStatus();
	sounds->playSound("end");
}

void MainWindow::interruptPomodoro()
{
	editTest->setText(tr("Interrupted"));
	pomodoroTimer.stop();
	qDebug() << tr("interruptPomodoro") << getStatus();
}

QString MainWindow::getStatus()
{
	return tr("Pomodoro: %2 for %3, finished: %1")
		.arg(finishedPomodoroCount)
		.arg(pomodoroTimer.isActive())
		.arg(pomodoroTimer.interval())
		;
}

