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

	loadSoundBuffers();
}

AudioManager::~AudioManager()
{
	alutExit();
}

void AudioManager::SetPlayerLocation(ecs::Entity player)
{
	validateEntity(player);
	this->player = player;
}

void AudioManager::validateEntity(ecs::Entity ent) const
{
	if (!ent.Has<Transform>())
	{
		throw std::runtime_error("entity must have a transform component");
	}
}

void AudioManager::loadSoundBuffers()
{
	string audioFilepath;
	audioFilepath = AssetManager::GetAudioPath("monstrous_cow.wav");
	sounds.emplace(SoundId::DRAGON_ROAR, audioFilepath);

	audioFilepath = AssetManager::GetAudioPath("small_firework_explosion2.wav");
	sounds.emplace(SoundId::FIREWORK_SMALL_EXPLOSION, audioFilepath);

	audioFilepath = AssetManager::GetAudioPath("whistle_firework_launch.wav");
	sounds.emplace(SoundId::FIREWORK_WHISTLE_LAUNCH, audioFilepath);
}

bool AudioManager::Frame()
{
	validateEntity(player);

	ecs::Handle<Transform> transform = player.Get<Transform>();

	glm::vec3 position = transform->GetPosition();
	glm::vec3 forward = transform->GetForward(game.GetWorldForward());

	glm::vec3 up = game.GetWorldUp();

	float listenerOri[] = {
		forward.x, forward.y, forward.z,
		up.x, up.y, up.z
	};

	alListener3f(AL_POSITION, position.x, position.y, position.z);
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
		throw std::runtime_error("failed to play sound " + std::to_string(static_cast<int>(sound)));
	}
}

}