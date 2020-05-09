#pragma once

class Application;
struct GLFWwindow;

namespace io
{
	void ErrorCallback(int error, const char* description);

	void WindowSizeCallback(GLFWwindow* window, int width, int height);

	void KeyCallback(GLFWwindow*, int key, int scancode, int action, int mods);

	void CharCallback(GLFWwindow*, unsigned int c);

	void MouseButtonCallback(GLFWwindow*, int button, int action, int mods);

	void MouseMoveCallback(GLFWwindow*, double x, double y);

	void ScrollCallback(GLFWwindow*, double xoffset, double yoffset);

	void IconificationCallback(GLFWwindow* window, int iconified);

	void WindowCloseCallback(GLFWwindow* window);
}
