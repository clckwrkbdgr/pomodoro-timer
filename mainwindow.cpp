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
MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	pomodoro = new Pomodoro(this);
	connect(pomodoro, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));

	QHBoxLayout * hbox = new QHBoxLayout();
	editTest = new QLineEdit();
	hbox->addWidget(editTest);
	QPushButton * button;
	button = new QPushButton(tr("Start/interrupt"));
	connect(button, SIGNAL(clicked()), pomodoro, SLOT(startOrInterrupt()));
	hbox->addWidget(button);
	setLayout(hbox);

	sounds = new Sounds(this);
	sounds->loadSound("start", "beep-start.wav");
	sounds->loadSound("end", "beep-end.wav");
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

void MainWindow::changeState(int event)
{
	switch(event) {
		case STARTED: 
			editTest->setText(tr("Started"));
			break;
		case SHORT_BREAK: 
			editTest->setText(tr("Short break"));
			sounds->playSound("start");
			break;
		case LONG_BREAK: 
			editTest->setText(tr("Long break"));
			sounds->playSound("start");
			break;
		case BREAK_ENDED: 
			editTest->setText(tr("Get ready"));
			sounds->playSound("end");
			break;
		case INTERRUPTED: 
			editTest->setText(tr("Interrupted"));
			break;
		default:
			break;
	}
}

