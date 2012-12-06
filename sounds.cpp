#include <QtDebug>
#include "sounds.h"

Sounds::Sounds(QObject * parent)
	: QObject(parent)
{
	if(SDL_Init(SDL_INIT_AUDIO) != 0) {
		qDebug() << QObject::tr("Unable to initialize SDL: %1").arg(SDL_GetError());
		return;
	}

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		qDebug() << QObject::tr("Unable to initialize audio: %1").arg(Mix_GetError());
	}
}

Sounds::~Sounds()
{
	foreach(const QString & sound, sounds.keys()) {
		Mix_FreeChunk(sounds[sound]);
	}

	Mix_CloseAudio();
	SDL_Quit();
}

Mix_Chunk * Sounds::loadSound(const QString & fileName)
{
	Mix_Chunk *sound = NULL;
	sound = Mix_LoadWAV(fileName.toAscii().constData());
	if(sound == NULL) {
		qDebug() << QObject::tr("Unable to load WAV file: %1").arg(Mix_GetError());
	}
	return sound;
}

bool Sounds::playSound(Mix_Chunk * sound)
{
	int channel;
	 
	channel = Mix_PlayChannel(-1, sound, 0);
	if(channel == -1) {
		qDebug() << QObject::tr("Unable to play WAV file: %1").arg(Mix_GetError());
		return false;
	}
	return true;
}

bool Sounds::loadSound(const QString & sound, const QString & fileName)
{
	Mix_Chunk * chunk = loadSound(fileName);
	if(!chunk)
		return false;
	sounds[sound] = chunk;
	return true;
}

bool Sounds::playSound(const QString & sound)
{
	Mix_Chunk * chunk = sounds[sound];
	if(chunk) {
		return playSound(chunk);
	}
	return false;
}

