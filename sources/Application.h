#pragma once
#include "Object.h"
#include "IApplication.h"
#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/DebugRenderer.h"
#include "UI/UI_Backend.h"
#include "misc/bgfx_custom_printers.h"
#include <glm/glm.hpp>


namespace UI
{
	class Block;

	typedef std::shared_ptr<Block> BlockPtr;
}

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
	void OnMouseButton(int button, int action, int mods) override;
	void OnMouseMove(glm::vec2 pos) override;
	void OnScroll(glm::vec2 offset) override {};
	void OnIconification(int iconified) override {};
	void OnWindowClose() override {};
	void SetMouse(float x, float y, bool mouseDown) override
	{
		mouse_pos = glm::vec2(x, y);
		mouse_left_click_prev = mouse_left_click;
		mouse_left_click = mouseDown;
	};


private:
	uint32_t m_reset_flags = 0;
	glm::ivec2 m_windowBufferSize;
	CustomPrinters m_cp;

	Render::ProgramPtr m_program;
	Render::DebugRenderer m_dr;
	UI::Renderer m_uir;

	Render::Uniform u_texture;

	UI::BlockPtr root;

	glm::vec2 mouse_pos;
	bool mouse_left_click = false;
	bool mouse_left_click_prev = false;
	Object m_obj;
};
