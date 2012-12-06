#pragma once
#include <QtGui/QWidget>
class QLineEdit;
class Sounds;
class Pomodoro;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private slots:
	void changeState(int event);
private:
	Pomodoro * pomodoro;
	QLineEdit * editTest;
	Sounds * sounds;
};
