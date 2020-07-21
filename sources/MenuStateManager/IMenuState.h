#pragma once
#include "2DEngine/View.h"
#include "2DEngine/Renderer2D.h"
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
	virtual void Update(Render::View viewbox, float time, float deltaTime) {};
	virtual void Draw(Render::View viewbox, Render::Renderer2D* rd) {};
	virtual std::string GetName() { return "noname"; };

	// virtual void OnEvent(EventBase* event) {};
};

typedef std::shared_ptr<IMenuState> IMenuStatePtr;
