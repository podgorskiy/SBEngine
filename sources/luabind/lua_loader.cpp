#include "lua_loader.h"

#define lua_c
extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <LuaBridge/LuaBridge.h>

#include <MenuStateManager/MenuStateManager.h>
#include <MenuStateManager/IMenuState.h>

namespace luabridge
{
	template<typename T>
	struct ContainerTraits<std::shared_ptr<T> >
	{
		typedef T Type;

		static T* get(const std::shared_ptr<T>& c)
		{
			return c.get();
		}
	};
}


void LoadUI_API(lua_State* L)
{
	using  namespace luabridge;

	getGlobalNamespace (L)
	  .beginNamespace ("ui")
	    .beginClass<MenuStateManager>("MenuStateManager")
	      .addFunction ("pop", &MenuStateManager::Pop)
	      .addFunction ("update", &MenuStateManager::Update)
	      .addFunction ("push", &MenuStateManager::Push)
	      .addFunction ("get_front_state", &MenuStateManager::GetFrontState)
	    .endClass ()
	  .endNamespace();

	getGlobalNamespace (L)
	  .beginNamespace ("ui")
	    .beginClass<IMenuState>("IMenuState")
	      .addFunction ("get_name", &IMenuState::GetName)
	    .endClass ()
	  .endNamespace();

	getGlobalNamespace (L)
	  .beginNamespace ("ui")
	    .beginClass<Render::View>("View")
	      .addProperty("dpi", &Render::View::dpi)
	      .addProperty("box", &Render::View::view_box)
	      .addFunction("get_aspect", &Render::View::GetAspect)
	      .addFunction("get_ppd_scale", &Render::View::GetPixelPerDotScalingFactor)
	    .endClass ()
	  .endNamespace();

	getGlobalNamespace (L)
	  .beginNamespace ("math")
	    .beginClass<glm::aabb2>("aabb2")
	      .addProperty("maxp", &glm::aabb2::maxp)
	      .addProperty("minp", &glm::aabb2::minp)
	      .addFunction("size", &glm::aabb2::size)
	      .addFunction("center", &glm::aabb2::center)
	      .addFunction("is_negative", &glm::aabb2::is_negative)
	      .addFunction("is_positive", &glm::aabb2::is_positive)
	      .addFunction("set", &glm::aabb2::set)
	      .addFunction("reset", &glm::aabb2::reset)
	      .addFunction("set_any", &glm::aabb2::set_any)
	    .endClass ()
	  .endNamespace();

	getGlobalNamespace (L)
	  .beginNamespace ("math")
	    .beginClass<glm::vec2>("vec2")
	      .addProperty("x", &glm::vec2::x)
	      .addProperty("y", &glm::vec2::y)
	    .endClass ()
	  .endNamespace();

}

