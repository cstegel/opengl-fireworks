#include "audio/AudioManager.hpp"
#include "core/AssetManager.hpp"
#include "core/Game.hpp"

// don't use AL/ prefix since cmake adds this dir as the include_dir
#include <al.h>
#include <alc.h>
#include <alut.h>

#include <stdio.h>

namespace fw
{

AudioManager::AudioManager(Game & game) : game(game)
{
	alutInit(0, NULL);
}

AudioManager::~AudioManager()
{
	alutExit();
}

void AudioManager::Play(const string & audioFile)
{
	ALuint buffer, source;
	ALint state;

	// Capture errors
	alGetError();

	// Load pcm data into buffer
	string audioFilepath = AssetManager::GetAudioPath(audioFile);
	buffer = alutCreateBufferFromFile(audioFilepath.c_str());
	if (buffer == AL_NONE)
	{
		throw std::runtime_error(
			"error loading sound \"" + audioFilepath + "\": "
			+ alutGetErrorString(alutGetError()));
	}
	// buffer = alutCreateBufferHelloWorld();

	// Create sound source (use buffer to fill source)
	alGenSources(1, &source);
	alSourcei(source, AL_BUFFER, buffer);

	// Play
	alSourcePlay(source);

	// Wait for the song to complete
	do {
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	} while (state == AL_PLAYING);

	// Clean up sources and buffers
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
}



}