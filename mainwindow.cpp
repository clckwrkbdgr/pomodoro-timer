#include <iostream>
#include <QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtCore/QUrl>
#include <QtGui/QMenu>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtGui/QInputDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QFormLayout>
#include <QtCore/QFileInfo>
#include <QtGui/QSound>
#include <chthon2/pixmap.h>
#include <chthon2/util.h>
#include "settings.h"
#include "pomodoro.h"
#include "mainwindow.h"

namespace Icons {
const
#include "res/icons/break.xpm"
const
#include "res/icons/interrupted.xpm"
const
#include "res/icons/ready.xpm"
const
#include "res/icons/start.xpm"
}

//! Use `qmake "CONFIG+=debug" to use debug mode.
#ifdef QT_DEBUG
const bool DEBUG = true;
#else//not DEBUG
const bool DEBUG = false;
#endif//DEBUG

void makeDebugSettings(Settings & settings)
{
	settings.setPomodoroLength(1 * Settings::SECOND);
	settings.setBreakLength(1 * Settings::SECOND);
}

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

QImage load_xpm(const char * xpm [], int size)
{
	std::vector<std::string> xpm_lines(xpm, xpm + size);
	QImage result;
	try {
		Chthon::Pixmap pixmap;
		pixmap.load(xpm_lines);
		result = QImage(pixmap.pixels.width(), pixmap.pixels.height(), QImage::Format_ARGB32);
		for(unsigned x = 0; x < pixmap.pixels.width(); ++x) {
			for(unsigned y = 0; y < pixmap.pixels.height(); ++y) {
				Chthon::Color c = pixmap.palette[(pixmap.pixels.cell(x, y))];
				result.setPixel(x, y, c);
			}
		}
	} catch(const Chthon::Pixmap::Exception & e) {
		std::cerr << e.what << std::endl;
	}
	return result;
}

MainWindow::MainWindow(bool use_single_shot, QWidget * parent)
	: QWidget(parent), single_shot(use_single_shot), single_shot_timer(nullptr),
	pomodoro(nullptr)
{
	eventIcons[Pomodoro::ON_RUN    ] = "start";
	eventIcons[Pomodoro::BREAK] = "break";
	eventIcons[Pomodoro::BREAK_ENDED] = "ready";
	eventIcons[Pomodoro::INTERRUPTED] = "interrupted";

	eventNames[Pomodoro::ON_RUN    ] = tr("On run");
	eventNames[Pomodoro::BREAK] = tr("Break");
	eventNames[Pomodoro::BREAK_ENDED] = tr("Get ready");
	eventNames[Pomodoro::INTERRUPTED] = tr("Interrupted");

	ui.setupUi(this);
	restoreWindowState();

	icons["start"] = QPixmap::fromImage(load_xpm(Icons::start_xpm, Chthon::size_of_array(Icons::start_xpm)));
	icons["break"] = QPixmap::fromImage(load_xpm(Icons::break_xpm, Chthon::size_of_array(Icons::break_xpm)));
	icons["ready"] = QPixmap::fromImage(load_xpm(Icons::ready_xpm, Chthon::size_of_array(Icons::ready_xpm)));
	icons["interrupted"] = QPixmap::fromImage(load_xpm(Icons::interrupted_xpm, Chthon::size_of_array(Icons::interrupted_xpm)));

	Settings settings;
	settings.load();
	if(DEBUG) {
		makeDebugSettings(settings);
	}
	updateDescription(settings);

	pomodoro = new Pomodoro(settings, this);

	if(single_shot) {
		bool ok = false;
		int length = QInputDialog::getInt(this, tr("Single shot interval"),
				tr("Set interval (in minutes):"), 5, 1, 60, 1, &ok);
		if(!ok) {
			exit(0);
		}
		single_shot_timer = new QTimer(this);
		single_shot_timer->setInterval(length * Settings::MINUTE);
		single_shot_timer->setSingleShot(true);
		connect(single_shot_timer, SIGNAL(timeout()),
				this, SLOT(single_shot_fired()));
		single_shot_timer->start();
	} else {
		connect(pomodoro, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));
		connect(ui.startOrInterrupt, SIGNAL(clicked()), pomodoro, SLOT(startOrInterrupt()));
		connect(pomodoro, SIGNAL(timeLeftChanged(int)), this, SLOT(changeTimeLeft(int)));
	}

	tray = new QSystemTrayIcon(this);
	if(!single_shot) {
		QMenu * trayMenu = new QMenu(this);
		trayMenu->addAction(tr("Start/interrupt"), pomodoro, SLOT(startOrInterrupt()));
		trayMenu->addSeparator();
		trayMenu->addAction(tr("Settings..."), this, SLOT(toggleVisibility()));
		trayMenu->addSeparator();
		trayMenu->addAction(tr("Quit"), this, SLOT(close()));
		tray->setContextMenu(trayMenu);
	}
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activateFromTray(QSystemTrayIcon::ActivationReason)));

	if(single_shot) {
		changeState(Pomodoro::ON_RUN);
	} else {
		changeState(Pomodoro::NONE);
	}
	if(DEBUG) {
		show();
	} else {
		hide();
	}
}

MainWindow::~MainWindow()
{
	if(!DEBUG) {
		pomodoro->getSettings().save();
	}
	saveWindowState();
}

void MainWindow::single_shot_fired()
{
	changeState(Pomodoro::BREAK_ENDED);
}

void MainWindow::activateFromTray(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger) {
		if(single_shot) {
			close();
		} else {
			pomodoro->startOrInterrupt();
		}
	}
}

void MainWindow::toggleVisibility()
{
	setVisible(!isVisible());
}

void MainWindow::updateDescription(const Settings & settings)
{
	static const QString description_text = tr(
			"Press button to start time run.\n"
			"After <a href=#pomodoro-length>%1</a> minutes it will end and execute <a href=#start-command>[%4]</a> command.\n"
			"Then a break for <a href=#break-length>%2</a> minutes starts, after which <a href=#end-command>[%5]</a> command is executed.\n"
			"Then it <a href=#autorestart>%3</a>.\n"
			);
	ui.description->setText(
			description_text
			.arg(settings.getPomodoroLength() / Settings::MINUTE)
			.arg(settings.getBreakLength() / Settings::MINUTE)
			.arg(settings.getAutorestart() ? "starts again" : "stopped")
			.arg(settings.getStartCommand())
			.arg(settings.getEndCommand())
			);
}

void MainWindow::on_description_linkActivated(const QString & link)
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	if(link == "#autorestart") {
		settings.setAutorestart(!settings.getAutorestart());
	} else if(link == "#pomodoro-length") {
		int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New pomodoro length in minutes:"), settings.getPomodoroLength() / Settings::MINUTE, 1, 60, 1, &ok);
		if(ok) {
			settings.setPomodoroLength(newValue * Settings::MINUTE);
		}
	} else if(link == "#break-length") {
		int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New break length in minutes:"), settings.getBreakLength() / Settings::MINUTE, 1, 60, 1, &ok);
		if(ok) {
			settings.setBreakLength(newValue * Settings::MINUTE);
		}
	} else if(link == "#start-command") {
		QString newValue = QInputDialog::getText(this, tr("Settings"), tr("New start break command:"), QLineEdit::Normal, settings.getStartCommand(), &ok);
		if(ok) {
			settings.setStartCommand(newValue);
		}
	} else if(link == "#end-command") {
		QString newValue = QInputDialog::getText(this, tr("Settings"), tr("New end break command:"), QLineEdit::Normal, settings.getEndCommand(), &ok);
		if(ok) {
			settings.setEndCommand(newValue);
		}
	}
	if(ok) {
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
	ui.statistics->setText(
			tr("Total pomodoros taken: %1")
			.arg(pomodoro->totalPomodorosTaken())
			);

	switch(event) {
		case Pomodoro::BREAK: QProcess::startDetached(pomodoro->getSettings().getStartCommand()); break;
		case Pomodoro::BREAK_ENDED: QProcess::startDetached(pomodoro->getSettings().getEndCommand()); break;
		default: break;
	}

	QString iconName = eventIcons.contains(event) ? eventIcons[event] : "ready";
	ui.icon->setPixmap(icons[iconName]);
	if(ui.icon->pixmap()) {
		tray->setIcon(*ui.icon->pixmap());
	}

	stateName = eventNames.contains(event) ? eventNames[event] : tr("Get ready");
	ui.state->setText(stateName);
	tray->setToolTip(ui.state->text());

	tray->show();
}

void MainWindow::changeTimeLeft(int msecs_left)
{
	QTime time_left;
	time_left = time_left.addMSecs(msecs_left);
	QString stateWithTime = stateName + tr(" (%1 left)").arg(time_left.toString());
	tray->setToolTip(stateWithTime);
	ui.state->setText(stateWithTime);
}
