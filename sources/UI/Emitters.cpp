#include "2DEngine/Renderer2D.h"
#include "Block.h"
#include "UI_Constraint_solver.h"


void UI::SFillEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags, float ppd)
{
	auto box = block->GetBox();
	r->Rect(box, col, block->GetRadius(), block->GetDepth());
}

UI::SFillEmitter::SFillEmitter(Render::color c): col(c)
{}

void UI::SShadowEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags, float ppd)
{
	auto box = block->GetBox();
	r->RectShadow(box, col, glm::vec2(0.), ComputeValue(box, size_value, size_unit, ppd), glm::vec4(0.0), block->GetDepth());
}

UI::SShadowEmitter::SShadowEmitter(Render::color col, float size_value, Constraint::Unit size_unit): col(col), size_value(size_value), size_unit(size_unit)
{}

void UI::SImageEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags, float ppd)
{
	auto box = block->GetBox();
	glm::aabb2 uv;
	glm::vec2 box_size = box.size();
	int nx = std::min(frames_count, nrow);
	int ny = frames_count / nrow;
	auto img_size = glm::vec2(image_size) / glm::vec2(nx, ny);
	glm::vec2 uv_size = box_size / img_size;

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
	uv_size /= glm::vec2(nx, ny);
	uv.minp += glm::vec2(
			t & T::FlipX ? nx - 1 - current_frame % nrow : current_frame % nrow ,
			t & T::FlipY ? ny - 1 - current_frame / nrow : current_frame / nrow) * uv_size;
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
	r->Rect(box, tex->m_handle, uv, glm::vec4(0), block->GetDepth());
}

void UI::SShaderEmitter::operator()(Render::Encoder* r, const UI::Block* block, float time, int flags, float ppd)
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

UI::SImageEmitter::SImageEmitter(Render::TexturePtr tex, ImSize::Enum size,  ImPos::Enum pos,  ImTransform::Enum t, int frames_count, int nrow):
	image_size(tex->GetSize()), tex(std::move(tex)), size(size), pos(pos), t(t), frames_count(frames_count), nrow(nrow), current_frame(0)
{
	if (frames_count == 0)
	{
		this->frames_count = 1;
	}
	if (nrow == 0)
	{
		this->nrow = this->frames_count;
	}
}


UI::SShaderEmitter::SShaderEmitter(Render::TexturePtr tex, int shader):
	tex(std::move(tex)), shader(shader)
{
}


UI::STextEmitter::STextEmitter(uint8_t f_id, std::string text, float height_value, Constraint::Unit height_unit, Render::color f_color, uint8_t f_style, uint8_t f_stroke):
	text(std::move(text)), f_color(f_color), height_value(height_value), height_unit(height_unit), f_id(f_id),  f_style(f_style), f_stroke(f_stroke)
{
}

void UI::STextEmitter::operator()(Render::Encoder* r, const Block* block, float time, int flags, float ppd)
{
	auto box = block->GetBox();
	float iheight = ComputeValue(box, height_value, height_unit, ppd);
	r->Text(f_id, box, text.c_str(), iheight, f_color, f_style, f_stroke, text.size(), block->GetDepth());
}
