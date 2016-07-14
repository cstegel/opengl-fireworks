#include "AssetManager.hpp"
#include "core/Game.hpp"

namespace fw
{
	string AssetManager::GetModelPath(const string & modelName, const string & fileType)
	{
		return Game::RootDir() + "/assets/models/" + modelName + "/" + modelName + "." + fileType;
	}

	string AssetManager::GetAudioPath(const string & filename)
	{
		return Game::RootDir() + "/assets/audio/" + filename;
	}
}