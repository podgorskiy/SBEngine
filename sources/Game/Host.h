#pragma once
#include <string>

class Host
{
public:
	virtual std::string GetHostHame() = 0;
	virtual int GetHostID() = 0;
};
