#include "Renderer2D.h"
#include "View.h"
#include "Render/Shader.h"
#include <tuple>
#include <MemRefFile.h>
#include <spdlog/spdlog.h>

#include <bgfx/bgfx.h>
#include "views.h"


using namespace Render;



class TextBackend: public Scriber::IRenderAPI
{
public:
	TextBackend()
	{
		int size = 1024;

		m_textureHandle = bgfx::createTexture2D(
			(uint16_t)size
			, (uint16_t)size
			, false
			, 1
			, bgfx::TextureFormat::RG8
			, BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP
			, nullptr
		);

		m_program = Render::MakeProgram("vs_text.bin", "fs_text.bin");

		m_vertexSpec
			.begin()
			.add(bgfx::Attrib::Position,  2, bgfx::AttribType::Int16, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true)
			.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
			.end();

		u_texture = m_program->GetUniform("u_texture");
		u_stroke = m_program->GetUniform("u_stroke");
	}

	void SaveTextureToFile() override {};

	void UpdateTexture(Scriber::Image glyph, Scriber::u16vec2 pos) override
	{
		const bgfx::Memory* memory = bgfx::copy(glyph.ptr<const uint8_t*>(0), (int)glyph.GetRowSizeAligned() * glyph.GetSize().y);
		Scriber::ivec2 p(pos);
		bgfx::updateTexture2D(m_textureHandle, 0, 0, p.x, p.y, glyph.GetSize().x, glyph.GetSize().y, memory, glyph.GetRowSizeAligned());
	}

	void ClearTexture() override
	{
	}

	void Render(Scriber::Vertex* vertexBuffer, uint16_t* indexBuffer, uint16_t vertex_count, uint16_t primitiveCount) override
	{
		bgfx::TransientVertexBuffer tvb;
		bgfx::TransientIndexBuffer tib;

		bgfx::allocTransientVertexBuffer(&tvb, vertex_count, m_vertexSpec);
		bgfx::allocTransientIndexBuffer(&tib, primitiveCount * 3);

		memcpy(tvb.data, vertexBuffer, vertex_count * sizeof(Scriber::Vertex));
		memcpy(tib.data, indexBuffer, primitiveCount * 3 * sizeof(uint16_t));

		bgfx::setVertexBuffer(0, &tvb, 0, vertex_count);
		bgfx::setIndexBuffer(&tib, 0, primitiveCount * 3);

		bgfx::setTexture(0, u_texture.m_handle,  m_textureHandle);

		u_stroke.ApplyValue(glm::vec4(0.0, 0.0, 0.0, 1.0));

		uint64_t state = 0
					| BGFX_STATE_WRITE_RGB
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA);
		bgfx::setState(state);

		bgfx::submit(ViewIds::GUI, m_program->GetHandle(), 0);
	}

	bgfx::TextureHandle m_textureHandle;
	Render::ProgramPtr m_program;
	bgfx::VertexLayout m_vertexSpec;
	Render::Uniform u_texture;
	Render::Uniform u_stroke;
};


Renderer2D::Renderer2D(): m_gamma_correction(false)
{
	encode_sciscors.set_any();
	scissoring_enabled = false;
}

Renderer2D::~Renderer2D()
{

}

void Renderer2D::Init()
{
	m_programCol = Render::MakeProgram("vs_gui.bin", "fs_gui.bin");
	m_programTex = Render::MakeProgram("vs_gui.bin", "fs_gui_tex.bin");

	u_texture = m_programTex->GetUniform("u_texture");

	m_vertexSpec
		.begin()
		.add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
		.end();

	m_command_queue = fsal::File(new fsal::MemRefFile());

	Scriber::Driver::SetCustomIOFunctions(
			[this](const char* filename, const char* mode)
			{
				fsal::Mode m = fsal::kRead;
				if (strcmp(mode, "rb") == 0)
				{
					m = fsal::kRead;
				}
				if (strcmp(mode, "wb") == 0)
				{
					m = fsal::kWrite;
				}
				auto f = m_fs.Open(filename, m);
				if (!f)
					return (Scriber::UserFile*)nullptr;
				return (Scriber::UserFile*)new std::shared_ptr<fsal::FileInterface>(f.GetInterface());
			},
			+[](Scriber::UserFile* userfile)
			{
				delete (std::shared_ptr<fsal::FileInterface>*)userfile;
				return 0;
			},
			+[](void* ptr, size_t size, size_t count, Scriber::UserFile* userfile)
			{
				size_t read = 0;
				(*(std::shared_ptr<fsal::FileInterface>*)userfile)->ReadData((uint8_t*)ptr, size * count, &read);
				return read;
			},
			+[](Scriber::UserFile* userfile, long int offset, int origin)
			{
				auto file = (*(std::shared_ptr<fsal::FileInterface>*)userfile);

				switch (origin)
				{
				case 0:
					return (int)!file->SetPosition(offset);
				case 1:
					return (int)!file->SetPosition(file->GetPosition() + offset);
				case 2:
					return (int)!file->SetPosition(file->GetSize() + offset);
				}
				return 1;
			},
			+[](Scriber::UserFile* userfile)
			{
				auto file = (*(std::shared_ptr<fsal::FileInterface>*)userfile);
				return file->GetPosition();
			});

	m_text_backend = std::make_shared<TextBackend>();
	m_text_driver.SetBackend(m_text_backend);

	auto id = m_text_driver.NewTypeface("NotoSans");
	m_text_driver.AndFontToTypeface(id, "fonts/NotoSans-Regular.ttf", Scriber::FontStyle::Regular);
}

void Renderer2D::Rect(glm::aabb2 rect, color col, glm::vec4 radius)
{
	m_command_queue.Write(C_RectCol);
	m_command_queue.Write(rect);
	m_command_queue.Write(radius);
	m_command_queue.Write(col);
}


void Renderer2D::Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv, glm::vec4 radius)
{
	if (glm::is_overlapping(encode_sciscors, rect))
	{
		m_command_queue.Write(C_RectTex);
		m_command_queue.Write(rect);
		m_command_queue.Write(radius);
		m_command_queue.Write(uv);
		m_command_queue.Write(texture);
	}
}

void Renderer2D::Text(glm::aabb2 rect, const char* text, size_t len)
{
	m_command_queue.Write(C_Text);
	m_command_queue.Write(rect);
	if (len == 0)
	{
		len = strlen(text);
	}
	m_command_queue.Write(len + 1);
	m_command_queue.Write((const uint8_t*)text, len);
	m_command_queue.Write(char(0));
}

void Renderer2D::SetUp(View view_box)
{
	auto prj = glm::ortho(view_box.view_box.minp.x, view_box.view_box.maxp.x, view_box.view_box.maxp.y, view_box.view_box.minp.y);
	bgfx::setViewTransform(ViewIds::GUI, nullptr, &prj[0]);
}

void Renderer2D::PushScissors(glm::aabb2 box)
{
	m_command_queue.Write(C_SetScissors);

	if (!scissors_stack.empty())
	{
		auto rect = scissors_stack.back();
		box &= rect;
	}
	encode_sciscors = box;
	scissors_stack.push_back(box);
	m_command_queue.Write(box);
}

void Renderer2D::PopScissors()
{
	scissors_stack.pop_back();
	if (!scissors_stack.empty())
	{
		auto box = scissors_stack.back();
		m_command_queue.Write(C_SetScissors);
		m_command_queue.Write(box);
		encode_sciscors = box;
	}
	else
	{
		m_command_queue.Write(C_ResetScissors);
		encode_sciscors.set_any();
	}
}


void Renderer2D::PathArcTo(const glm::vec2& center, float radius, float a_min, float a_max, int num_segments)
{
	if (radius == 0.0f)
	{
		m_path.push_back(center);
		return;
	}

	m_path.reserve(m_path.size() + (num_segments + 1));
	for (int i = 0; i <= num_segments; i++)
	{
		const float a = a_min + ((float) i / (float) num_segments) * (a_max - a_min);
		m_path.push_back(center + glm::vec2(glm::cos(a), glm::cos(a)) * radius);
	}
}

void Renderer2D::Path90Arc(const glm::vec2& center, float radius, ArcType type)
{
	if (radius == 0.0f)
	{
		m_path.push_back(center);
		return;
	}

	int num_segments = int(0.5f + 5.2f / 4.0f * (float)powf(radius - 0.8f, 0.53f));
	num_segments = std::min(num_segments, 180);


	m_path.reserve(m_path.size() + (num_segments + 1));
	for (int i = 0; i <= num_segments; i++)
	{
		const float a = ((float) i / (float) num_segments) * glm::pi<float>() / 2.0f;
		switch(type)
		{
			case Arc_TL:
				m_path.push_back(center + glm::vec2(-glm::cos(a),-glm::sin(a)) * radius);
				break;
			case Arc_TR:
				m_path.push_back(center + glm::vec2( glm::sin(a),-glm::cos(a)) * radius);
				break;
			case Arc_BR:
				m_path.push_back(center + glm::vec2( glm::cos(a), glm::sin(a)) * radius);
				break;
			case Arc_BL:
				m_path.push_back(center + glm::vec2(-glm::sin(a), glm::cos(a)) * radius);
				break;
		}
	}
}

void Renderer2D::PathRect(const glm::vec2& a, const glm::vec2& c, const glm::vec4& radius)
{
    glm::vec2 b(c.x, a.y), d(a.x, c.y);
	Path90Arc(a + glm::vec2( radius.x, radius.x), radius.x, Arc_TL);
	Path90Arc(b + glm::vec2(-radius.y, radius.y), radius.y, Arc_TR);
	Path90Arc(c + glm::vec2(-radius.z,-radius.z), radius.z, Arc_BR);
	Path90Arc(d + glm::vec2( radius.w,-radius.w), radius.w, Arc_BL);
}

void Renderer2D::PrimReserve(int idx_count, int vtx_count)
{
	m_vertexArray.resize(m_vertexArray.size() + vtx_count);
	m_indexArray.resize(m_indexArray.size() + idx_count);
	_index_write_ptr = &m_indexArray[m_indexArray.size() - idx_count];
	_vertex_write_ptr = &m_vertexArray[m_vertexArray.size() - vtx_count];
}


void Renderer2D::PrimReset()
{
	m_vertexArray.resize(0);
	m_indexArray.resize(0);
	_current_index = 0;
	_index_write_ptr = &m_indexArray[0];
	_vertex_write_ptr = &m_vertexArray[0];
}


void Renderer2D::PrimRect(const glm::vec2& a, const glm::vec2& c, const glm::vec2& uv_a, const glm::vec2& uv_c, color col)
{
	PrimReserve(6, 4);
    glm::vec2 b(c.x, a.y), d(a.x, c.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);
    auto idx = _current_index;
    _index_write_ptr[0] = idx; _index_write_ptr[1] = idx+1; _index_write_ptr[2] = idx+2;
    _index_write_ptr[3] = idx; _index_write_ptr[4] = idx+2; _index_write_ptr[5] = idx+3;
    _vertex_write_ptr[0].pos = a; _vertex_write_ptr[0].uv = uv_a; _vertex_write_ptr[0].col = col;
    _vertex_write_ptr[1].pos = b; _vertex_write_ptr[1].uv = uv_b; _vertex_write_ptr[1].col = col;
    _vertex_write_ptr[2].pos = c; _vertex_write_ptr[2].uv = uv_c; _vertex_write_ptr[2].col = col;
    _vertex_write_ptr[3].pos = d; _vertex_write_ptr[3].uv = uv_d; _vertex_write_ptr[3].col = col;
    _vertex_write_ptr += 4;
    _current_index += 4;
    _index_write_ptr += 6;
}

void Renderer2D::PrimRectRounded(const glm::vec2& a, const glm::vec2& c, const glm::vec4& radius, color col)
{
	PathClear();
	PathRect(a, c, radius);
	PrimConvexPolyFilled(&m_path[0], m_path.size(), col);
}

void Renderer2D::PrimConvexPolyFilled(glm::vec2* points, int count, color col)
{
    if (count < 3)
        return;

	const float anti_aliasing = 1.0f;
	const color col_trans = color(glm::vec<3, uint8_t>(col), 0);
	const int idx_count = glm::max(0, (count - 2) * 3) + count * 6;
	const int vtx_count = count * 2;
	PrimReserve(idx_count, vtx_count);

	for (int i = 2; i < count; i++)
	{
		_index_write_ptr[0] = _current_index;
		_index_write_ptr[1] = _current_index + 2 * i - 2;
		_index_write_ptr[2] = _current_index + 2 * i - 0;
		_index_write_ptr += 3;
	}

	glm::vec2 p1 = points[count - 2];
	glm::vec2 p2 = points[count - 1];
	glm::vec2 n1 = glm::normalize(p2 - p1);
	n1 = glm::vec2(n1.y, -n1.x);
	for (int i0 = count - 2, i1 = count - 1, i2 = 0; i2 < count; i0 = i1, i1 = i2++)
	{
		p1 = p2;
		p2 = points[i2];

		glm::vec2 n0 = n1;
		n1 = glm::normalize(p2 - p1);
		n1 = glm::vec2(n1.y, -n1.x);
		float dm_x = (n0.x + n1.x) * 0.5f;
		float dm_y = (n0.y + n1.y) * 0.5f;
		float k = 0.5f * anti_aliasing / std::max(dm_x * dm_x + dm_y * dm_y, 0.5f);
		dm_x *= k;
		dm_y *= k;

		_vertex_write_ptr[0].pos.x = (p1.x - dm_x);
		_vertex_write_ptr[0].pos.y = (p1.y - dm_y);
		_vertex_write_ptr[0].uv = glm::vec2(0.0, 0.0);
		_vertex_write_ptr[0].col = col;
		_vertex_write_ptr[1].pos.x = (p1.x + dm_x);
		_vertex_write_ptr[1].pos.y = (p1.y + dm_y);
		_vertex_write_ptr[1].uv = glm::vec2(0.0, 0.0);
		_vertex_write_ptr[1].col = col_trans;
		_vertex_write_ptr += 2;

		_index_write_ptr[0] = _current_index + 2 * i1 + 0;
		_index_write_ptr[1] = _current_index + 2 * i0 + 0;
		_index_write_ptr[2] = _current_index + 2 * i0 + 1;
		_index_write_ptr[3] = _current_index + 2 * i0 + 1;
		_index_write_ptr[4] = _current_index + 2 * i1 + 1;
		_index_write_ptr[5] = _current_index + 2 * i1 + 0;
		_index_write_ptr += 6;
	}
	_current_index += vtx_count;
}


void Renderer2D::Draw()
{
	m_command_queue.Write(C_End);
	m_command_queue.Seek(0);
	PrimReset();

	bgfx::TextureHandle tex;

	Command cmd;
	while (m_command_queue.Read(cmd) && cmd != C_End)
	{
		bool need_flush = false;
		switch(cmd)
		{
			case C_RectCol:
			{
				glm::aabb2 rect;
				color col;
				glm::vec4 radius;
				m_command_queue.Read(rect);
				m_command_queue.Read(radius);
				m_command_queue.Read(col);

				if (m_gamma_correction)
				{
					col = glm::pow(glm::vec4(col) / 255.0f, glm::vec4(2.2f)) * 255.0f;
				}

				if (radius == glm::vec4(0))
				{
					PrimRect(rect.minp, rect.maxp, glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), col);
				}
				else
				{
					PrimRectRounded(rect.minp, rect.maxp, radius, col);
				}
			}
			need_flush = true;
			break;

			case C_RectTex:
			{
				glm::aabb2 rect;
				glm::aabb2 uv;
				glm::vec4  radius;
				m_command_queue.Read(rect);
				m_command_queue.Read(radius);
				m_command_queue.Read(uv);
				m_command_queue.Read(tex);

				if (radius == glm::vec4(0))
				{
					PrimRect(rect.minp, rect.maxp, uv.minp, uv.maxp, color(0));
				}
				else
				{

				}
			}
			need_flush = true;
			break;

			case C_Text:
			{
				glm::aabb2 rect;
				size_t len;
				m_command_queue.Read(rect);
				m_command_queue.Read(len);
				auto ptr = (const char*)m_command_queue.GetDataPointer() + m_command_queue.Tell();
				m_command_queue.Seek(len, fsal::File::CurrentPosition);
				m_text_driver.DrawLabel(ptr, rect.minp.x, rect.minp.y, Scriber::Font(0, 32, Scriber::FontStyle::Regular, 0xFFFFFFFF, 1));
			}
			need_flush = true;
			break;
			case C_SetScissors:
			{
				m_command_queue.Read(current_sciscors);
				scissoring_enabled = true;
			}
			need_flush = true;
			break;
			case C_ResetScissors:
			{
				current_sciscors.reset();
				scissoring_enabled = false;
			}
			need_flush = true;
			break;
		}

		if (scissoring_enabled)
		{
			bgfx::setScissor(
					(int)current_sciscors.minp.x,
					(int)current_sciscors.minp.y,
					(int)current_sciscors.size().x,
					(int)current_sciscors.size().y);
		}

		switch(cmd)
		{
			case C_RectCol:
			{
				int num_vertex = m_vertexArray.size();
				int num_index = m_indexArray.size();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_vertexArray.data(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_indexArray.data(), num_index * sizeof(uint16_t));

				PrimReset();

				bgfx::setVertexBuffer(0, &tvb, 0, num_vertex);
				bgfx::setIndexBuffer(&tib, 0, num_index);

				uint64_t state = 0
				                 | BGFX_STATE_WRITE_RGB
				                 | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				bgfx::setState(state);

				bgfx::submit(ViewIds::GUI, m_programCol->GetHandle(), 0);
			}
			break;
			case C_RectTex:
			{
				int num_vertex = m_vertexArray.size();
				int num_index = m_indexArray.size();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_vertexArray.data(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_indexArray.data(), num_index * sizeof(uint16_t));

				PrimReset();

				bgfx::setVertexBuffer(0, &tvb, 0, num_vertex);
				bgfx::setIndexBuffer(&tib, 0, num_index);

				bgfx::setTexture(0, u_texture.m_handle, tex);

				uint64_t state = 0
				                 | BGFX_STATE_WRITE_RGB
				                 | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				bgfx::setState(state);

				bgfx::submit(ViewIds::GUI, m_programTex->GetHandle(), 0);
			}
			break;
			case C_Text:
			{
				m_text_driver.Render();
			}
			break;
		}

		if (need_flush)
		{
		}
	}
	m_command_queue.Seek(0);
}
