#include "audio/AudioManager.hpp"
#include "core/AssetManager.hpp"
#include "core/Game.hpp"

#include <Ecs.hh>

#include <stdio.h>

namespace fw
{

AudioManager::AudioManager(Game & game) : game(game)
{
	alutInit(0, NULL);

	// drain errors
	while(alGetError());

	// ALuint buffer = alutCreateBufferFromFile(AssetManager::GetAudioPath("monstrous_cow.wav").c_str());
	//
	// ALuint source;
	// alGenSources(1, &source);
	// alSourcei(source, AL_BUFFER, buffer);
	//
	// alSourcePlay(source);

	loadSoundBuffers();
}

AudioManager::~AudioManager()
{
	alutExit();
}

void AudioManager::loadSoundBuffers()
{
	string audioFilepath = AssetManager::GetAudioPath("monstrous_cow.wav");
	sounds.emplace(SoundId::DRAGON_ROAR, audioFilepath);
}

bool AudioManager::Frame()
{
	float listenerOri[] = {0, 0, 1, 0, 1, 0}; // lookAt, Up
	// update listener kinematics
	// TODO
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	for (auto & pair : sounds)
	{
		Sound & sound = pair.second;
		sound.DeleteFinishedSources();
		sound.UpdateSourceKinematics();
	}

	return true;
}

void AudioManager::Play(SoundId sound, ecs::Entity location)
{
	if (sounds.count(sound) < 1)
	{
		throw std::runtime_error("sound " + string(static_cast<int>(sound) + " not loaded"));
	}

	if (!sounds.at(sound).PlayNewSound(location))
	{
		throw std::runtime_error("failed to play sound " + to_string(static_cast<int>(sound)));
	}
}

}