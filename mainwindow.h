#pragma once

#include <QtGui/QWidget>

class QTextBrowser;
class QUrl;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private slots:
	void updateText();
	void changeSetting(const QUrl & settingUrl);
private:
	QTextBrowser * text;
	int pomodoroLength;
	int shortBreakLength;
	int pomodoroBlockSize;
	int longBreakLength;
	QColor runningPomodoroColor;
	QColor passedQuarterColor;
	QColor breakColor;
	QString bellFile;
	QColor readinessFlashColor;
};
