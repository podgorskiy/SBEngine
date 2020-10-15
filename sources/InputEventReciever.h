#pragma once
#include <glm/glm.hpp>

class InputEventReciever
{
public:
	virtual ~InputEventReciever() = default;

	virtual void OnWindowResize(glm::ivec2 size)
	{}

	virtual void OnKey(int key, char asci, int action, int mods)
	{}

	virtual void OnSetChar(unsigned int c)
	{}

	virtual void OnMouseButton(int button, int action, int mods)
	{}

	virtual void OnMouseMove(glm::vec2 pos)
	{}

	virtual void OnScroll(glm::vec2 offset)
	{}

	virtual void OnIconification(int iconified)
	{}
};
