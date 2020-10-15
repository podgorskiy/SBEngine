#pragma once
#include "Block.h"
#include <fsal.h>


namespace UI
{
	typedef std::map<std::string, Render::color> ColorMap;
	typedef std::map<std::string, int> IntMap;
	BlockPtr Load(const fsal::File& f, const IntMap& tf_map, const IntMap& shader_map);
}

