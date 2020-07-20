#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

class IApplication
{
public:
	virtual ~IApplication() = default;

	virtual void Update(float time, float deltaTime) = 0;
	virtual void OnWindowResize(glm::ivec2 size) {}
	virtual void OnKey(int key, char asci, int action, int mods) {}
	virtual void OnSetChar(unsigned int c) {}
	virtual void OnMouseButton(int button, int action, int mods) {}
	virtual void OnMouseMove(glm::vec2 pos) {}
	virtual void OnScroll(glm::vec2 offset) {}
	virtual void OnIconification(int iconified) {}
	virtual void OnWindowClose() = 0;
	virtual void SetMouse(float x, float y, bool mouseDown) {}
	virtual void SetDPI(glm::ivec2 dpi) {};
};
