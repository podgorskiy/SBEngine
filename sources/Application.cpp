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


#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "Audio/Audio.h"
#include "TTS/TTS.h"



Application::Application(int argc, const char* const* argv)
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

	m_program = Render::MakeProgram("vs_unlit.bin", "fs_unlit.bin");

	u_texture = m_program->GetUniform("u_texture");

	m_obj.Load("LeePerrySmith.obj");

	bgfx::setViewName(ViewIds::Main, "Main");
	bgfx::setViewName(ViewIds::GUI, "GUI");
	bgfx::setViewMode(ViewIds::GUI, bgfx::ViewMode::Sequential);

	m_menu_manager.reset(new MenuStateManager);
	m_menu_manager->Push(config["startup_state"].as<std::string>());


//	m_dr.Init();
//
//	{
//		Render::debug_guard<> debug_lock;
//	}
	m_uir.Init();
	m_uir.m_gamma_correction = config["srgb"].as<bool>();

//	TTS tts;
//	tts.LoadDict();
//	tts.SetText("")
//	tts.TextToPhonems();
////	tts.TestPhonems();
//	tts.PhonemsToPCM();
//	tts.SaveToWav("");
//	fflush(stdout);
}


Application::~Application()
{
}


void Application::Update(float time, float deltaTime)
{
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


//	bgfx::setTexture(0, u_texture.m_handle,  m_texture->m_handle);
//
//	uint64_t state = 0
//				| BGFX_STATE_DEFAULT
//				;
//	bgfx::setState(state);

	// m_obj.Draw(ViewIds::Main, m_program);

	{
		UI::View view_box(m_windowBufferSize, 72);
		m_menu_manager->Update(view_box, time, deltaTime);
		m_menu_manager->Draw(view_box, &m_uir, time, deltaTime);
//
//		using namespace UI::lit;
//		using UI::operator""_c;
////
//		root = UI::make_block({0_l, 100_wpe, 0_t, 100_hpe}, 0xFFFFFFFF_c);
//		auto stage = UI::make_block({130_wvh, 130_hvh, 50_clpe, 60_ctpe});
//		root->AddChild(stage);
//		auto room = UI::make_block({100_wpe, 100_hpe, 5_tvh, 0_l}, 0x00FF00FF_c);
//		stage->AddChild(room);

//		auto tile1a = UI::make_block({20_wpe, 40_hpe, 30_tpe, 20_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2a = UI::make_block({20_wpe, 40_hpe, 25_tpe, 30_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3a = UI::make_block({20_wpe, 40_hpe, 20_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto tile1b = UI::make_block({20_wpe, 40_hpe, 35_tpe, 30_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2b = UI::make_block({20_wpe, 40_hpe, 30_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3b = UI::make_block({20_wpe, 40_hpe, 25_tpe, 50_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto tile1c = UI::make_block({20_wpe, 40_hpe, 40_tpe, 40_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile2c = UI::make_block({20_wpe, 40_hpe, 35_tpe, 50_lpe}, m_texture, S::Contain, P::leftCenter);
//		auto tile3c = UI::make_block({20_wpe, 40_hpe, 30_tpe, 60_lpe}, m_texture, S::Contain, P::leftCenter);
//
//		auto wall1a = UI::make_block({20_wpe, 40_hpe, 18_tpe, 10_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//		auto wall2a = UI::make_block({20_wpe, 40_hpe, 13_tpe, 20_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//		auto wall3a = UI::make_block({20_wpe, 40_hpe,  8_tpe, 30_lpe}, m_texturew, S::Contain, P::leftCenter, T::FlipX);
//
//		auto wall1b = UI::make_block({20_wpe, 40_hpe,  8_tpe, 50_lpe}, m_texturew, S::Contain, P::leftCenter);
//		auto wall2b = UI::make_block({20_wpe, 40_hpe, 13_tpe, 60_lpe}, m_texturew, S::Contain, P::leftCenter);
//		auto wall3b = UI::make_block({20_wpe, 40_hpe, 18_tpe, 70_lpe}, m_texturew, S::Contain, P::leftCenter);

//		room->AddChild(wall3a);
//		room->AddChild(wall2a);
//		room->AddChild(wall1a);
//
//		room->AddChild(wall1b);
//		room->AddChild(wall2b);
//		room->AddChild(wall3b);
//
//		room->AddChild(tile3a);
//		room->AddChild(tile2a);
//		room->AddChild(tile1a);
//
//		room->AddChild(tile3b);
//		room->AddChild(tile2b);
//		room->AddChild(tile1b);
//
//		room->AddChild(tile3c);
//		room->AddChild(tile2c);
//		room->AddChild(tile1c);

		bool trigger = !mouse_left_click && mouse_left_click_prev;
		//UI::DoLayout(root, view_box);

//		UI::Editor(root, view_box);

//		std::vector<UI::Events> event_stack;
//		UI::Action(root, view_box, mouse_pos, trigger, mouse_left_click);



		//UI::Render(&m_uir, root, view_box);
	}

	bgfx::frame();
}


void Application::OnKey(int key, char asci, int action, int mods)
{
	if (action == 0)
	{
		// keyboard.ListenOnKeyDown(asci);
	}
	else if (action == 1)
	{
		// keyboard.ListenOnKeyUp(asci);
		if (asci == 'R')
		{
            m_menu_manager->Reload();
		}
	}
}

void Application::OnMouseButton(int button, int action, int mods)
{

}

void Application::OnMouseMove(glm::vec2 pos)
{

}

void Application::OnWindowResize(glm::ivec2 size)
{
	m_windowBufferSize = size;
	bgfx::reset(m_windowBufferSize.x, m_windowBufferSize.y, m_reset_flags);
}

namespace ApplicationFactory
{
	std::shared_ptr<IApplication> NewApplication(int argc, const char* const*  argv)
	{
		return std::shared_ptr<IApplication>(new Application(argc, argv));
	}
}
