#pragma once

#include "StepOffer.h"
#include "MAP_OBJECT.h"
#include <sstream>
#include <fstream>
#include "distcache.h"

struct Queen : public MAP_OBJECT
{
	Queen(std::stringstream& command_buffer_, DISTCACHE& mDistCache_,	std::ofstream& mDebugLog_):
		command_buffer{command_buffer_},
		mDistCache{mDistCache_},
		mDebugLog{mDebugLog_}
	{}
	std::stringstream& command_buffer;
	DISTCACHE& mDistCache;
	std::ofstream& mDebugLog;
	StepOffer CalcOffer()
	{
		return StepOffer();
	}
};