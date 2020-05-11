#pragma once
#include "Render/Texture.h"
#include <glm/glm.hpp>
#include "Render/Shader.h"
#include "Render/VertexSpec.h"
#include "utils/aabb.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <fsal.h>
#include <Scriber.h>
#include <IRenderAPI.h>


typedef glm::vec<4, uint8_t> color;
typedef glm::vec<4, uint16_t> color16;

inline color operator/(const color16& c, int d)
{
	return color(c[0] / d, c[1] / d, c[2] / d, c[3] / d);
}

inline color operator*(const color& ca, const color& cb)
{
	return (color16(ca) * color16(cb)) / 255;
}

namespace UI
{
	enum Command: uint8_t
	{
		C_Rect,
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

		void Rect(glm::aabb2 rect, glm::aabb2 uv = glm::aabb2(glm::vec2(0.0), glm::vec2(1.0)), int8_t rounding=0);
		void Text(glm::aabb2 rect, const char* text, size_t len=0);

	    void  PathClear()   { m_path.clear(); }
	    void  PathLineTo(const glm::vec2& pos)  { m_path.push_back(pos); }

	    ~Renderer();

		void Init();
		void Draw();

		void PushVertex(glm::vec2 p, glm::vec2 uv, color color);

		int GetGlyphTexture() const;

		Scriber::Driver m_text_driver;
		fsal::FileSystem m_fs;

		std::vector<glm::vec2> m_path;
		std::vector<uint16_t> m_indexArray;
		std::vector<Vertex> m_vertexArray;
		Render::ProgramPtr m_program;
		bgfx::VertexLayout m_vertexSpec;
		bgfx::DynamicIndexBufferHandle m_ibh;
		bgfx::DynamicVertexBufferHandle m_vbh;
		fsal::File m_command_queue;
		Scriber::IRenderAPIPtr m_text_backend;
	};
}
