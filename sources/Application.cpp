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

GLuint CompileShader(const char* src, GLint type)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);

	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1)
	{
		spdlog::warn("{} during shader compilation.", compiled == GL_TRUE ? "Warning" : "Error");
		char* buf = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, NULL, buf);
		spdlog::warn("Compilation log: {}", buf);
		delete[] buf;
	}
	
	return shader;
}

Application::Application()
{
	gl3wInit();
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
			v_uv = vec2(a_uv.x, a_uv.y);
			gl_Position = u_projection * pos;
		}
	)";

	const char* fragment_shader_src = R"(
		varying vec2 v_uv;

		uniform sampler2D u_texture;

		uniform mat4 u_modelView;

		void main()
		{
			vec3 color = vec3(1.0);
			gl_FragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
		}
	)";

	const char* skybox_fragment_shader_src = R"(
		// light and material properties
		varying vec3 v_E;
		uniform mat4 u_modelView;

		uniform samplerCube u_cubemap;

		void main()
		{
			vec3 E = normalize(vec3(vec4(v_E, 0.0) * u_modelView));
			vec3 frag_colour = textureCube(u_cubemap, E).xyz;
			gl_FragColor = vec4(frag_colour, 1.0);
		}
	)";

	int vertex_shader_handle = CompileShader(vertex_shader_src, GL_VERTEX_SHADER);
	int fragment_shader_handle = CompileShader(fragment_shader_src, GL_FRAGMENT_SHADER);

	m_program = glCreateProgram();

	glAttachShader(m_program, vertex_shader_handle);
	glAttachShader(m_program, fragment_shader_handle);

	glLinkProgram(m_program);

	int linked;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* buf = new char[infoLen];
			glGetProgramInfoLog(m_program, infoLen, NULL, buf);
			spdlog::info("Linking error: \n{}", buf);
			delete[] buf;
		}
	}

	glDetachShader(m_program, vertex_shader_handle);
	glDetachShader(m_program, fragment_shader_handle);

	glDeleteShader(vertex_shader_handle);
	glDeleteShader(fragment_shader_handle);

	m_attrib_pos = glGetAttribLocation(m_program, "a_position");
	m_attrib_uv = glGetAttribLocation(m_program, "a_uv");

	u_modelView = glGetUniformLocation(m_program, "u_modelView");
	u_projection = glGetUniformLocation(m_program, "u_projection");

	u_modelView = glGetUniformLocation(m_program, "u_modelView");
	u_projection = glGetUniformLocation(m_program, "u_projection");

	m_uniform_texture = glGetUniformLocation(m_program, "u_texture");

	Oquonie::GetInstance()->Install();
	Oquonie::GetInstance()->Start();
}


Application::~Application()
{
	glDeleteProgram(m_program);
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

	glUseProgram(m_program);

	glUniformMatrix4fv(u_projection, 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(u_modelView, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]);
	glUniform3fv(u_camera_pos, 1, &camera_pos[0]);

	glUniform1i(m_uniform_texture, 0);
//
//	m_obj.Bind();
//
//	glEnableVertexAttribArray(m_attrib_pos);
//	glVertexAttribPointer(m_attrib_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ToVoidPointer(0));
//
//	glEnableVertexAttribArray(m_attrib_uv);
//	glVertexAttribPointer(m_attrib_uv, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ToVoidPointer(1 * sizeof(glm::vec3)));
//
//	m_obj.Draw();
//
//	glDisableVertexAttribArray(m_attrib_pos);
//	glDisableVertexAttribArray(m_attrib_uv);
//
//	m_obj.UnBind();
//
//	glActiveTexture(GL_TEXTURE0 + 0);
//	glBindTexture(GL_TEXTURE_2D, 0);

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
