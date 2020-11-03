#pragma once
#include "Block.h"

namespace UI
{
	void Render(Render::Renderer2D* renderer, const BlockPtr& root, Render::View view, float time = 0.0f, int flags = 0);

	void DoLayout(const BlockPtr& block, const Render::View& view, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click);
}
