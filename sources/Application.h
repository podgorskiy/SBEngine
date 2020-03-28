#pragma once
#include "Object.h"
#include "Texture.h"
#include "Render/Shader.h"
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

	unsigned int u_modelView;
	unsigned int u_model;

	unsigned int u_projection;
	unsigned int m_uniform_texture;
	unsigned int u_camera_pos;

	int m_width;
	int m_height;
	Object m_obj;
	TexturePtr m_texture;
};
