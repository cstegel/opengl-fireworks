#pragma once

#include "Common.hpp"

#include "audio/SoundId.hpp"
#include "ecs/components/Transform.hpp"

#include <Ecs.hh>

#include <al.h>
#include <alc.h>
#include <alut.h>

namespace fw
{

class Sound
{
public:
	Sound();
	Sound(const string & filepath);
	Sound(const char * filepath);
	~Sound();

	/**
	 * Plays a new instance of the sound.
	 * location is used for position/velocity and must have a Transform
	 * component at the very least.
	 * Returns the that can be used to refer to the spawned instance.
	 */
	ALuint PlayNewSound(ecs::Entity location);

	// return true if successful, false if the soundInstance no longer exists
	bool Play(ALuint soundInstance);
	bool Pause(ALuint soundInstance);
	bool Stop(ALuint soundInstance);
	void DeleteFinishedSources();
	void UpdateSourceKinematics();

	string filepath;

private:
	void validateEntity(ecs::Entity ent) const;
	void checkALErrors() const;

	ALuint buffer;
	unordered_map<ALuint, ecs::Entity> sources;
};

}