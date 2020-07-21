#pragma once
#include "Encoder.h"
#include "View.h"
#include "Vertices.h"
#include "Render/Shader.h"
#include "utils/aabb.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <Scriber.h>
#include <IRenderAPI.h>


namespace Render
{
	class Renderer2D
	{
	public:
		enum ArcType : uint8_t
		{
			Arc_TL,
			Arc_TR,
			Arc_BR,
			Arc_BL,
		};

		Renderer2D();
	    ~Renderer2D();

	    Encoder* GetEncoder() { return &m_encoder; };

	    void PathClear()   { m_path.clear(); }
	    void PathLineTo(const glm::vec2& pos)  { m_path.push_back(pos); }
		void PathArcTo(const glm::vec2& center, float radius, float a_min, float a_max, int num_segments);
		void Path90Arc(const glm::vec2& center, float radius, ArcType type);
		void PathRect(const glm::vec2& a, const glm::vec2& b, const glm::vec4& radius);

	    void SetUp(View view);
		void Init();
		void Draw();

		void PrimReserve(int idx_count, int vtx_count);
		void PrimReset();
		void PrimRect(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, color col);
		void PrimRectRounded(const glm::vec2& a, const glm::vec2& c, const glm::vec4& radius, color col);
		void PrimConvexPolyFilled(glm::vec2* points, int count, color col);

		bool m_gamma_correction;
	private:
		Scriber::Driver m_text_driver;

		Encoder m_encoder;

		bool scissoring_enabled = false;

		std::vector<glm::vec2> m_path;
		std::vector<uint16_t> m_indexArray;
		std::vector<Vertex> m_vertexArray;

		uint16_t _current_index;
		uint16_t* _index_write_ptr;
		Vertex* _vertex_write_ptr;
		glm::aabb2 current_sciscors;

		Render::ProgramPtr m_programCol;
		Render::ProgramPtr m_programTex;
		Render::Uniform u_texture;
		bgfx::VertexLayout m_vertexSpec;
		Scriber::IRenderAPIPtr m_text_backend;
	};
}
