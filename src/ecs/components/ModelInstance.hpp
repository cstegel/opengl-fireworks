#pragma once

#include "Common.hpp"

namespace fw
{

struct ModelInstance
{
	ModelInstance() = default;
	ModelInstance(string modelId) : modelId(modelId) {}

	// ID that can look up the model in GrahpicsManager (dir under /assets/models/)
	string modelId;
};

}