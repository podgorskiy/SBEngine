#pragma once
#include "Encoder.h"
#include "View.h"
#include "Path.h"
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

		Renderer2D();
	    ~Renderer2D();

	    Encoder* GetEncoder() { return &m_encoder; };

	    void SetUp(View view);
		void Init();
		void Draw();

		bool m_gamma_correction;
	private:
		void PrimReserve(int idx_count, int vtx_count);
		void PrimReset();
		void PrimRect(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, color col);
		void PrimRectRounded(const glm::vec2& a, const glm::vec2& c, const glm::vec4& radius, color col);
		void PrimConvexPolyFilled(const glm::vec2* points, int count, color col);

		Scriber::Driver m_text_driver;

		Encoder m_encoder;
		Path m_path;

		bool scissoring_enabled = false;

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
