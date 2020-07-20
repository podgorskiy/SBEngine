#pragma once
#include <fsal.h>
#include <spdlog/spdlog.h>
#include "IMenuState.h"
#include <functional>
#include <string>
#include <map>


class MenuStateManager;


class MenuRegisterer
{
private:
	typedef std::map<std::string, std::function<IMenuStatePtr(const fsal::File&, MenuStateManager*)> > CtrMap;

	static CtrMap& GetMap()
	{
		static CtrMap m_factoryMap;
		return m_factoryMap;
	}
public:
	template<class T>
	class _MenuRegisterer
	{
	public:
		explicit _MenuRegisterer(const char* name)
		{
			GetMap()[name] = [](const fsal::File& yaml, MenuStateManager* msm) -> IMenuStatePtr
			{ return IMenuStatePtr((IMenuState*)new T(yaml, msm)); };
			spdlog::info("Registered Menu %s class", name);
		}
	};

	static std::function<IMenuStatePtr(const fsal::File&, MenuStateManager*)> GetConstructor(const char* type);

private:

};

#define RegisterMenu(X) auto _r_##X = MenuRegisterer::_MenuRegisterer<X>(#X)
