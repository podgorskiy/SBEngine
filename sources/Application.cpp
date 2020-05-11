#include "Application.h"
#include "Render/gl_headers.h"
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
#include "Render/GLDebugMessage.h"
#include "UI/Block.h"
#include "utils/assertion.h"
#include "utils/align.h"
#include "utils/system_info.h"
#include "utils/aabb.h"
#include "misc/doc_test_runner.h"
#include <stb_image_write.h>
#include <bx/bx.h>
#include <bx/math.h>


#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
//#include <Tile.h>
//#include <Wall.h>
//#include <Oquonie.h>
//#include <World.h>
//#include <Keyboard.h>
//#include <Music.h>
//#include "Audio.h"


//Keyboard keyboard;

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
		, config["srgb"].as<bool>() ? 0x727272ff : 0xb2b2b2ff
		, 1.0f
		, 0
	);

	bgfx::setViewMode(0, bgfx::ViewMode::Sequential);

	m_program = Render::MakeProgram("vs_unlit.bin", "fs_unlit.bin");

	u_texture = m_program->GetUniform("u_texture");

	m_obj.Load("LeePerrySmith.obj");
	m_texture = Render::LoadTexture("test.png");


//	m_dr.Init();
//	m_uir.Init();
//
//	{
//		Render::debug_guard<> debug_lock;
//	}
	m_uir.Init();
}


Application::~Application()
{
}


void Application::Update(float time, float deltaTime)
{
	bgfx::setViewRect(0, 0, 0, uint16_t(m_windowBufferSize.x), uint16_t(m_windowBufferSize.y));
	bgfx::touch(0);
	bgfx::setViewFrameBuffer(3, BGFX_INVALID_HANDLE);
	bgfx::setViewRect(3, 0, 0, uint16_t(m_windowBufferSize.x), uint16_t(m_windowBufferSize.y));
	bgfx::touch(3);

	float aspect = m_windowBufferSize.x / (float)m_windowBufferSize.y;

	float view_height = 2.2f;
	float view_width = aspect * view_height;

	float camera_rotation = -time * 0.1;

	glm::mat4 projection = glm::perspectiveFov(1.0f, view_width, view_height, 0.01f, 1000.0f);
	glm::vec3 camera_pos = glm::vec3(1.0, 0.5, 1.0) * 0.3f;
	camera_pos = glm::rotate(glm::mat4(1.0), camera_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos, 1.0);
	glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 model = glm::mat4(1.0f);

	bgfx::setViewTransform(0, &view[0], &projection[0]);
	bgfx::setTransform(&model[0]);

	bgfx::setTexture(0, u_texture.m_handle,  m_texture->m_handle);

	uint64_t state = 0
				| BGFX_STATE_DEFAULT
				;
	bgfx::setState(state);

	m_obj.Draw(0, m_program);

//	// Oquonie::GetInstance()->m_music->m_audio.Update();
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glViewport(0, 0, m_width, m_height);

	{
		glm::aabb2 view_box(glm::vec2(0.0), m_windowBufferSize);

		UI::View view;
		view.size_in_dots = view_box.size();
		view.dpi = 72;

		using namespace UI::lit;

		auto root_block = UI::make_block(30_lpe, 200_w, 50_t, 50_bpe);
		auto block2 = UI::make_block(30_lpe, 200_w, 50_t, 50_bpe);
		root_block->AddChild(block2);

		UI::DoLayout(root_block, view);
		m_uir.SetProjection(view_box);

		auto prj =glm::ortho(view_box.minp.x, view_box.maxp.x, view_box.maxp.y, view_box.minp.y);

		UI::Traverse(root_block, nullptr, [prj, this](UI::Block* block, UI::Block* parent)
		{
			//Render::DrawRect(m_dr, box.minp, box.maxp, prj);
			auto box = block->GetBox();
			m_uir.Rect(box);
			m_uir.Text(box, "ПОМОГИТЕ ПОЖАЛУЙСТАHello!!!\n\nauto shadowPaint = nvgBoxGradient(vg, glm::aabb2(box.minp + glm::vec2(0.0f, 2.0f), box.maxp + glm::vec2(0.0f, 2.0f)), cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));");
//			float cornerRadius = 3.0f;
//
//			auto box = block->GetBox();
//
//			nvgBeginPath(vg);
//			nvgRoundedRect(vg, box, cornerRadius);
//			nvgFillColor(vg, nvgRGBA(28,30,34,192));
//			// nvgFillColor(vg, nvgRGBA(255,192,0,255));
//			nvgFill(vg);
//
//			// Drop shadow
//			auto shadowPaint = nvgBoxGradient(vg, glm::aabb2(box.minp + glm::vec2(0.0f, 2.0f), box.maxp + glm::vec2(0.0f, 2.0f)), cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
//			nvgBeginPath(vg);
//			nvgRect(vg, glm::aabb2(box.minp - glm::vec2(10.0f), box.maxp + glm::vec2(10.0f, 20.0f)));
//			nvgRoundedRect(vg, box, cornerRadius);
//			nvgPathWinding(vg, NVG_HOLE);
//			nvgFillPaint(vg, shadowPaint);
//			nvgFill(vg);
		});

		m_uir.Draw();

////
////		ImGui::Begin("Glyph cache");
////		ImGui::Image((ImTextureID)m_uir.GetGlyphTexture(), ImVec2(1024, 1024));
////#ifndef __EMSCRIPTEN__
////		if (ImGui::Button("Save to file"))
////		{
////			GLint current_texture;
////			glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);
////			glBindTexture(GL_TEXTURE_2D, m_uir.GetGlyphTexture());
////			int texDims[2];
////			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texDims[0]);
////			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texDims[1]);
////
////			static std::vector<uint8_t> data;
////			data.resize(misc::align(texDims[0] * 3, 4) * texDims[1]);
////			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, &data[0]);
////
////			stbi_write_png("glyph_cache.png", texDims[0], texDims[1], 3, data.data(), misc::align(texDims[0] * 3, 4));
////		}
////#endif
////		ImGui::End();
	}

	bgfx::frame();
}


void Application::OnKeyAction(int key, char asci, int action, int mods)
{
//	if (action == 0)
//	{
//		keyboard.ListenOnKeyDown(asci);
//	}
//	else if (action == 1)
//	{
//		keyboard.ListenOnKeyUp(asci);
//	}
}

void Application::OnWindowResize(glm::ivec2 size)
{
	m_windowBufferSize = size;
	bgfx::reset(m_windowBufferSize.x, m_windowBufferSize.y, m_reset_flags);
}

namespace ApplicationFactory
{
	std::shared_ptr<IApplication> NewApplication(int argc, char **argv)
	{
		return std::shared_ptr<IApplication>(new Application(argc, argv));
	}
}
