TARGET = pomodoro

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

unix:LIBS += -lSDL -lSDL_mixer
FORMS = mainwindow.ui
SOURCES = main.cpp
modules = mainwindow sounds pomodoro settings
for(module, modules) {
	HEADERS += $${module}.h
	SOURCES += $${module}.cpp
}
