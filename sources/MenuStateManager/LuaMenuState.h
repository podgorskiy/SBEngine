#pragma once
#include "BaseMenuState.h"
#include <string>


typedef struct lua_State lua_State;

namespace UI
{
	class Block;

	typedef std::shared_ptr<Block> BlockPtr;
}

class LuaMenuState: public BaseMenuState
{
public:
	LuaMenuState(const fsal::File& yaml, MenuStateManager* msm);
	~LuaMenuState();

	void Update(UI::View viewbox, float time, float deltaTime) override;
	void Draw(UI::View viewbox, UI::Renderer* rd) override;

protected:
	lua_State* m_lua;
	std::vector<char*> m_history;
	std::function<void(const char*)> m_printer;
	// virtual void OnEvent(EventBase* event) {};
};

typedef std::shared_ptr<IMenuState> IMenuStatePtr;
