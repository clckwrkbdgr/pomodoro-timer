TARGET = pomodoro

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

unix:LIBS += -lSDL -lSDL_mixer
#HEADERS = mainwindow.h sounds.h pomodoro.h
SOURCES = main.cpp #mainwindow.cpp sounds.cpp pomodoro.cpp
modules = mainwindow sounds pomodoro
for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
