TARGET = pomodoro
#CONFIG += POMODORO_TEST

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

QMAKE_CXXFLAGS += -std=c++11

LIBS += -lchthon2

POMODORO_TEST {
	DEFINES += POMODORO_TEST
	QT += testlib
	SOURCES = pomodoro.cpp settings.cpp
	HEADERS = pomodoro.h settings.h
} else {
	FORMS = mainwindow.ui
	RESOURCES = pomodoro.qrc
	SOURCES = main.cpp
	modules = mainwindow pomodoro settings
	for(module, modules) {
		HEADERS += $${module}.h
		SOURCES += $${module}.cpp
	}
}
deb.depends = $(TARGET)
deb.commands = @debpackage.py \
		pomodoro-timer \
		-v 0.1.0 \
		--maintainer \'umi041 <umi0451@gmail.com>\' \
		--bin $(TARGET) \
		--build-dir tmp \
		--dest-dir . \
		--description \'Simple pomodoro timer.\'
QMAKE_EXTRA_TARGETS += deb
