#pragma once
#include "IMenuState.h"
#include "EventDispatcher/EventDispatcher.h"
#include <string>
#include <fsal.h>
#include <UI/Block.h>


namespace UI
{
	class Block;

	typedef std::shared_ptr<Block> BlockPtr;
}

class MenuStateManager;


class BaseMenuState: public IMenuState
{
public:
	BaseMenuState(const fsal::File& yaml, MenuStateManager* msm);

	void OnPush(float time) override;
	void OnPop(float time) override;

	void Update(Render::View viewbox, float time, float deltaTime) override;
	void Draw(Render::View viewbox, Render::Renderer2D* rd) override;
	std::string GetName() override;

	bool OnUserInput(uint64_t eventID, core::EventDispatcher::Box box);

protected:
	glm::vec2 m_cursor_pos;
	bool m_cursor_down = false;
	bool m_cursor_action = false;

	UI::BlockPtr m_root;
	UI::Animation m_animation;
	MenuStateManager* m_msm;
	std::string m_name;
	float m_time;
	int m_frame;
	core::EventDispatcher* m_eventDispatcher;
	// virtual void OnEvent(EventBase* event) {};
};
