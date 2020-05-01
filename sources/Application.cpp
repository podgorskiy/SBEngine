#include "Application.h"
#include <GL/gl3w.h>
#include <stdio.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include "Render/DebugRenderer.h"
#include "Render/TextureReaders/TextureLoader.h"
#include "Render/GLDebugMessage.h"
#include "UI/Block.h"
#include "Vector/nanovg.h"
#include "Vector/nanovg_backend.h"
#include "Vector/demo.h"


#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include <Tile.h>
#include <Wall.h>
#include <Oquonie.h>
#include <World.h>
#include <Keyboard.h>
#include <Music.h>
#include "Audio.h"
#include "aabb.h"


Keyboard keyboard;
NVGcontext* vg = nullptr;


Application::Application(int argc, const char* const* argv)
{
	gl3wInit();

	{
		doctest::Context context;
		// defaults
		context.setOption("rand-seed", 1);
		context.setOption("order-by", "file");

		context.applyCommandLine(argc, argv);

		// overrides
		context.setOption("no-breaks", true); // don't break in the debugger when assertions fail

		int res = context.run(); // run queries, or run tests unless --no-run is specified

		if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
		{
			exit(res);
		}

		context.clearFilters(); // removes all filters added up to this point
	}

	fsal::FileSystem fs;
	fs.PushSearchPath("resources");
	fs.PushSearchPath("../resources");

	const char* OpenGLversion = (const char*)glGetString(GL_VERSION);
	const char* GLSLversion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	spdlog::info("OpenGL {} GLSL: {}", OpenGLversion, GLSLversion);


	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);


	const char* vertex_shader_src = R"(
		attribute vec3 a_position;
		attribute vec2 a_uv;

		uniform mat4 u_modelView;
		uniform mat4 u_model;
		uniform mat4 u_projection;

		varying vec2 v_uv;

		void main()
		{
			vec4 pos = u_modelView * vec4(a_position, 1.0);
			v_uv = a_uv;
			gl_Position = u_projection * pos;
		}
	)";

	const char* fragment_shader_src = R"(
		varying vec2 v_uv;

		uniform sampler2D u_texture;

		uniform mat4 u_modelView;

		void main()
		{
			vec3 color = texture2D(u_texture, v_uv).rgb;
			color = pow(color, vec3(2.2));
			gl_FragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
		}
	)";

	m_program = Render::MakeProgram(vertex_shader_src, fragment_shader_src);

	u_modelView = m_program->GetUniformLocation("u_modelView");
	u_projection = m_program->GetUniformLocation("u_projection");
	m_uniform_texture = m_program->GetUniformLocation("u_texture");

	m_obj.Load("LeePerrySmith.obj");
	m_obj.Collect(m_program);
	m_dr.Init();

	{
		Render::debug_guard<> debug_lock;
		m_texture = Render::LoadTexture("test.png");
	}

	Oquonie::GetInstance()->Install();
	Oquonie::GetInstance()->Start();

	vg = nvgCreateContext(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == nullptr)
	{
		spdlog::error("Error, Could not init nanovg.");
	}
}


Application::~Application()
{
}


void Application::Draw(float time)
{
	Render::debug_guard<> debug_lock;

	Oquonie::GetInstance()->m_music->m_audio.Update();

	ImGui::Begin("IntroductionToComputerGraphics");
	ImGui::Text("Face culling.");

	static bool cull = false;
	ImGui::Checkbox("Do culling", &cull);
	if (cull)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	static bool front = false;
	ImGui::Checkbox("Cull front/back", &front);
	if (front)
	{
		glCullFace(GL_FRONT);
	}
	else
	{
		glCullFace(GL_BACK);
	}

	ImGui::End();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_width, m_height);

	float aspect = m_width / (float)m_height;

	float view_height = 2.2f;
	float view_width = aspect * view_height;

	float camera_rotation = -time * 0.1;

	glm::mat4 projection = glm::perspectiveFov(1.0f, view_width, view_height, 0.01f, 100.0f);
	glm::vec3 camera_pos = glm::vec3(1.0, 0.5, 1.0) * 0.3f;
	camera_pos = glm::rotate(glm::mat4(1.0), camera_rotation, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camera_pos, 1.0);
	glm::mat4 view = glm::lookAt(camera_pos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 modelView = view * model;

	m_program->Use();
	m_texture->Bind(0);

	glUniformMatrix4fv(u_projection, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(u_modelView, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]);

	glUniform1i(m_uniform_texture, 0);

	m_obj.Bind();

	m_obj.Draw();

	m_obj.UnBind();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

	auto viewProjection = projection * view;

	Render::DrawCoordinateSystemOrigin(m_dr, viewProjection);

	for (int j = 0; j < 10; ++j)
	{
		glm::mat4 teapot_coordinate_system = glm::rotate(glm::mat4(1.0f), j / 10.0f * 2.0f * 3.14f, glm::vec3(0.0, 1.0, 0.0)) *
		                                     glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 30.0));

		Render::DrawCoordinateSystemOrigin(m_dr, viewProjection * teapot_coordinate_system);


		for (int i = 0; i < 10; ++i)
		{
			glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 7.0));
			glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 1.5));
			glm::mat4 rotate = glm::rotate(glm::mat4(1.0), i / 10.0f * 2.0f * 3.14f + time, glm::vec3(1.0, 0.0, 0.0));

			glm::mat4 box_coordinate_system = rotate * translate;

			Render::DrawCoordinateSystemOrigin(m_dr, viewProjection * teapot_coordinate_system * box_coordinate_system,
			                           2.0f);
		}
	}
	{
		glm::aabb2 view_box(glm::vec2(0.0), glm::vec2(m_width, m_height));

		UI::View view;
		view.size_in_dots = view_box.size();
		view.dpi = 72;

		using namespace UI::lit;

		auto root_block = UI::make_block(30_lpe, 200_w, 50_t, 50_bpe);
		auto block2 = UI::make_block(30_lpe, 200_w, 50_t, 50_bpe);
		root_block->AddChild(block2);

		UI::DoLayout(root_block, view);
		glm::mat4 prj =glm::ortho(view_box.minp.x, view_box.maxp.x, view_box.maxp.y, view_box.minp.y);

		UI::Traverse(root_block, nullptr, [prj, this](UI::Block* block, UI::Block* parent)
		{
			//Render::DrawRect(m_dr, box.minp, box.maxp, prj);

			float cornerRadius = 3.0f;

			auto box = block->GetBox();

			nvgBeginPath(vg);
			nvgRoundedRect(vg, box, cornerRadius);
			nvgFillColor(vg, nvgRGBA(28,30,34,192));
			// nvgFillColor(vg, nvgRGBA(255,192,0,255));
			nvgFill(vg);

			// Drop shadow
			auto shadowPaint = nvgBoxGradient(vg, glm::aabb2(box.minp + glm::vec2(0.0f, 2.0f), box.maxp + glm::vec2(0.0f, 2.0f)), cornerRadius*2, 10, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
			nvgBeginPath(vg);
			nvgRect(vg, glm::aabb2(box.minp - glm::vec2(10.0f), box.maxp + glm::vec2(10.0f, 20.0f)));
			nvgRoundedRect(vg, box, cornerRadius);
			nvgPathWinding(vg, NVG_HOLE);
			nvgFillPaint(vg, shadowPaint);
			nvgFill(vg);
		});
	}

	nvgBeginFrame(vg, m_width, m_height, 1.0f);

	renderDemo(vg, 0, 0, m_width, m_height, time, false, nullptr);

	nvgEndFrame(vg);
}


void Application::OnKeyAction(int key, char asci, int action, int mods)
{
	if (action == 0)
	{
		keyboard.ListenOnKeyDown(asci);
	}
	else if (action == 1)
	{
		keyboard.ListenOnKeyUp(asci);
	}
}

void Application::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
}
