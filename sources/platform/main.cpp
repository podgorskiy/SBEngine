#include "Application.h"
#include "ApplicationFactory.h"
#include "glfw_callbacks.h"
#include "utils/common.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include <yaml-cpp/yaml.h>
#include "imgui/imgui.h"

#define NOMINMAX
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#elif __unix__
#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bgfx/platform.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

static auto start = std::chrono::steady_clock::now();
static auto last_timestep = std::chrono::steady_clock::now();


static void InstallCallbacks(GLFWwindow* window)
{
	glfwSetWindowSizeCallback(window, io::WindowSizeCallback);
	glfwSetKeyCallback(window, io::KeyCallback);
	glfwSetCharCallback(window, io::CharCallback);
	glfwSetScrollCallback(window, io::ScrollCallback);
	glfwSetMouseButtonCallback(window, io::MouseButtonCallback);

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io.ConfigFlags |= 0
			| ImGuiConfigFlags_NavEnableGamepad
			| ImGuiConfigFlags_NavEnableKeyboard
			;
}


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

// static CustomPrinters cp;


void Update(void* window, glm::ivec2 m_windowBufferSize);


double scroll = 0.0f;


int main(int argc, const char* const* argv)
{
    spdlog::info("Command line args:");
	for (int i = 0; i < argc; ++i)
	{
        spdlog::info("{}", argv[i]);
	}

    spdlog::info("Working directory: {}", utils::get_cwd().c_str());

	glfwSetErrorCallback(io::ErrorCallback);

    spdlog::info("Compiled against GLFW {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);

	if (!glfwInit())
	{
		spdlog::error("Failed to initialize GLFW");
		std::exit(EXIT_FAILURE);
	}

	fsal::FileSystem fs;
	fs.PushSearchPath("resources");
	fs.PushSearchPath("../resources");

	YAML::Node config = YAML::Load(std::string(fs.Open("glfw_config.yaml")));

	if (config["depth_bits"])
		glfwWindowHint(GLFW_DEPTH_BITS, config["depth_bits"].as<int>(24));

	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);

	if (config["srgb"].as<bool>())
		glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

	if (config["double_buffer"].as<bool>())
		glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window;
	GLFWmonitor* monitor;
	glm::ivec2 m_windowBufferSize;
	int m_scale;
	glm::vec2 m_dpi;

	m_windowBufferSize.x = config["width"].as<int>(1200);
	m_windowBufferSize.y = config["height"].as<int>(900);

	monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glm::ivec2 monitorSizeInmm;
	glfwGetMonitorPhysicalSize(monitor, &monitorSizeInmm.x, &monitorSizeInmm.y);
	glm::ivec2 monitorSizeInpixels(mode->width, mode->height);

    spdlog::info("Monitor Physical Size: {}x{}mm.", monitorSizeInmm.x, monitorSizeInmm.y);

	m_dpi = glm::round(glm::vec2(monitorSizeInpixels) / glm::vec2(monitorSizeInmm) * utils::k_mm_per_inch);

    spdlog::info("DPI: {}x{}", m_dpi.x, m_dpi.y);

	constexpr float standardDPI = 96.0f;
	m_scale = (int)glm::round(m_dpi / standardDPI).x;
	m_scale = m_scale == 0 ? 1 : m_scale;
    spdlog::info("Selected scaling: {}", m_scale);

	//m_scale = 1.0;
	m_windowBufferSize = glm::vec2(m_windowBufferSize) * (float)m_scale;

	window = glfwCreateWindow(m_windowBufferSize.x, m_windowBufferSize.y, config["title"].as<std::string>("No title").c_str(), nullptr, nullptr);

	bgfx::PlatformData pd;
	bx::memSet(&pd, 0, sizeof(pd));
#ifdef _WIN32
//	HWND w = glfwGetWin32Window(window);
//	pd.nwh = w;
#elif __linux__
	auto w = glfwGetX11Window(window);
	pd.nwh = (void*)w;
	pd.ndt = glfwGetX11Display();
#endif

	bgfx::setPlatformData(pd);

	{
		std::shared_ptr<IApplication> app = ApplicationFactory::NewApplication(argc, argv);

		imguiCreate();

		glfwGetWindowSize(window, &m_windowBufferSize[0], &m_windowBufferSize[1]);
		app->OnWindowResize(m_windowBufferSize);

		glfwSetWindowUserPointer(window, app.get());
		InstallCallbacks(window);

		start = std::chrono::steady_clock::now();

#ifndef __EMSCRIPTEN__
		while (!glfwWindowShouldClose(window))
		{
			Update(window, m_windowBufferSize);
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
		imguiDestroy();
		glfwTerminate();
#endif
	}

	return 0;
}


void Update(void* window, glm::ivec2 m_windowBufferSize)
{
	auto current_timestamp = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsed_time = (current_timestamp - start);
	std::chrono::duration<float> delta_time = (current_timestamp - last_timestep);
	glfwPollEvents();

	auto app = static_cast<Application*>(glfwGetWindowUserPointer((GLFWwindow*)window));

	double x, y;
	glfwGetCursorPos((GLFWwindow*)window, &x, &y);

	app->SetMouse(x, y, glfwGetMouseButton((GLFWwindow*) window, 0) == GLFW_PRESS);

	imguiBeginFrame(x, y,
	                (glfwGetMouseButton((GLFWwindow*)window, 0) == GLFW_PRESS ? IMGUI_MBUT_LEFT : 0)
	                | (glfwGetMouseButton((GLFWwindow*)window, 1) == GLFW_PRESS ? IMGUI_MBUT_RIGHT : 0)
	                | (glfwGetMouseButton((GLFWwindow*)window, 2) == GLFW_PRESS ? IMGUI_MBUT_MIDDLE : 0), 0,
	                uint16_t(m_windowBufferSize.x), uint16_t(m_windowBufferSize.y)
	);

	ImGui::ShowDemoWindow();

	app->Update(elapsed_time.count(), delta_time.count());

	imguiEndFrame();

	last_timestep = current_timestamp;
}
