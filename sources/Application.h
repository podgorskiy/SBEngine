#pragma once
#include "Object.h"
#include "IApplication.h"
#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/DebugRenderer.h"
#include "UI/UI_Backend.h"
#include <glm/glm.hpp>


class Application: public IApplication
{
public:
	Application(int argc, const char* const* argv);
	~Application() final;

	// IApplication
	void Update(float time, float deltaTime) final;
	void OnWindowResize(glm::ivec2 size) final;
	void OnKeyAction(int key, char asci, int action, int mods);
	void OnKey(int key, int scancode, int action, int mods) override {};
	void OnSetChar(unsigned int c) override  {};
	void OnMouseButton(int button, int action, int mods) override {};
	void OnMouseMove(glm::vec2 pos) override {};
	void OnScroll(glm::vec2 offset) override {};
	void OnIconification(int iconified) override {};
	void OnWindowClose() override {};
	void SetMouse(float x, float y, bool mouseDown) override {};

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
