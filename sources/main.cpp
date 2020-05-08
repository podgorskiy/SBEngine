#include "Application.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

auto start = std::chrono::steady_clock::now();


#ifdef __EMSCRIPTEN__
std::shared_ptr<Application> g_app;

EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* event, void* userData)
{
	ImGuiIO& io = ImGui::GetIO();
	if (event == NULL)
	{
		return false;
	}

	switch (eventType)
	{
	case EMSCRIPTEN_EVENT_MOUSEMOVE:
	{
		io.MousePos.x = event->canvasX;
		io.MousePos.y = event->canvasY;
		return true;
	}
	case EMSCRIPTEN_EVENT_MOUSEDOWN:
		io.MouseDown[0] = true;
		return true;
	case EMSCRIPTEN_EVENT_MOUSEUP:
		io.MouseDown[0] = false;
		return true;
	}
	return false;
}
#endif


void Update(void* window)
{
	auto app = static_cast<Application*>(glfwGetWindowUserPointer((GLFWwindow*)window));

	auto current_timestamp = std::chrono::steady_clock::now();

	std::chrono::duration<float> elapsed_time = (current_timestamp - start);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	app->Draw(elapsed_time.count());

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#ifndef __EMSCRIPTEN__
	glfwSwapInterval(1);
#endif

	/* Swap front and back buffers */
	glfwSwapBuffers((GLFWwindow*)window);

	/* Poll for and process events */
	glfwPollEvents();
}


int main(int argc, const char* const* argv)
{
	GLFWwindow* window;

	/* Initialize the library */
	glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1200, 900, "Hello World", nullptr, nullptr);

	/* Make the window's context current */
#ifndef __EMSCRIPTEN__
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
#endif

    ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, false);

	{
		std::shared_ptr<Application> app = std::make_shared<Application>(argc, argv);

	    const char* glsl_version = "#version 300 es";
	    ImGui_ImplOpenGL3_Init(glsl_version);

		app->Resize(640, 640);
		
		glfwSetWindowUserPointer(window, app.get());

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
			app->Resize(width, height);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int mods)
		{
			Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
			if (ImGui::IsAnyItemHovered())
			{
				ImGuiIO& io = ImGui::GetIO();
				if (action == GLFW_PRESS)
					io.KeysDown[key] = true;
				if (action == GLFW_RELEASE)
					io.KeysDown[key] = false;

				io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
				io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
				io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
				io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
			}
			else
			{
				char asci = 0;
				switch(key)
				{
					case GLFW_KEY_UP:
						asci = 38;
						break;
					case GLFW_KEY_DOWN:
						asci = 40;
						break;
					case GLFW_KEY_LEFT:
						asci = 37;
						break;
					case GLFW_KEY_RIGHT:
						asci = 39;
					default:
						break;
				}
				if (key == 32
					|| key == 39
					|| (key >= 44 && key <= 57)
					|| key == 59
					|| key == 61
					|| (key >= 65 && key <= 93)
					|| key == 96)
				{
					asci = key;
				}
				if (key == GLFW_KEY_ESCAPE)
				{
					asci = 27;
				}

				app->OnKeyAction(key, asci, action, mods);
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow*, unsigned int c)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharacter((unsigned short)c);
		});

		glfwSetScrollCallback(window, [](GLFWwindow*, double /*xoffset*/, double yoffset)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseWheel += (float)yoffset * 2.0f;
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int /*mods*/)
		{
			ImGuiIO& io = ImGui::GetIO();

			if (button >= 0 && button < 3)
			{
				io.MouseDown[button] = action == GLFW_PRESS;
			}
		});

		auto start = std::chrono::steady_clock::now();

#ifndef __EMSCRIPTEN__
		while (!glfwWindowShouldClose(window))
		{
			Update(window);
		}
#else
		g_app = app;
		emscripten_set_mousedown_callback("#canvas", (void*)window, true, mouseCb);
		emscripten_set_mouseup_callback("#canvas", (void*)window, true, mouseCb);
		emscripten_set_mousemove_callback("#canvas", (void*)window, true, mouseCb);

		emscripten_set_main_loop_arg(Update, (void*)window, 0, 0);
#endif

#ifndef __EMSCRIPTEN__
		glfwSetWindowSizeCallback(window, nullptr);
		ImGui_ImplGlfw_Shutdown();
		glfwTerminate();
#endif
	}

	return 0;
}
