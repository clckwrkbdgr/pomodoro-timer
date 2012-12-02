#include <QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTextBrowser>
#include <QtGui/QInputDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>
#include "mainwindow.h"

namespace Defaults {
	const int pomodoroLength    = 25; // minutes.
	const int minPomodoroLength = 10; // minutes.
	const int maxPomodoroLength = 30; // minutes.
	const int shortBreakLength    = 5; // minutes.
	const int minShortBreakLength = 3; // minutes.
	const int maxShortBreakLength = 10; // minutes.
	const int pomodoroBlockSize    = 4; // pomodoros.
	const int minPomodoroBlockSize = 1; // pomodoros.
	const int maxPomodoroBlockSize = 6; // pomodoros.
	const int longBreakLength    = 15; // minutes.
	const int minLongBreakLength = 15; // minutes.
	const int maxLongBreakLength = 45; // minutes.
	const QColor runningPomodoroColor = Qt::red;
	const QColor passedQuarterColor   = Qt::green;
	const QColor breakColor           = Qt::green;
	const QString bellFile            = ":/sounds/bell.wav";
	const QColor readinessFlashColor  = Qt::red;

	QMap<QString, QString> makeAnchorToSettingNameMap() {
		QMap<QString, QString> result;
		result["#1"] = "pomodoroLength";
		result["#2"] = "shortBreakLength";
		result["#3"] = "pomodoroBlockSize";
		result["#4"] = "longBreakLength";
		result["#5"] = "runningPomodoroColor";
		result["#6"] = "passedQuarterColor";
		result["#7"] = "breakColor";
		result["#8"] = "bellFile";
		result["#9"] = "readinessFlashColor";
		return result;
	}
	const QMap<QString, QString> anchorToSettingName = makeAnchorToSettingNameMap();

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
};

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	QSettings settings;
	pomodoroLength       = settings.value("pomodoro/pomodoroLength",       Defaults::pomodoroLength).toInt();
	shortBreakLength     = settings.value("pomodoro/shortBreakLength",     Defaults::shortBreakLength).toInt();
	pomodoroBlockSize    = settings.value("pomodoro/pomodoroBlockSize",    Defaults::pomodoroBlockSize).toInt();
	longBreakLength      = settings.value("pomodoro/longBreakLength",      Defaults::longBreakLength).toInt();
	runningPomodoroColor = settings.value("pomodoro/runningPomodoroColor", Defaults::runningPomodoroColor).value<QColor>();
	passedQuarterColor   = settings.value("pomodoro/passedQuarterColor",   Defaults::passedQuarterColor).value<QColor>();
	breakColor           = settings.value("pomodoro/breakColor",           Defaults::breakColor).value<QColor>();
	bellFile             = settings.value("pomodoro/bellFile",             Defaults::bellFile).toString();
	readinessFlashColor  = settings.value("pomodoro/readinessFlashColor",  Defaults::readinessFlashColor).value<QColor>();
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	text = new QTextBrowser();
	connect(text, SIGNAL(anchorClicked(QUrl)), this, SLOT(changeSetting(QUrl)));

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addWidget(text);
	setLayout(hbox);

	updateText();
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("pomodoro/pomodoroLength",       pomodoroLength);
	settings.setValue("pomodoro/shortBreakLength",     shortBreakLength);
	settings.setValue("pomodoro/pomodoroBlockSize",    pomodoroBlockSize);
	settings.setValue("pomodoro/longBreakLength",      longBreakLength);
	settings.setValue("pomodoro/runningPomodoroColor", runningPomodoroColor);
	settings.setValue("pomodoro/passedQuarterColor",   passedQuarterColor);
	settings.setValue("pomodoro/breakColor",           breakColor);
	settings.setValue("pomodoro/bellFile",             bellFile);
	settings.setValue("pomodoro/readinessFlashColor",  readinessFlashColor);
	settings.setValue("mainwindow/maximized", windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}

}

void MainWindow::changeSetting(const QUrl & settingUrl)
{
	QString settingName = Defaults::anchorToSettingName[settingUrl.toString()];
	bool ok = false;
	if("pomodoroLength" == settingName) {
		int newValue = QInputDialog::getInt(this, tr("Change settings"), tr("New pomodoro length:"), pomodoroLength, Defaults::minPomodoroLength, Defaults::maxPomodoroLength, 1, &ok);
		if(ok) {
			pomodoroLength = newValue;
			updateText();
		}
	} else if("shortBreakLength" == settingName) {
		int newValue = QInputDialog::getInt(this, tr("Change settings"), tr("New short break length:"), shortBreakLength, Defaults::minShortBreakLength, Defaults::maxShortBreakLength, 1, &ok);
		if(ok) {
			shortBreakLength = newValue;
			updateText();
		}
	} else if("pomodoroBlockSize" == settingName) {
		int newValue = QInputDialog::getInt(this, tr("Change settings"), tr("New pomodoro block size:"), pomodoroBlockSize, Defaults::minPomodoroBlockSize, Defaults::maxPomodoroBlockSize, 1, &ok);
		if(ok) {
			pomodoroBlockSize = newValue;
			updateText();
		}
	} else if("longBreakLength" == settingName) {
		int newValue = QInputDialog::getInt(this, tr("Change settings"), tr("New long break length:"), longBreakLength, Defaults::minLongBreakLength, Defaults::maxLongBreakLength, 1, &ok);
		if(ok) {
			longBreakLength = newValue;
			updateText();
		}
	} else if("runningPomodoroColor" == settingName) {
		QColor newValue = QColorDialog::getColor(runningPomodoroColor, this, tr("Change running pomodoro color"));
		if(newValue.isValid()) {
			runningPomodoroColor = newValue;
			updateText();
		}
	} else if("passedQuarterColor" == settingName) {
		QColor newValue = QColorDialog::getColor(passedQuarterColor, this, tr("Change passed quarter color"));
		if(newValue.isValid()) {
			passedQuarterColor = newValue;
			updateText();
		}
	} else if("breakColor" == settingName) {
		QColor newValue = QColorDialog::getColor(breakColor, this, tr("Change break color"));
		if(newValue.isValid()) {
			breakColor = newValue;
			updateText();
		}
	} else if("readinessFlashColor" == settingName) {
		QColor newValue = QColorDialog::getColor(readinessFlashColor, this, tr("Change running pomodoro color"));
		if(newValue.isValid()) {
			readinessFlashColor = newValue;
			updateText();
		}
	} else if("bellFile" == settingName) {
		QString newValue = QFileDialog::getOpenFileName(this, tr("Change bell sound"), bellFile);
		if(!newValue.isEmpty()) {
			bellFile = newValue;
			updateText();
		}
	}
}

void MainWindow::updateText()
{
	text->setHtml(
			Defaults::TEXT
			.arg(pomodoroLength)
			.arg(shortBreakLength)
			.arg(pomodoroBlockSize)
			.arg(longBreakLength)
			.arg(runningPomodoroColor.name())
			.arg(passedQuarterColor.name())
			.arg(breakColor.name())
			.arg(QFileInfo(bellFile).fileName())
			.arg(readinessFlashColor.name())
			);
}
