#pragma once
#include "Object.h"
#include "Texture.h"
#include <glm/glm.hpp>

class Application
{
public:
	Application();
	~Application();

	void Draw(float time);
	void Resize(int width, int height);

	void OnKeyAction(int key, char asci, int action, int mods);

private:
	unsigned int m_program;

	unsigned int u_modelView;
	unsigned int u_model;
	
	unsigned int m_attrib_pos;
	unsigned int m_attrib_normal;
	unsigned int m_attrib_uv;
	unsigned int m_attrib_tangent;
	unsigned int m_attrib_color;

	unsigned int u_projection;
	unsigned int m_uniform_texture;
	unsigned int u_camera_pos;

	int m_width;
	int m_height;
	Object m_obj;
	TexturePtr m_texture;
};
