#pragma once

#include "Common.hpp"

namespace fw
{

class Game;

class AudioManager
{
public:
	AudioManager(Game & game);
	~AudioManager();

	void Play(const string & audioFile);

private:
	Game & game;
};

}