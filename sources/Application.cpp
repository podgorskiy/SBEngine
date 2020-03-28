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


Keyboard keyboard;

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


	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


	const char* vertex_shader_src = R"(
		attribute vec3 a_position;
		attribute vec2 a_uv;

		uniform mat4 u_modelView;
		uniform mat4 u_model;
		uniform vec3 u_camera_pos;
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

	m_texture = Texture::LoadTexture("albido.pvr");

	Oquonie::GetInstance()->Install();
	Oquonie::GetInstance()->Start();
}


Application::~Application()
{
}

inline void* ToVoidPointer(int offset)
{
	size_t offset_ = static_cast<size_t>(offset);
	return reinterpret_cast<void*>(offset_);
}

void Application::Draw(float time)
{
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
	glUniform3fv(u_camera_pos, 1, &camera_pos[0]);

	glUniform1i(m_uniform_texture, 0);

	m_obj.Bind();

	m_obj.Draw();

	m_obj.UnBind();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
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
