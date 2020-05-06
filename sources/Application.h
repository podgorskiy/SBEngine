#pragma once
#include "Object.h"
#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/DebugRenderer.h"
#include "UI/UI_Backend.h"
#include <glm/glm.hpp>


class Application
{
public:
	Application(int argc, const char* const* argv);
	~Application();

	void Draw(float time);
	void Resize(int width, int height);

	void OnKeyAction(int key, char asci, int action, int mods);

private:
	Render::ProgramPtr m_program;
	Render::DebugRenderer m_dr;
	UI::Renderer m_uir;

	unsigned int u_modelView = -1;
	unsigned int u_model = -1;

	unsigned int u_projection = -1;
	unsigned int m_uniform_texture = -1;

	int m_width;
	int m_height;
	Object m_obj;
	Render::TexturePtr m_texture;
};
