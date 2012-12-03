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
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
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
const int POMODORO_LENGTH = 25; // minutes.
const int POMODORO_CYCLE_SIZE = 4;
const int SHORT_BREAK_LENGTH = 5; // minutes.
const int LONG_BREAK_LENGTH = 20; // minutes.

void initSDL()
{
	if(SDL_Init(SDL_INIT_AUDIO) != 0) {
		qDebug() << MainWindow::tr("Unable to initialize SDL: %1").arg(SDL_GetError());
		return;
	}

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		qDebug() << MainWindow::tr("Unable to initialize audio: %1").arg(Mix_GetError());
	}
}

void closeSDL()
{
	Mix_CloseAudio();
	SDL_Quit();
}

Mix_Chunk * loadSound(const QString & fileName)
{
	Mix_Chunk *sound = NULL;
	sound = Mix_LoadWAV(fileName.toAscii().constData());
	if(sound == NULL) {
		qDebug() << MainWindow::tr("Unable to load WAV file: %1").arg(Mix_GetError());
	}
	return sound;
}

bool playSound(Mix_Chunk * sound)
{
	int channel;
	 
	channel = Mix_PlayChannel(-1, sound, 0);
	if(channel == -1) {
		qDebug() << MainWindow::tr("Unable to play WAV file: %1").arg(Mix_GetError());
		return false;
	}
	return true;
}

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

	initSDL();
	soundStart = loadSound("beep-start.wav");
	soundEnd = loadSound("beep-end.wav");

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

	Mix_FreeChunk(soundStart);
	Mix_FreeChunk(soundEnd);
	closeSDL();
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
	pomodoroTimer.setInterval(POMODORO_LENGTH * 60 * 1000);
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

	playSound(soundStart);
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
	pomodoroTimer.setInterval(SHORT_BREAK_LENGTH * 60 * 1000);
	pomodoroTimer.setSingleShot(true);
	pomodoroTimer.disconnect();
	connect(&pomodoroTimer, SIGNAL(timeout()), this, SLOT(getReady()));
	pomodoroTimer.start();
	qDebug() << tr("startShortBreak") << getStatus();
}


void MainWindow::startLongBreak()
{
	editTest->setText(tr("Long break"));
	pomodoroTimer.setInterval(LONG_BREAK_LENGTH * 60 * 1000);
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
	playSound(soundEnd);
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

