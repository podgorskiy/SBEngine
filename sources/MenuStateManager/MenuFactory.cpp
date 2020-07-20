#include "MenuFactory.h"
#include "UI/Parsers.h"
#include "utils/assertion.h"
#include "MenuStateManager/MenuStateManager.h"
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include <memory>


std::function<IMenuStatePtr(const fsal::File&, MenuStateManager*)> MenuRegisterer::GetConstructor(const char* type)
{
	auto it = GetMap().find(type);
	ASSERT(it != GetMap().end(), "Could not find menu class %s", type);
	return it->second;
}
