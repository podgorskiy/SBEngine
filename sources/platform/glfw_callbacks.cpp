#include "glfw_callbacks.h"
#include "IApplication.h"
#include "utils/common.h"
//#include "core/EventDispatcher.h"

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

#include <stdio.h>


static IApplication* GetApp(GLFWwindow* window)
{
	return static_cast<IApplication*>(glfwGetWindowUserPointer(window));
}


void io::ErrorCallback(int error, const char* description)
{
	spdlog::error("Error: {}{}", error, description);
	throw utils::runtime_error("Error: %d, %s", error, description);
}

void io::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	auto* app = GetApp(window);
	app->OnWindowResize(glm::ivec2(width, height));
}

void io::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto* app = GetApp(window);
	app->OnKey(key, scancode, action, mods);
}

void io::CharCallback(GLFWwindow* window, unsigned int c)
{
	auto* app = GetApp(window);
	app->OnSetChar(c);
}

void io::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	//GetEventDispatcher().Dispatch("mouse_but#2"_hash, action == GLFW_PRESS);
}

void io::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	auto* app = GetApp(window);

//	if (!ImGui::IsAnyWindowHovered())
//	{
//		app->m_camera.Scroll(float(-yoffset));
//		glm::mat3 c2w = app->m_camera.GetCanvasToWorld();
//		glm::vec2 size(100);
//		glm::vec3 sizeW = c2w * glm::vec3(size, 0.0);
//		sizeW.y = -sizeW.y;
//	}
//	else
//	{
//		ImGuiIO& io = ImGui::GetIO();
//		io.MouseWheel += (float)yoffset * 2.0f;
//	}
}


//void io::MouseButtonCallback(GLFWwindow*, int button, int action, int mods)
//{
//	SB::BasicEvents::ButtonType buttonType = SB::BasicEvents::MOUSE_BUTTON_NONE;
//	SB::BasicEvents::ActionType actionType = SB::BasicEvents::ACTION_NONE;
//	switch (button)
//	{
//	case GLFW_MOUSE_BUTTON_LEFT:
//		buttonType = SB::BasicEvents::MOUSE_BUTTON_LEFT;
//		break;
//	case GLFW_MOUSE_BUTTON_RIGHT:
//		buttonType = SB::BasicEvents::MOUSE_BUTTON_RIGHT;
//		break;
//	case GLFW_MOUSE_BUTTON_MIDDLE:
//		buttonType = SB::BasicEvents::MOUSE_BUTTON_MIDDLE;
//		break;
//	}
//	switch (action)
//	{
//	case GLFW_PRESS:
//		actionType = SB::BasicEvents::ACTION_PRESS;
//		break;
//	case GLFW_RELEASE:
//		actionType = SB::BasicEvents::ACTION_RELEASE;
//		break;
//	}
//	SB::BasicEvents::OnMouseButtonEvent buttonEvent;
//	buttonEvent.button = buttonType;
//	buttonEvent.action = actionType;
//	app->OnMousePressed(buttonEvent);
//}
//
////Handles mouse movement and creates a move event. Calls OnMouseMove with new event
//void io::MouseMoveCallback(GLFWwindow*, double x, double y)
//{
//	SB::BasicEvents::OnMouseMoveEvent moveEvent;
//	moveEvent.x = static_cast<float>(x);
//	moveEvent.y = static_cast<float>(y);
//	app->OnMouseMove(moveEvent);
//}
//
////Handles mouse wheel scrolling / turning
//void io::ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset)
//{
//	SB::BasicEvents::OnScrollEvent scrollEvent;
//	scrollEvent.x = static_cast<float>(yoffset);
//	app->OnScrollEvent(scrollEvent);
//}
//
////Handles keyboard button presses / releases, creates a key event and calls OnKeyPressed with the new event.
//void io::KeyCallback(GLFWwindow*, int key, int, int action, int /*mods*/)
//{
//	SB::BasicEvents::OnKeyEvent keyEvent;
//	switch (action)
//	{
//	case GLFW_PRESS:
//		keyEvent.action = SB::BasicEvents::ACTION_PRESS;
//		break;
//	case GLFW_RELEASE:
//		keyEvent.action = SB::BasicEvents::ACTION_RELEASE;
//		break;
//	case GLFW_REPEAT:
//		keyEvent.action = SB::BasicEvents::ACTION_REPEAT;
//		break;
//	}
//	switch (key)
//	{
//	case GLFW_KEY_BACKSPACE:
//		keyEvent.key = SB::BasicEvents::KEY_BACKSPACE;
//		break;
//	case GLFW_KEY_G:
//		keyEvent.key = SB::BasicEvents::KEY_G;
//		break;
//	case GLFW_KEY_W:
//		keyEvent.key = SB::BasicEvents::KEY_W;
//		break;
//	case GLFW_KEY_A:
//		keyEvent.key = SB::BasicEvents::KEY_A;
//		break;
//	case GLFW_KEY_S:
//		keyEvent.key = SB::BasicEvents::KEY_S;
//		break;
//	case GLFW_KEY_D:
//		keyEvent.key = SB::BasicEvents::KEY_D;
//		break;
//	case GLFW_KEY_B:
//		keyEvent.key = SB::BasicEvents::KEY_B;
//		break;
//	case GLFW_KEY_C:
//		keyEvent.key = SB::BasicEvents::KEY_C;
//		break;
//	case GLFW_KEY_E:
//		keyEvent.key = SB::BasicEvents::KEY_E;
//		break;
//	case GLFW_KEY_F:
//		keyEvent.key = SB::BasicEvents::KEY_F;
//		break;
//	case GLFW_KEY_H:
//		keyEvent.key = SB::BasicEvents::KEY_H;
//		break;
//	case GLFW_KEY_J:
//		keyEvent.key = SB::BasicEvents::KEY_J;
//		break;
//	case GLFW_KEY_K:
//		keyEvent.key = SB::BasicEvents::KEY_K;
//		break;
//	case GLFW_KEY_L:
//		keyEvent.key = SB::BasicEvents::KEY_L;
//		break;
//	case GLFW_KEY_M:
//		keyEvent.key = SB::BasicEvents::KEY_M;
//		break;
//	case GLFW_KEY_N:
//		keyEvent.key = SB::BasicEvents::KEY_N;
//		break;
//	case GLFW_KEY_O:
//		keyEvent.key = SB::BasicEvents::KEY_O;
//		break;
//	case GLFW_KEY_P:
//		keyEvent.key = SB::BasicEvents::KEY_P;
//		break;
//	case GLFW_KEY_R:
//		keyEvent.key = SB::BasicEvents::KEY_R;
//		break;
//	case GLFW_KEY_T:
//		keyEvent.key = SB::BasicEvents::KEY_T;
//		break;
//	case GLFW_KEY_Q:
//		keyEvent.key = SB::BasicEvents::KEY_Q;
//		break;
//	case GLFW_KEY_Z:
//		keyEvent.key = SB::BasicEvents::KEY_Z;
//		break;
//	case GLFW_KEY_X:
//		keyEvent.key = SB::BasicEvents::KEY_X;
//		break;
//	case GLFW_KEY_V:
//		keyEvent.key = SB::BasicEvents::KEY_V;
//		break;
//	case GLFW_KEY_Y:
//		keyEvent.key = SB::BasicEvents::KEY_Y;
//		break;
//	case GLFW_KEY_U:
//		keyEvent.key = SB::BasicEvents::KEY_U;
//		break;
//	case GLFW_KEY_I:
//		keyEvent.key = SB::BasicEvents::KEY_I;
//		break;
//	case GLFW_KEY_SPACE:
//		keyEvent.key = SB::BasicEvents::KEY_SPACE;
//		break;
//	case GLFW_KEY_F1:
//		keyEvent.key = SB::BasicEvents::KEY_F1;
//		break;
//	case GLFW_KEY_F2:
//		keyEvent.key = SB::BasicEvents::KEY_F2;
//		break;
//	case GLFW_KEY_F3:
//		keyEvent.key = SB::BasicEvents::KEY_F3;
//		break;
//	case GLFW_KEY_F4:
//		keyEvent.key = SB::BasicEvents::KEY_F4;
//		break;
//	case GLFW_KEY_F5:
//		keyEvent.key = SB::BasicEvents::KEY_F5;
//		break;
//	case GLFW_KEY_F6:
//		keyEvent.key = SB::BasicEvents::KEY_F6;
//		break;
//	case GLFW_KEY_F7:
//		keyEvent.key = SB::BasicEvents::KEY_F7;
//		break;
//	case GLFW_KEY_F8:
//		keyEvent.key = SB::BasicEvents::KEY_F8;
//		break;
//	case GLFW_KEY_F9:
//		keyEvent.key = SB::BasicEvents::KEY_F9;
//		break;
//	case GLFW_KEY_F10:
//		keyEvent.key = SB::BasicEvents::KEY_F10;
//		break;
//	case GLFW_KEY_F11:
//		keyEvent.key = SB::BasicEvents::KEY_F11;
//		break;
//	case GLFW_KEY_F12:
//		keyEvent.key = SB::BasicEvents::KEY_F12;
//		break;
//	case GLFW_KEY_LEFT:
//		keyEvent.key = SB::BasicEvents::KEY_LEFT;
//		break;
//	case GLFW_KEY_RIGHT:
//		keyEvent.key = SB::BasicEvents::KEY_RIGHT;
//		break;
//	case GLFW_KEY_UP:
//		keyEvent.key = SB::BasicEvents::KEY_UP;
//		break;
//	case GLFW_KEY_DOWN:
//		keyEvent.key = SB::BasicEvents::KEY_DOWN;
//		break;
//	case GLFW_KEY_COMMA:
//		keyEvent.key = SB::BasicEvents::KEY_COMMA;
//		break;
//	case GLFW_KEY_PERIOD:
//		keyEvent.key = SB::BasicEvents::KEY_PERIOD;
//		break;
//	case GLFW_KEY_LEFT_CONTROL:
//		keyEvent.key = SB::BasicEvents::KEY_LEFT_CONTROL;
//		break;
//	case GLFW_KEY_RIGHT_CONTROL:
//		keyEvent.key = SB::BasicEvents::KEY_RIGHT_CONTROL;
//		break;
//	case GLFW_KEY_GRAVE_ACCENT:
//		keyEvent.key = SB::BasicEvents::KEY_GRAVE_ACCENT;
//		break;
//	case GLFW_KEY_PAGE_UP:
//		keyEvent.key = SB::BasicEvents::KEY_PAGE_UP;
//		break;
//	case GLFW_KEY_PAGE_DOWN:
//		keyEvent.key = SB::BasicEvents::KEY_PAGE_DOWN;
//		break;
//	case GLFW_KEY_KP_1:
//		keyEvent.key = SB::BasicEvents::KEY_KP_1;
//		break;
//	case GLFW_KEY_KP_2:
//		keyEvent.key = SB::BasicEvents::KEY_KP_2;
//		break;
//	case GLFW_KEY_KP_3:
//		keyEvent.key = SB::BasicEvents::KEY_KP_3;
//		break;
//	case GLFW_KEY_KP_4:
//		keyEvent.key = SB::BasicEvents::KEY_KP_4;
//		break;
//	case GLFW_KEY_KP_5:
//		keyEvent.key = SB::BasicEvents::KEY_KP_5;
//		break;
//	case GLFW_KEY_KP_6:
//		keyEvent.key = SB::BasicEvents::KEY_KP_6;
//		break;
//	case GLFW_KEY_KP_7:
//		keyEvent.key = SB::BasicEvents::KEY_KP_7;
//		break;
//	case GLFW_KEY_KP_8:
//		keyEvent.key = SB::BasicEvents::KEY_KP_8;
//		break;
//	case GLFW_KEY_KP_9:
//		keyEvent.key = SB::BasicEvents::KEY_KP_9;
//		break;
//
//	default:
//		return;
//	}
//	keyEvent.rawKey = key;
//	app->OnKeyPressed(keyEvent);
//}
//
//void io::CharCallback(GLFWwindow*, unsigned int c)
//{
//	SB::BasicEvents::OnCharEvent charevent;
//	charevent.c = c;
//	app->OnCharEvent(charevent);
//}
//
//void io::SetApplication(Application* application)
//{
//	app = application;
//}
//
//void io::SetJoystickManager(JoystickManager* jmgr)
//{
//	joystick = jmgr;
//}
//
//void io::IconificationCallback(GLFWwindow* window, int iconified)
//{
//	app->OnIconifyEvent(iconified == GLFW_TRUE);
//}
//
//void io::WindowCloseCallback(GLFWwindow* window)
//{
//	app->CloseOpenedProject();
//}
