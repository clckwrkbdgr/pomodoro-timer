#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	// Pomodoro lasts for <25> minutes followed up by a short break for <5> minutes.
	// Every <4> pomodoros a long break for <15> minutes should be taken.
	// 
	// Pomodoro runs by a click on the tray: its changed to <red>.
	// Every quarter of the pomodoro length a quarter of then the tray icon turns <green>
	// When pomodoro is over, it should be all <green> and the <ring> will bell.
	// When break is over, the tray icon starts flashing <red>.
	// When you're ready to start, click the tray icon again: it will run another pomodoro.
	// If you want to interrupt current pomodoro, click on the tray icon.
	// It will run a short break after which interrupted pomodoro will be started anew.

}
