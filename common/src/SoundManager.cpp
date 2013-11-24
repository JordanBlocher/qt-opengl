#include "SoundManager.hpp"

SoundManager::SoundManager(const char* name)
{
	this->name = name;
	// Setup audio output for bgm
	bgmOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	bgmObject = new Phonon::MediaObject(this);
	bgmObject->setTickInterval(1000);
	Phonon::createPath(bgmObject, bgmOutput);

	// Add bgm files to sources
	bgmSources.append(Phonon::MediaSource("assets/sound/RalphieHolyGrail.mp3"));
	bgmSources.append(Phonon::MediaSource("assets/sound/ZomboyHoedown.mp3"));
	bgmSources.append(Phonon::MediaSource("assets/sound/KonecRailgun.mp3"));

	// Setup audio outputs for sound effects
	for(int i = 0; i < 5; i++)
	{
		audioOutputs.append(new Phonon::AudioOutput(Phonon::MusicCategory, this));
		mediaObjects.append(new Phonon::MediaObject(this));
		mediaObjects[i]->setTickInterval(1000);
		Phonon::createPath(mediaObjects[i], audioOutputs[i]);
	}

	// Add sound effect files to sources
	sources.append(Phonon::MediaSource("assets/sound/hit.mp3"));

	// Connect signals to play sounds
	QObject::connect(this, SIGNAL(playBgm(int)), this, SLOT(playBgmWorker(int)));
	QObject::connect(this, SIGNAL(playEffect(int)), this, SLOT(playEffectWorker(int)));
	QObject::connect(bgmObject, SIGNAL(finished()), SLOT(finished()));
}

void SoundManager::emitPlayBgm(int bgmNum)
{
	emit playBgm(bgmNum);
}

void SoundManager::emitPlayEffect(int effectNum)
{
	emit playEffect(effectNum);
}

void SoundManager::playBgmWorker(int bgmNum)
{
	// Check if current playing bgm is the same one that is being requested
	if( bgmObject->currentSource().fileName() != bgmSources[bgmNum].fileName())
	{
		bgmObject->stop();
		bgmObject->setCurrentSource(bgmSources[bgmNum]);
		bgmObject->play();
	}

}

void SoundManager::playEffectWorker(int effectNum)
{
	// Check to see if there is an available sound output
	for(int i = 0; i < 5; i ++)
	{
		// Check if this mediaObject is able to play our effect
		if(mediaObjects[i]->state() == Phonon::StoppedState)
		{
			mediaObjects[i]->setCurrentSource(sources[effectNum]);
			mediaObjects[i]->play();
			break;
		}
	}
}

void SoundManager::finished()
{
	bgmObject->play();
}