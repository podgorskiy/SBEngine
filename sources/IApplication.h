#pragma once
#include "InputEventReciever.h"
#include <glm/glm.hpp>


struct GLFWwindow;

class IApplication: public InputEventReciever
{
public:
	virtual ~IApplication() = default;

	virtual void Update(float time, float deltaTime) = 0;
	virtual void OnWindowClose() = 0;
	virtual void SetMouse(float x, float y, bool mouseDown) {}
	virtual void SetDPI(glm::ivec2 dpi) {};
};
