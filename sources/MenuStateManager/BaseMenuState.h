#pragma once
#include "IMenuState.h"
#include <string>
#include <fsal.h>


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

	void Update(UI::View viewbox, float time, float deltaTime) override;
	void Draw(UI::View viewbox, UI::Renderer* rd) override;
	std::string GetName() override;

protected:
	UI::BlockPtr m_root;
	MenuStateManager* m_msm;
	std::string m_name;
	// virtual void OnEvent(EventBase* event) {};
};
