#pragma once

#include "Common.hpp"

namespace fw
{
	class AssetManager
	{
	public:
		static string GetModelPath(const string & modelName, const string & fileType);
		static string GetAudioPath(const string & filename);
	};
}