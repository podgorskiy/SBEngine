#pragma once
#include "View.h"
#include "Render/Texture.h"
#include "UI/color.h"
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
		C_SetScissors,
		C_ResetScissors,
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
		enum ArcType
		{
			Arc_TL,
			Arc_TR,
			Arc_BR,
			Arc_BL,
		};

		Renderer();

		void Rect(glm::aabb2 rect, color c, glm::vec4 radius = glm::vec4(0));
		// void Rect(glm::aabb2 rect, int8_t rounding);
		void Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv = glm::aabb2(glm::vec2(1.0), glm::vec2(0.0)), glm::vec4 radius = glm::vec4(0));
		void Text(glm::aabb2 rect, const char* text, size_t len=0);

	    void PathClear()   { m_path.clear(); }
	    void PathLineTo(const glm::vec2& pos)  { m_path.push_back(pos); }
		void PathArcTo(const glm::vec2& center, float radius, float a_min, float a_max, int num_segments);
		void Path90Arc(const glm::vec2& center, float radius, ArcType type);
		void PathRect(const glm::vec2& a, const glm::vec2& b, const glm::vec4& radius);

	    ~Renderer();

	    void SetUp(UI::View view);
		void Init();
		void Draw();
		void PrimReserve(int idx_count, int vtx_count);
		void PrimReset();

		void PrimRect(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, color col);
		void PrimRectRounded(const glm::vec2& a, const glm::vec2& c, const glm::vec4& radius, color col);

		void PrimConvexPolyFilled(glm::vec2* points, int count, color col);

		int GetGlyphTexture() const;

		void PushScissors(glm::aabb2 box);
		void PopScissors();

		Scriber::Driver m_text_driver;
		fsal::FileSystem m_fs;

		std::vector<glm::aabb2> scissors_stack;
		glm::aabb2 current_sciscors;
		glm::aabb2 encode_sciscors;
		bool scissoring_enabled = false;

		std::vector<glm::vec2> m_path;
		std::vector<uint16_t> m_indexArray;
		std::vector<Vertex> m_vertexArray;

		uint16_t _current_index;
		uint16_t* _index_write_ptr;
		Vertex* _vertex_write_ptr;

		Render::ProgramPtr m_programCol;
		Render::ProgramPtr m_programTex;
		Render::Uniform u_texture;
		bgfx::VertexLayout m_vertexSpec;
		fsal::File m_command_queue;
		Scriber::IRenderAPIPtr m_text_backend;

		bool m_gamma_correction;
	};
}
