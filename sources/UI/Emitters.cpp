#include "2DEngine/Renderer2D.h"
#include "Block.h"


void UI::SFillEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags)
{
	auto box = block->GetBox();
	r->Rect(box, col, block->GetRadius());
}

UI::SFillEmitter::SFillEmitter(Render::color c): col(c)
{}

void UI::SShadowEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags)
{
	auto box = block->GetBox();
	r->RectShadow(box, col, glm::vec2(0.), size);
}

UI::SShadowEmitter::SShadowEmitter(Render::color col, float size): col(col), size(size)
{}

void UI::SImageEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags)
{
	auto box = block->GetBox();
	glm::aabb2 uv;
	glm::vec2 box_size = box.size();
	glm::vec2 uv_size = box_size / glm::vec2(image_size);

	using namespace UI::lit;
	switch(size)
	{
		case S::Auto: break;
		case S::Contain: uv_size = uv_size / glm::min(uv_size.x, uv_size.y); break;
		case S::Cover: uv_size = uv_size / glm::max(uv_size.x, uv_size.y); break;
		case S::Fill: uv_size = glm::vec2(1.0); break;
	}
	switch (pos)
	{
		case P::LeftTop: uv.minp = glm::vec2(0.0f); break;
		case P::leftCenter: uv.minp = glm::vec2(0.0f, 0.5f - 0.5f * uv_size.y); break;
		case P::leftBottom: uv.minp = glm::vec2(0.0f, 1.0f - uv_size.y); break;
		case P::RightTop: uv.minp = glm::vec2(1.0f- uv_size.x, 0.0f); break;
		case P::RightCenter: uv.minp = glm::vec2(1.0f - uv_size.x, 0.5f - 0.5f * uv_size.y); break;
		case P::RightBottom: uv.minp = glm::vec2(1.0f - uv_size.x, 1.0f - uv_size.y); break;
		case P::CenterTop: uv.minp = glm::vec2(0.5f - 0.5f * uv_size.x, 0.0f); break;
		case P::CenterCenter: uv.minp = 0.5f - 0.5f * uv_size; break;
		case P::CenterBottom: uv.minp = glm::vec2(0.5f - 0.5f * uv_size.x, 1.0f - uv_size.y); break;
	}
	uv.maxp = uv.minp + uv_size;
	switch (t)
	{
		default:
			uv.minp.y = 1.0 - uv.minp.y;
			uv.maxp.y = 1.0 - uv.maxp.y;
			break;
		case T::FlipY:
			break;
		case T::FlipX:
			uv.minp = 1.0f - uv.minp;
			uv.maxp = 1.0f - uv.maxp;
			break;
		case T::FlipX | T::FlipY:
			uv.minp.x = 1.0 - uv.minp.x;
			uv.maxp.x = 1.0 - uv.maxp.x;
			break;
	}
	r->Rect(box, tex->m_handle, uv);
}

void UI::SShaderEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags)
{
	auto box = block->GetBox();
	if (tex)
	{
		r->RectShader(box, tex->m_handle, shader);
	}
	else
	{
		r->RectShader(box, {bgfx::kInvalidHandle}, shader);
	}
}

UI::SImageEmitter::SImageEmitter(Render::TexturePtr tex, ImSize::Enum size,  ImPos::Enum pos,  ImTransform::Enum t):
	image_size(tex->GetSize()), tex(std::move(tex)), size(size), pos(pos), t(t)
{
}


UI::SShaderEmitter::SShaderEmitter(Render::TexturePtr tex, int shader):
	tex(std::move(tex)), shader(shader)
{
}


UI::STextEmitter::STextEmitter(uint8_t f_id, std::string text, uint8_t f_height, Render::color f_color, uint8_t f_style, uint8_t f_stroke):
	text(std::move(text)), f_color(f_color), f_id(f_id),  f_style(f_style), f_height(f_height), f_stroke(f_stroke)
{
}

void UI::STextEmitter::operator()(Render::Encoder* r, const Block* block, float time, int flags)
{
	auto box = block->GetBox();
	r->Text(f_id, box, text.c_str(), f_height, f_color, f_style, f_stroke, text.size());
}
