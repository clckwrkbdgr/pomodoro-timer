#pragma once
#include <QtCore/QObject>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

class Sounds : public QObject {
public:
	Sounds(QObject * parent = 0);
	virtual ~Sounds();

	//void initSDL();
	//void closeSDL();

	bool loadSound(const QString & sound, const QString & fileName);
	bool playSound(const QString & sound);
private:
	QMap<QString, Mix_Chunk*> sounds;

	Mix_Chunk * loadSound(const QString & fileName);
	bool playSound(Mix_Chunk * sound);
};

