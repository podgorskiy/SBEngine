#pragma once
#include "IApplication.h"
#include <memory>

namespace ApplicationFactory
{
	std::shared_ptr<IApplication> NewApplication(int argc, char **argv);
}
