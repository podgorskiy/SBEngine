#include "Encoder.h"
#include "Commands.h"
#include <MemRefFile.h>
#include <algorithm>

using namespace Render;


#define BEGIN int _start = m_command_data.Tell();
#define END(NAME) int _size = m_command_data.Tell() - _start; m_command_queue.push_back(Render::Pack(NAME, depth, _start, _size));


Encoder::Encoder()
{
	m_sciscors.set_any();
	m_command_data = fsal::File(new fsal::MemRefFile());
}

bool compare_as_cmd(uint64_t i,uint64_t j)
{
  return i < j;
}

void Encoder::PushScissors(glm::aabb2 box)
{
	std::sort(m_command_queue.begin() + m_sorted, m_command_queue.end(), compare_as_cmd);
	m_sorted = m_command_queue.size();

	if (!scissors_stack.empty())
	{
		auto rect = scissors_stack.back();
		box &= rect;
	}
	m_sciscors = box;
	scissors_stack.push_back(box);

	int depth = -2147483647;

	BEGIN
	m_command_data.Write(box);
	END(C_SetScissors)

}

void Encoder::PopScissors()
{
	int depth = 2147483647;
	scissors_stack.pop_back();
	if (!scissors_stack.empty())
	{
		auto box = scissors_stack.back();
		BEGIN
		m_command_data.Write(box);
		END(C_SetScissors)
		m_sciscors = box;
	}
	else
	{
		BEGIN
		END(C_ResetScissors)
		m_sciscors.set_any();
	}
	std::sort(m_command_queue.begin() + m_sorted, m_command_queue.end(), compare_as_cmd);
	m_sorted = m_command_queue.size();
}

void Encoder::Rect(glm::aabb2 rect, color col, glm::vec4 radius, int depth)
{
	BEGIN
	m_command_data.Write(rect);
	m_command_data.Write(radius);
	m_command_data.Write(col);
	END(C_RectCol)
}

void Encoder::RectShadow(glm::aabb2 rect, Render::color col, glm::vec2 dir, float size, glm::vec4 radius, int depth)
{
	BEGIN
	m_command_data.Write(rect);
	m_command_data.Write(radius);
	m_command_data.Write(dir);
	m_command_data.Write(col);
	m_command_data.Write(size);
	END(C_RectShadow)
}

void Encoder::Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv, glm::vec4 radius, int depth)
{
	if (glm::is_overlapping(m_sciscors, rect))
	{
		BEGIN
		m_command_data.Write(rect);
		m_command_data.Write(radius);
		m_command_data.Write(uv);
		m_command_data.Write(texture);
		END(C_RectTex)
	}
}

void Encoder::RectShader(glm::aabb2 rect, bgfx::TextureHandle texture, uint8_t shader, int depth)
{
	if (glm::is_overlapping(m_sciscors, rect))
	{
		BEGIN
		m_command_data.Write(rect);
		m_command_data.Write(texture);
		m_command_data.Write(shader);
		END(C_Shader)
	}
}

void Encoder::Rect(glm::aabb2 rect, const glm::mat3& transform, bgfx::TextureHandle texture, glm::aabb2 uv, int depth)
{
	if (glm::is_overlapping(m_sciscors, rect))
	{
		BEGIN
		m_command_data.Write(rect);
		m_command_data.Write(transform);
		m_command_data.Write(uv);
		m_command_data.Write(texture);
		END(C_RectTex)
	}
}

void Encoder::Text(uint8_t f_id, glm::aabb2 rect, const char* text, float f_size, color f_color, uint8_t f_style, uint8_t f_stroke, size_t len, int depth)
{
	BEGIN
	m_command_data.Write(rect);
	m_command_data.Write(f_id);
	m_command_data.Write(f_size);
	m_command_data.Write(f_style);
	m_command_data.Write(f_color);
	m_command_data.Write(f_stroke);
	if (len == 0)
	{
		len = strlen(text);
	}
	m_command_data.Write(len + 1);
	m_command_data.Write((const uint8_t*)text, len);
	m_command_data.Write(char(0));
	END(C_Text)
}

void Encoder::Finish()
{
	std::sort(m_command_queue.begin() + m_sorted, m_command_queue.end(), compare_as_cmd);
	m_sorted = m_command_queue.size();
	m_command_data.Seek(0);
}

void Encoder::Reset()
{
	m_command_data.Seek(0);
	m_command_queue.clear();
	m_sorted = 0;
}
