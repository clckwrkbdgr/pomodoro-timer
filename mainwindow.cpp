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
#include "settings.h"
#include "pomodoro.h"
#include "mainwindow.h"

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

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
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

	icons["start"] = QPixmap(":/res/icons/start.png");
	icons["break"] = QPixmap(":/res/icons/break.png");
	icons["ready"] = QPixmap(":/res/icons/ready.png");
	icons["interrupted"] = QPixmap(":/res/icons/interrupted.png");

	Settings settings;
	settings.load();
	if(DEBUG) {
		makeDebugSettings(settings);
	}
	updateDescription(settings);

	pomodoro = new Pomodoro(settings, this);
	connect(pomodoro, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));
	connect(ui.startOrInterrupt, SIGNAL(clicked()), pomodoro, SLOT(startOrInterrupt()));

	tray = new QSystemTrayIcon(this);
	QMenu * trayMenu = new QMenu(this);
	trayMenu->addAction(tr("Start/interrupt"), pomodoro, SLOT(startOrInterrupt()));
	trayMenu->addSeparator();
	trayMenu->addAction(tr("Settings..."), this, SLOT(toggleVisibility()));
	trayMenu->addSeparator();
	trayMenu->addAction(tr("Quit"), this, SLOT(close()));
	tray->setContextMenu(trayMenu);
	connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activateFromTray(QSystemTrayIcon::ActivationReason)));

	changeState(Pomodoro::NONE);
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

void MainWindow::activateFromTray(QSystemTrayIcon::ActivationReason reason)
{
	if(reason == QSystemTrayIcon::Trigger) {
		pomodoro->startOrInterrupt();
	}
}

void MainWindow::toggleVisibility()
{
	setVisible(!isVisible());
}

void MainWindow::updateDescription(const Settings & settings)
{
	ui.description->setText(
			tr("Pomodoro length: %1 minutes.\nBreak: %2 minutes.\nStart break command: <%3>.\nEnd break command: <%4>.")
			.arg(settings.getPomodoroLength() / Settings::MINUTE)
			.arg(settings.getBreakLength() / Settings::MINUTE)
			.arg(settings.getStartCommand())
			.arg(settings.getEndCommand())
			);
}

void MainWindow::on_startSound_clicked()
{
	Settings settings = pomodoro->getSettings();
	QString newValue = QInputDialog::getText(this, tr("New start break command..."), 0, QLineEdit::Normal, settings.getStartCommand());
	if(!newValue.isEmpty()) {
		settings.setStartCommand(newValue);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
}

void MainWindow::on_endSound_clicked()
{
	Settings settings = pomodoro->getSettings();
	QString newValue = QInputDialog::getText(this, tr("New end break command..."), 0, QLineEdit::Normal, settings.getEndCommand());
	if(!newValue.isEmpty()) {
		settings.setEndCommand(newValue);
		pomodoro->setSettings(settings);
		updateDescription(settings);
	}
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

void MainWindow::on_breakLength_clicked()
{
	Settings settings = pomodoro->getSettings();
	bool ok = false;
	int newValue = QInputDialog::getInt(this, tr("Settings"), tr("New break length in minutes:"), settings.getBreakLength() / Settings::MINUTE, 1, 60, 1, &ok);
	if(ok) {
		settings.setBreakLength(newValue * Settings::MINUTE);
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

	QString stateName = eventNames.contains(event) ? eventNames[event] : tr("Get ready");
	ui.state->setText(stateName);
	tray->setToolTip(ui.state->text());

	tray->show();
}

