#pragma once
#include "Render/Texture.h"
#include "color.h"
#include "utils/aabb.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fsal.h>
#include <Scriber.h>


namespace Render
{
	class Encoder
	{
	public:
		Encoder();

		void PushScissors(glm::aabb2 box);

		void PopScissors();

		void Rect(glm::aabb2 rect, color c, glm::vec4 radius = glm::vec4(0));

		void RectShadow(glm::aabb2 rect, color c, glm::vec2 dir, float size, glm::vec4 radius = glm::vec4(0));

		void Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv = glm::aabb2(glm::vec2(0.0, 1.0), glm::vec2(1.0, 0.0)), glm::vec4 radius = glm::vec4(0));

		void RectShader(glm::aabb2 rect, bgfx::TextureHandle texture, uint8_t shader);

		void Rect(glm::aabb2 rect, const glm::mat3& transform, bgfx::TextureHandle texture, glm::aabb2 uv = glm::aabb2(glm::vec2(0.0, 1.0), glm::vec2(1.0, 0.0)));

		void Text(uint8_t f_id, glm::aabb2 rect, const char* text, float f_size, color f_color, uint8_t f_style=0, uint8_t f_stroke=0, size_t len=0);

		fsal::File GetCommandQueue() { return m_command_queue; }
	private:
		std::vector<glm::aabb2> scissors_stack;
		glm::aabb2 m_sciscors;
		fsal::File m_command_queue;
	};
}
