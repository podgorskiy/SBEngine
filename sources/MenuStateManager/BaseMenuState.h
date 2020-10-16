#pragma once
#include "IMenuState.h"
#include <string>
#include <fsal.h>
#include <UI/Block.h>


namespace UI
{
	class Block;

	typedef std::shared_ptr<Block> BlockPtr;
}

namespace core
{
	class EventDispatcher;
}

class MenuStateManager;


class BaseMenuState: public IMenuState
{
public:
	BaseMenuState(const fsal::File& yaml, MenuStateManager* msm);
	virtual void OnPush(float time)
	{
		auto it = m_animation.find("on_push");
		if (it != m_animation.end())
		{
			for (auto& b:it->second)
			{
				for (auto& c: b.second)
				{
					b.first->PushTargetTransitionConstraints(c);
				}
			}
		}
	};

	void Update(Render::View viewbox, float time, float deltaTime) override;
	void Draw(Render::View viewbox, Render::Renderer2D* rd) override;
	std::string GetName() override;

protected:
	UI::BlockPtr m_root;
	UI::Animation m_animation;
	MenuStateManager* m_msm;
	std::string m_name;
	float m_time;
	core::EventDispatcher* m_eventDispatcher;
	// virtual void OnEvent(EventBase* event) {};
};
