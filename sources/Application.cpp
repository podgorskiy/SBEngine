#include "Application.h"
#include <bgfx/bgfx.h>
#include <stdio.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Render/DebugRenderer.h"
#include "Render/TextureReaders/TextureLoader.h"
#include "UI/Block.h"
#include "utils/assertion.h"
#include "utils/align.h"
#include "utils/system_info.h"
#include "utils/aabb.h"
#include "misc/doc_test_runner.h"
#include <stb_image_write.h>
#include <bx/bx.h>
#include <bx/math.h>
#include "views.h"
#include "UI/UI_Load.h"
#include "EventIds.h"


#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "Audio/Audio.h"
#include "TTS/TTS.h"

#include <entt/entt.hpp>
#include <tuple>


struct position {
    float x;
    float y;
};
entt::registry registry;


Application::Application(int argc, const char* const* argv): m_eventDispatcher(core::GetEventDispatcher())
{
    spdlog::info("{}", utils::GetMemoryUsage());
    using fs = fsal::FileSystem;
	using loc = fsal::Location;
	spdlog::info("RoamingAppData: {}", fs::GetSystemPath(loc::kStorageSynced).string());
	spdlog::info("LocalAppData: {}", fs::GetSystemPath(loc::kStorageLocal).string());
	spdlog::info("UserPictures: {}", fs::GetSystemPath(loc::kPictures).string());
	spdlog::info("UserMusic: {}", fs::GetSystemPath(loc::kMusic).string());
	spdlog::info("ProgramData: {}", fs::GetSystemPath(loc::kWin_ProgramData).string());
	spdlog::info("ProgramFiles: {}", fs::GetSystemPath(loc::kWin_ProgramFiles).string());
	spdlog::info("Temp: {}", fs::GetSystemPath(loc::kTemp).string());

	YAML::Node config = YAML::Load(std::string(fs().Open("app_config.yaml")));

	bgfx::RendererType::Enum backend = bgfx::RendererType::Noop;
	auto backend_str = config["backend"].as<std::string>();
	if (backend_str == "OpenGLES")
		backend = bgfx::RendererType::OpenGLES;
	else if (backend_str == "Direct3D9")
		backend = bgfx::RendererType::Direct3D9;
	else if (backend_str == "Direct3D11")
		backend = bgfx::RendererType::Direct3D11;
	else if (backend_str == "Direct3D12")
		backend = bgfx::RendererType::Direct3D12;
	else if (backend_str == "Metal")
		backend = bgfx::RendererType::Metal;
	else if (backend_str == "OpenGL")
		backend = bgfx::RendererType::OpenGL;
	else if (backend_str == "Vulkan")
		backend = bgfx::RendererType::Vulkan;
	else
		ASSERT(false, "Unknown backend: %s", backend_str.c_str())

    spdlog::info("Using backend: {}", bgfx::getRendererName(backend));

	m_reset_flags = BGFX_RESET_NONE
		| (config["msaa16"].as<bool>() ? BGFX_RESET_MSAA_X16 : 0)
		| (config["flush_after_render"].as<bool>() ? BGFX_RESET_FLUSH_AFTER_RENDER : 0)
		| (config["vsync"].as<bool>() ? BGFX_RESET_VSYNC : 0)
		| (config["flip_after_render"].as<bool>() ? BGFX_RESET_FLIP_AFTER_RENDER : 0)
		| (config["srgb"].as<bool>() ? BGFX_RESET_SRGB_BACKBUFFER : 0)
		;

	bgfx::Init init;
	init.type     = backend;
	init.vendorId = 0;
	init.resolution.width  = 1200;
	init.resolution.height = 900;
	init.resolution.reset  = m_reset_flags;
	init.callback = &m_cp;
	init.debug = true;
	init.profile = true;
	bgfx::init(init);

	bgfx::setDebug(BGFX_DEBUG_NONE
		| (config["debug_text"].as<bool>() ? BGFX_DEBUG_TEXT : 0)
		| (config["debug_stats"].as<bool>() ? BGFX_DEBUG_STATS : 0)
	);

	if (config["run_tests"].as<bool>())
	{
		run_doc_tests(argc, argv);
	}

	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL
		, 0x000000ff
		, 1.0f
		, 0
	);

	bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

//	m_program = Render::MakeProgram("vs_unlit.bin", "fs_unlit.bin");
//
//	u_texture = m_program->GetUniform("u_texture");

//	m_obj.Load("LeePerrySmith.obj");

	bgfx::setViewName(ViewIds::Main, "Main");
	bgfx::setViewName(ViewIds::GUI, "GUI");
	bgfx::setViewMode(ViewIds::GUI, bgfx::ViewMode::Sequential);

	m_uir.Init();
	m_uir.m_gamma_correction = config["srgb"].as<bool>();

	auto shader_map = m_uir.GetShaderMap();
	auto tf_map = m_uir.GetTypefaceMap();

	m_menu_manager.reset(new MenuStateManager);
	m_menu_manager->InjectMaps(tf_map, shader_map);
	m_menu_manager->InjectEventDispatcher(&m_eventDispatcher);
	m_menu_manager->Push(config["startup_state"].as<std::string>());

    for(auto i = 0; i < 10; ++i)
    {
        auto entity = registry.create();
        registry.emplace<position>(entity, i * 1.f, i * 1.f);
    }

//	m_dr.Init();
//
//	{
//		Render::debug_guard<> debug_lock;
//	}
	Audio::AudioContext().Init();

//	m_audio_driver.PlayStream(Audio::Convert(
//			Audio::MakeAModulation(
//					Audio::MakeFModulatedSineWave(
//							Audio::MakeFModulatedSineWave(Audio::MakeSineWave(26.0f), 0.5, 126.0), 0.3f, 349.228f * 2
//					),
//					Audio::MakeEnvelope(0.01, 0.03, 0.3, 0.6, 0.03)
//			)
//	));

	// Audio::AudioContext().PlayFile("audio/ui/App Menu Button 6.mp3", true);
	// Audio::AudioContext().PlayFile("audio/Celtic_Nakarada.mp3", true);

	//Audio::AudioContext().PlayFile("audio/dialog/boss.ogg", true);
	m_userInput.InjectEventDispatcher(&m_eventDispatcher);
}


Application::~Application()
{
	Audio::AudioContext().Destroy();
}


void Application::Update(float time, float deltaTime)
{
	Audio::AudioContext().Update();

    auto v = registry.view<position>();

	// ImGui::SliderFloat();

	bgfx::setViewRect(ViewIds::Main, 0, 0, uint16_t(m_windowBufferSize.x), uint16_t(m_windowBufferSize.y));
	bgfx::touch(ViewIds::Main);
	bgfx::setViewFrameBuffer(ViewIds::GUI, BGFX_INVALID_HANDLE);
	bgfx::setViewRect(ViewIds::GUI, 0, 0, uint16_t(m_windowBufferSize.x), uint16_t(m_windowBufferSize.y));
	bgfx::touch(ViewIds::GUI);

	float aspect = m_windowBufferSize.x / (float)m_windowBufferSize.y;

	float view_height = 2.2f;
	float view_width = aspect * view_height;

	float camera_rotation = -time * 0.1;

	glm::mat4 projection = glm::perspectiveFov(1.0f, view_width, view_height, 0.01f, 1000.0f);
	glm::vec3 camera_pos = glm::vec3(1.0, 0.5, 1.0) * 0.3f;
	camera_pos = glm::rotate(glm::mat4(1.0), camera_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos, 1.0);
	glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 model = glm::mat4(1.0f);

	bgfx::setViewTransform(ViewIds::Main, &view[0], &projection[0]);
	bgfx::setTransform(&model[0]);


	Render::View view_box(m_windowBufferSize, 72);
	m_menu_manager->Update(view_box, time, deltaTime);
	m_menu_manager->Draw(view_box, &m_uir, time, deltaTime);

	bgfx::frame();
}


void Application::OnKey(int key, char asci, int action, int mods)
{
	if (action == 0)
	{
		m_eventDispatcher.Dispatch(Events::KeyDown, std::pair<int, char>(key, asci));
	}
	else if (action == 1)
	{
		m_eventDispatcher.Dispatch(Events::KeyUp, std::pair<int, char>(key, asci));
		if (asci == 'R')
		{
            m_menu_manager->Reload();
		}
	}
}

void Application::OnMouseButton(int button, int action, int mods)
{
	if (action == 0)
	{
		m_eventDispatcher.Dispatch(Events::MouseButtonDown, button);
	}
	else if (action == 1)
	{
		m_eventDispatcher.Dispatch(Events::MouseButtonUp, button);
	}
	m_userInput.OnMouseButton(button, action, mods);
}

void Application::OnMouseMove(glm::vec2 pos)
{
	m_eventDispatcher.Dispatch(Events::MouseMove, pos);
	m_userInput.OnMouseMove(pos);
}

void Application::OnWindowResize(glm::ivec2 size)
{
	m_windowBufferSize = size;
	bgfx::reset(m_windowBufferSize.x, m_windowBufferSize.y, m_reset_flags);
	m_eventDispatcher.Dispatch(Events::WindowResize);
}

namespace ApplicationFactory
{
	std::shared_ptr<IApplication> NewApplication(int argc, const char* const*  argv)
	{
		return std::shared_ptr<IApplication>(new Application(argc, argv));
	}
}
