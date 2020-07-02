#include "UI_Backend.h"
#include "View.h"
#include "Render/Shader.h"
#include <tuple>
#include <MemRefFile.h>
#include <spdlog/spdlog.h>

#include <bgfx/bgfx.h>
#include "views.h"


using namespace Render;
using namespace UI;



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


Renderer::Renderer(): m_gamma_correction(false)
{
	encode_sciscors.set_any();
	scissoring_enabled = false;
}

Renderer::~Renderer()
{

}

void Renderer::Init()
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

void Renderer::Rect(glm::aabb2 rect, color col)
{
	m_command_queue.Write(C_RectCol);
	m_command_queue.Write(rect);
	m_command_queue.Write(col);
}


void Renderer::Rect(glm::aabb2 rect, bgfx::TextureHandle texture, glm::aabb2 uv, int8_t rounding)
{
	if (glm::is_overlapping(encode_sciscors, rect))
	{
		m_command_queue.Write(C_RectTex);
		m_command_queue.Write(rect);
		m_command_queue.Write(uv);
		m_command_queue.Write(texture);
	}
}

void Renderer::Text(glm::aabb2 rect, const char* text, size_t len)
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

void Renderer::PushVertex(glm::vec2 p, glm::vec2 uv, color color)
{
	m_vertexArray.emplace_back(p, uv, color);
}

int Renderer::GetGlyphTexture() const
{
	auto r = std::static_pointer_cast<TextBackend>(m_text_backend);
	return 0;// r->m_textureHandle;
}

void Renderer::SetUp(UI::View view_box)
{
	auto prj = glm::ortho(view_box.view_box.minp.x, view_box.view_box.maxp.x, view_box.view_box.maxp.y, view_box.view_box.minp.y);
	bgfx::setViewTransform(ViewIds::GUI, nullptr, &prj[0]);
}

void Renderer::PushScissors(glm::aabb2 box)
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

void Renderer::PopScissors()
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

void Renderer::Draw()
{
	m_command_queue.Write(C_End);
	m_command_queue.Seek(0);
	m_vertexArray.resize(0);
	m_indexArray.resize(0);

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
				m_command_queue.Read(rect);
				m_command_queue.Read(col);

				if (m_gamma_correction)
				{
					col = glm::pow(glm::vec4(col) / 255.0f, glm::vec4(2.2f)) * 255.0f;
				}

				uint16_t vertexIt = m_vertexArray.size();
				m_vertexArray.emplace_back(rect.minp, glm::vec2(0.0, 0.0), col);
				m_vertexArray.emplace_back(glm::vec2({rect.maxp.x, rect.minp.y}), glm::vec2(0.0, 0.0), col);
				m_vertexArray.emplace_back(rect.maxp, glm::vec2(0.0, 0.0), col);
				m_vertexArray.emplace_back(glm::vec2({rect.minp.x, rect.maxp.y}), glm::vec2(0.0, 0.0), col);
				uint16_t indices[] = { vertexIt, uint16_t(vertexIt + 2), uint16_t(vertexIt + 1), vertexIt, uint16_t(vertexIt + 3), uint16_t(vertexIt + 2)};
				m_indexArray.resize(m_indexArray.size() + 6);
				uint16_t* ptr = &*(m_indexArray.end() - 6);
				memcpy(ptr, indices, 6 * sizeof(uint16_t));
			}
			need_flush = true;
			break;

			case C_RectTex:
			{
				glm::aabb2 rect;
				glm::aabb2 uv;
				m_command_queue.Read(rect);
				m_command_queue.Read(uv);
				m_command_queue.Read(tex);
				uint16_t vertexIt = m_vertexArray.size();
				m_vertexArray.emplace_back(rect.minp, uv.minp, 0xFFFFFFFF_c);
				m_vertexArray.emplace_back(glm::vec2({rect.maxp.x, rect.minp.y}), glm::vec2({uv.maxp.x, uv.minp.y}), 0xFFFFFFFF_c);
				m_vertexArray.emplace_back(rect.maxp, uv.maxp, 0xFFFFFFFF_c);
				m_vertexArray.emplace_back(glm::vec2({rect.minp.x, rect.maxp.y}), glm::vec2({uv.minp.x, uv.maxp.y}), 0xFFFFFFFF_c);
				uint16_t indices[] = { vertexIt, uint16_t(vertexIt + 2), uint16_t(vertexIt + 1), vertexIt, uint16_t(vertexIt + 3), uint16_t(vertexIt + 2)};
				m_indexArray.resize(m_indexArray.size() + 6);
				uint16_t* ptr = &*(m_indexArray.end() - 6);
				memcpy(ptr, indices, 6 * sizeof(uint16_t));
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

				m_vertexArray.resize(0);
				m_indexArray.resize(0);

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

				m_vertexArray.resize(0);
				m_indexArray.resize(0);

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
