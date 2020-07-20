#pragma once
#include "UI/View.h"
#include "UI/UI_Backend.h"
#include <memory>
#include <string>


class IMenuState
{
public:
	IMenuState() = default;
	virtual ~IMenuState() = default;

	virtual void OnPush() {};
	virtual void OnResume() {};
	virtual void OnPop() {};
	virtual void Update(UI::View viewbox, float time, float deltaTime) {};
	virtual void Draw(UI::View viewbox, UI::Renderer* rd) {};
	virtual std::string GetName() { return "noname"; };

	// virtual void OnEvent(EventBase* event) {};
};

typedef std::shared_ptr<IMenuState> IMenuStatePtr;
