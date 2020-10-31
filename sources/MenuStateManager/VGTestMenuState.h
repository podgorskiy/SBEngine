#pragma once
#include "BaseMenuState.h"
#include <string>


namespace UI
{
	class Block;

	typedef std::shared_ptr<Block> BlockPtr;
}

class VGTestMenuState: public BaseMenuState
{
public:
	VGTestMenuState(const fsal::File& yaml, MenuStateManager* msm);
	~VGTestMenuState();

	void Update(Render::View viewbox, float time, float deltaTime) override;
	void Draw(Render::View viewbox, Render::Renderer2D* rd) override;

protected:
};

typedef std::shared_ptr<IMenuState> IMenuStatePtr;
