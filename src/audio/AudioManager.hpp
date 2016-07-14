#pragma once

#include "Common.hpp"
#include "audio/SoundId.hpp"
#include "audio/Sound.hpp"
#include "ecs/components/Transform.hpp"

#include <Ecs.hh>

// normally from <AL/al.h> and so on but CMake forces this for OS compatibility
#include <al.h>
#include <alc.h>
#include <alut.h>

namespace fw
{

class Game;


class AudioManager
{
public:
	AudioManager(Game & game);
	~AudioManager();

	/**
	 * location is used for the sound's position/velocity
	 * It must have a Transform component at the very minimum.
	 */
	void Play(SoundId sound, ecs::Entity location);
	bool Frame();

private:
	void loadSoundBuffers();

	Game & game;

	unordered_map<SoundId, Sound, EnumHash> sounds;
};

}