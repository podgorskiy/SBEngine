#pragma once
#include "View.h"
#include "Render/Texture.h"
#include "Render/color.h"
#include <glm/glm.hpp>
#include "Render/Shader.h"
#include "Render/VertexSpec.h"
#include "utils/aabb.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fsal.h>
#include <Scriber.h>
#include <IRenderAPI.h>

namespace UI
{
	enum Command: uint8_t
	{
		C_RectCol,
		C_RectTex,
		C_Text,
		C_End
	};

	struct DrawCommand
	{
		enum FillType
		{

		};

		glm::mat3x2 pos_transform;
		glm::mat3x2 uv_transform;
		color color_a;
		color color_b;
		Render::TexturePtr image;
		int z_index = 0;
		uint8_t radius;
	};

	struct Vertex
	{
		Vertex() = default;

		Vertex(glm::vec2 pos, glm::vec2 uv, color col): pos(pos), uv(uv), col(col) {}
		glm::vec2 pos;
		glm::vec2 uv;
		color col;
	};

	struct Renderer
	{
		Renderer();

		void Rect(glm::aabb2 rect, color c);
		// void Rect(glm::aabb2 rect, int8_t rounding);
		void Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv = glm::aabb2(glm::vec2(1.0), glm::vec2(0.0)), int8_t rounding=0);
		void Text(glm::aabb2 rect, const char* text, size_t len=0);

	    void  PathClear()   { m_path.clear(); }
	    void  PathLineTo(const glm::vec2& pos)  { m_path.push_back(pos); }

	    ~Renderer();

	    void SetUp(UI::View view);
		void Init();
		void Draw();

		void PushVertex(glm::vec2 p, glm::vec2 uv, color color);

		int GetGlyphTexture() const;

		Scriber::Driver m_text_driver;
		fsal::FileSystem m_fs;

		std::vector<glm::vec2> m_path;
		std::vector<uint16_t> m_indexArray;
		std::vector<Vertex> m_vertexArray;
		Render::ProgramPtr m_programCol;
		Render::ProgramPtr m_programTex;
		Render::Uniform u_texture;
		bgfx::VertexLayout m_vertexSpec;
		fsal::File m_command_queue;
		Scriber::IRenderAPIPtr m_text_backend;
	};
}
