#include "audio/Sound.hpp"

namespace fw
{

Sound::Sound() : buffer(AL_NONE) {}

Sound::Sound(const string & filepath) : Sound(filepath.c_str()) {}

Sound::Sound(const char * filepath) : filepath(filepath)
{
	ALuint buffer;

	buffer = alutCreateBufferFromFile(this->filepath.c_str());
	if (buffer == AL_NONE)
	{
		throw std::runtime_error(
			"error loading sound \"" + this->filepath + "\": "
			+ alutGetErrorString(alutGetError()));
	}
}

Sound::~Sound()
{
	for (auto & pair : sources)
	{
		ALuint source = pair.first;
		alDeleteSources(1, &source);
	}

	if (buffer != AL_NONE)
	{
		alDeleteBuffers(1, &buffer);
	}
}

ALuint Sound::PlayNewSound(ecs::Entity location)
{
	validateEntity(location);

	ALuint source;

	alGenSources(1, &source);

	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcei(source, AL_LOOPING, AL_FALSE);

	sources[source] = location;

	checkALErrors();

	return Play(source);
}

void Sound::validateEntity(ecs::Entity ent) const
{
	if (!ent.Has<Transform>())
	{
		throw std::invalid_argument("entity for a sound must have a Transform component");
	}
}

bool Sound::Play(ALuint source)
{
	if (sources.count(source) < 1)
	{
		return false;
	}

	alSourcePlay(source);
	checkALErrors();
	return true;
}

bool Sound::Pause(ALuint source)
{
	if (sources.count(source) < 1)
	{
		return false;
	}

	alSourcePause(source);
	checkALErrors();
	return true;
}

bool Sound::Stop(ALuint source)
{
	if (sources.count(source) < 1)
	{
		return false;
	}

	alSourceStop(source);
	checkALErrors();
	return true;
}

void Sound::UpdateSourceKinematics()
{
	for (auto & pair : sources)
	{
		ALuint source = pair.first;
		ecs::Entity location = pair.second;

		validateEntity(location);
		glm::mat4 transform = location.Get<Transform>()->GetModelTransform(*location.GetManager());
		glm::vec4 pos = transform * glm::vec4(0, 0, 0, 1);

		alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
		alSource3f(source, AL_VELOCITY, 0, 0, 0); // TODO
	}

	checkALErrors();
}

void Sound::DeleteFinishedSources()
{
	vector<ALuint> toErase;

	for (auto & pair : sources)
	{
		ALuint source = pair.first;
		ALint state;

		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING)
		{
			continue;
		}

		alDeleteSources(1, &source);
		toErase.push_back(source);
	}

	checkALErrors();

	for (ALuint source : toErase)
	{
		sources.erase(source);
	}
}

void Sound::checkALErrors() const
{
	ALuint err = alutGetError();
	if (err != AL_NONE)
	{
		throw std::runtime_error("error: " + string(alutGetErrorString(err)));
	}
}

}