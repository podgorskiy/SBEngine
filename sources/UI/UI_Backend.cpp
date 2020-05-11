#include "UI_Backend.h"
#include "Render/Shader.h"
#include "Render/gl_headers.h"
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
			.add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
			.end();

		u_texture = m_program->GetUniform("u_texture");
		u_stroke = m_program->GetUniform("u_stroke");

		ibh = bgfx::createDynamicIndexBuffer(1024 * 1024);
		vbh = bgfx::createDynamicVertexBuffer(1024 * 1024, m_vertexSpec);
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
		float tmpbuffer[1024 * 24];

		uint8_t* ptr = (uint8_t*)tmpbuffer;

		for (int i = 0; i < vertex_count; ++i)
		{
			((float*)(ptr + i * (4 * 4 +4)))[0] = vertexBuffer[i].pos.x;
			((float*)(ptr + i * (4 * 4 +4)))[1] = vertexBuffer[i].pos.y;
			((float*)(ptr + i * (4 * 4 +4)))[2] = vertexBuffer[i].uv.x;
			((float*)(ptr + i * (4 * 4 +4)))[3] = vertexBuffer[i].uv.y;
			((float*)(ptr + i * (4 * 4 +4)))[4] = *(float*)&vertexBuffer[i].color;
		}

		bgfx::update(
				ibh, 0, bgfx::copy(indexBuffer, primitiveCount * sizeof(short) * 3)
		);

		bgfx::update(
				vbh, 0,bgfx::copy(ptr, vertex_count * (4 * 4 + 4))
		);

		bgfx::setVertexBuffer(0, vbh, 0, vertex_count);
		bgfx::setIndexBuffer(ibh, 0, primitiveCount * 3);

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
	bgfx::DynamicIndexBufferHandle ibh;
	bgfx::DynamicVertexBufferHandle vbh;
};


Renderer::Renderer()
{
}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
//	const char* vertex_shader_src = R"(#version 300 es
//		in vec2 a_position;
//		in vec2 a_uv;
//		in vec4 a_color;
//
//		uniform mat4 u_transform;
//
//		out vec4 v_color;
//
//		void main()
//		{
//			v_color = a_color;
//			gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
//		}
//	)";
//
//	const char* fragment_shader_src = R"(#version 300 es
//		precision mediump float;
//		in vec4 v_color;
//		out vec4 color;
//
//		void main()
//		{
//			color = v_color;
//		}
//	)";

//  m_program = Render::MakeProgram(vertex_shader_src, fragment_shader_src);
//
//	m_vertexSpec = Render::VertexSpecMaker()
//			.PushType<glm::vec2>("a_position")
//			.PushType<glm::vec2>("a_uv")
//			.PushType<glm::vec<4, uint8_t> >("a_color", true);
//
//	m_vertexSpec.CollectHandles(m_program);
//
//	m_uniform_transform = m_program->GetUniformLocation("u_transform");

	m_command_queue = fsal::File(new fsal::MemRefFile());

//	glGenBuffers(1, &m_indexBufferHandle);
//	glGenBuffers(1, &m_vertexBufferHandle);

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

void Renderer::Rect(glm::aabb2 rect, glm::aabb2 uv, int8_t rounding)
{
	m_command_queue.Write(C_Rect);
	m_command_queue.Write(rounding);
	m_command_queue.Write(rect);
	m_command_queue.Write(uv);
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

void Renderer::Draw()
{
	m_command_queue.Write(C_End);
	m_command_queue.Seek(0);
	Command cmd;
	while (m_command_queue.Read(cmd) && cmd != C_End)
	{
		switch(cmd)
		{
			case C_Rect:
			{
				glm::aabb2 rect;
				glm::aabb2 uv;
				int8_t rounding;
				m_command_queue.Read(rounding);
				m_command_queue.Read(rect);
				m_command_queue.Read(uv);
				int vertexIt = m_vertexArray.size();
				m_vertexArray.emplace_back(rect.minp, uv.minp, color(0, 0, 0, 230));
				m_vertexArray.emplace_back(glm::vec2({rect.maxp.x, rect.minp.y}), glm::vec2({uv.maxp.x, uv.minp.y}), color(0, 0, 0, 230));
				m_vertexArray.emplace_back(rect.maxp, uv.maxp, color(0, 0, 0, 230));
				m_vertexArray.emplace_back(glm::vec2({rect.minp.x, rect.maxp.y}), glm::vec2({uv.minp.x, uv.maxp.y}), color(0, 0, 0, 230));
				int indices[] = { vertexIt, vertexIt + 2, vertexIt + 1, vertexIt, vertexIt + 3, vertexIt + 2};
				m_indexArray.resize(m_indexArray.size() + 6);
				int* ptr = &*(m_indexArray.end() - 6);
				memcpy(ptr, indices, 6 * sizeof(int));
			}
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
			break;
		}
	}
	m_command_queue.Seek(0);
//
//	GLint id;
//	glGetIntegerv(GL_CURRENT_PROGRAM, &id);
//
//	glEnable(GL_BLEND);
//	glDisable(GL_DEPTH_TEST);
//	glDisable(GL_SCISSOR_TEST);
//	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
//	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
//
////	m_program->Use();
//	glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &m_prj[0][0]);
//
//	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);
//
//	m_vertexSpec.Enable();
//
//	glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size() * sizeof(Vertex), m_vertexArray.data(), GL_DYNAMIC_DRAW);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size() * sizeof(int), m_indexArray.data(), GL_DYNAMIC_DRAW);
//
//	if (m_vertexArray.size() > 2)
//	{
//		glDrawElements(GL_TRIANGLES, (GLsizei)m_indexArray.size(), GL_UNSIGNED_INT, 0); //m_indexArray.data());
//	}
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	m_vertexSpec.Disable();

	auto r = std::static_pointer_cast<TextBackend>(m_text_backend);
	m_text_driver.Render();

	m_vertexArray.resize(0);
	m_indexArray.resize(0);
}
