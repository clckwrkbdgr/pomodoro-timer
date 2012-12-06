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
#include "settings.h"
#include "pomodoro.h"
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

void setDebugSettings(Settings & settings)
{
	settings.setPomodoroLength(1 * Settings::SECOND);
	settings.setPomodoroCycleSize(3);
	settings.setShortBreakLength(1 * Settings::SECOND);
	settings.setLongBreakLength(2 * Settings::SECOND);
}

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	restoreWindowState();

	Settings settings;
	settings.load();
	//setDebugSettings(pomodoroSettings);
	updateDescription(settings);
	pomodoro = new Pomodoro(settings, this);
	connect(pomodoro, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));
	connect(ui.startOrInterrupt, SIGNAL(clicked()), pomodoro, SLOT(startOrInterrupt()));

	sounds = new Sounds(this);
	sounds->loadSound("start", "beep-start.wav");
	sounds->loadSound("end", "beep-end.wav");
}

MainWindow::~MainWindow()
{
	pomodoro->getSettings().save();
	saveWindowState();
}

void MainWindow::updateDescription(const Settings & settings)
{
	ui.description->setText(
			tr("Pomodoro length: %1 minutes.\nCycle size: %2.\nShort break: %3 minutes.\nLong break: %4 minutes.")
			.arg(settings.getPomodoroLength() / Settings::MINUTE)
			.arg(settings.getPomodoroCycleSize())
			.arg(settings.getShortBreakLength() / Settings::MINUTE)
			.arg(settings.getLongBreakLength() / Settings::MINUTE)
			);
}

void MainWindow::on_pomodoroLength_clicked()
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New pomodoro length in minutes:"), settings.getPomodoroLength() / Settings::MINUTE, 1, 60, 1, &ok);
	if(ok) {
		settings.setPomodoroLength(newValue * Settings::MINUTE);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
}

void MainWindow::on_pomodoroCycleSize_clicked()
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New pomodoro cycle size:"), settings.getPomodoroCycleSize(), 1, 10, 1, &ok);
	if(ok) {
		settings.setPomodoroCycleSize(newValue);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
}

void MainWindow::on_shortBreakLength_clicked()
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New short break length in minutes:"), settings.getShortBreakLength() / Settings::MINUTE, 1, 60, 1, &ok);
	if(ok) {
		settings.setShortBreakLength(newValue * Settings::MINUTE);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
}

void MainWindow::on_longBreakLength_clicked()
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New long break length in minutes:"), settings.getLongBreakLength() / Settings::MINUTE, 1, 60, 1, &ok);
	if(ok) {
		settings.setLongBreakLength(newValue * Settings::MINUTE);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
}

void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.setValue("mainwindow/maximized", windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}
}

void MainWindow::restoreWindowState()
{
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);
}

void MainWindow::changeState(int event)
{
	switch(event) {
		case Pomodoro::STARTED: 
			ui.state->setText(tr("Started"));
			break;
		case Pomodoro::SHORT_BREAK: 
			ui.state->setText(tr("Short break"));
			sounds->playSound("start");
			break;
		case Pomodoro::LONG_BREAK: 
			ui.state->setText(tr("Long break"));
			sounds->playSound("start");
			break;
		case Pomodoro::BREAK_ENDED: 
			ui.state->setText(tr("Get ready"));
			sounds->playSound("end");
			break;
		case Pomodoro::INTERRUPTED: 
			ui.state->setText(tr("Interrupted"));
			break;
		default:
			break;
	}
}

