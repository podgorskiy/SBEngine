#include "Renderer2D.h"
#include "ScriberTextBackend.h"
#include "Commands.h"
#include "Render/Shader.h"
#include "views.h"
#include <spdlog/spdlog.h>
#include <fsal.h>
#include <FileInterface.h>
#include <bgfx/bgfx.h>
#include <yaml-cpp/yaml.h>
#include "utils/string_format.h"


using namespace Render;



Renderer2D::Renderer2D(): m_gamma_correction(false)
{
	scissoring_enabled = false;
}

Renderer2D::~Renderer2D()
{

}

void Renderer2D::Init()
{
    using fs = fsal::FileSystem;
	m_programCol = Render::MakeProgram("vs_gui.bin", "fs_gui.bin");
	m_programTex = Render::MakeProgram("vs_gui.bin", "fs_gui_tex.bin");
	m_programShadow = Render::MakeProgram("vs_gui.bin", "fs_gui_shadow.bin");

	u_texture = m_programTex->GetUniform("u_texture");

	u_resolution = bgfx::createUniform("iResolution", bgfx::UniformType::Vec4, 1);
	u_time = bgfx::createUniform("iTime", bgfx::UniformType::Vec4, 1);

	m_vertexSpec
		.begin()
		.add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
		.end();

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
				auto f = fsal::FileSystem().Open(filename, m);
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

	YAML::Node font_config = YAML::Load(std::string(fs().Open("fonts.yaml")));

	auto typefaces = font_config["typefaces"];

	if (typefaces.IsDefined())
	{
		for (auto t: typefaces)
		{
			auto name = t["name"].as<std::string>();
			auto priority = t["priority"].as<int>();
			auto id = m_text_driver.NewTypeface(name.c_str(), priority);
			m_typeface_pool[name] = id;

			auto fonts = t["fonts"];

			if (fonts.IsDefined())
			{
				for (auto f: fonts)
				{
					auto path = f["path"].as<std::string>();
					auto style = f["style"].as<std::string>();
					Scriber::FontStyle::Enum font_style = Scriber::FontStyle::Regular;
					if (style == "Regular")
						font_style = Scriber::FontStyle::Regular;
					else if (style == "Italic")
						font_style = Scriber::FontStyle::Italic;
					else if (style == "Bold")
						font_style = Scriber::FontStyle::Bold;
					auto index = f["index"].as<int>();

					m_text_driver.AndFontToTypeface(id, path.c_str(), font_style, index);
				}
			}
		}
	}

	YAML::Node shaders_config = YAML::Load(std::string(fs().Open("ui_shaders.yaml")));

	auto shaders = shaders_config["shaders"];

	if (shaders.IsDefined())
	{
		for (auto s: shaders)
		{
			auto name = s["name"].as<std::string>();
			auto program = Render::MakeProgram("vs_gui.bin", utils::format("%s.bin", name.c_str()).c_str());
			m_shader_pool[name] = m_shaders.size();
			m_shaders.push_back(program);
		}
	}
}

std::map<std::string, int> Renderer2D::GetShaderMap()
{
	return m_shader_pool;
}

std::map<std::string, int> Renderer2D::GetTypefaceMap()
{
	return m_typeface_pool;
}

void Renderer2D::SetUp(View view_box)
{
	auto prj = glm::ortho(view_box.view_box.minp.x, view_box.view_box.maxp.x, view_box.view_box.maxp.y, view_box.view_box.minp.y);
	bgfx::setViewTransform(ViewIds::GUI, nullptr, &prj[0]);
}


void Renderer2D::Draw(float time)
{
	auto command_queue =  m_encoder.GetCommandQueue();

	command_queue.Write(C_End);
	command_queue.Seek(0);
	m_mesher.PrimReset();

	bgfx::TextureHandle tex;
	uint8_t  shader = 0;
	glm::vec4 iSize;

	Command cmd;
	while (command_queue.Read(cmd) && cmd != C_End)
	{
		bool need_flush = false;
		switch(cmd)
		{
			case C_RectCol:
			{
				glm::aabb2 rect;
				color col;
				glm::vec4 radius;
				command_queue.Read(rect);
				command_queue.Read(radius);
				command_queue.Read(col);

				if (m_gamma_correction)
				{
					col = glm::pow(glm::vec4(col) / 255.0f, glm::vec4(2.2f)) * 255.0f;
				}

				if (radius == glm::vec4(0))
				{
					m_mesher.PrimRect(rect.minp, rect.maxp, glm::vec2(0.0, 0.0), glm::vec2(0.0, 0.0), col);
				}
				else
				{
					m_mesher.PrimRectRounded(rect.minp, rect.maxp, radius, col);
				}
			}
			need_flush = true;
			break;

			case C_RectTex:
			{
				glm::aabb2 rect;
				glm::aabb2 uv;
				glm::vec4  radius;
				command_queue.Read(rect);
				command_queue.Read(radius);
				command_queue.Read(uv);
				command_queue.Read(tex);

				if (radius == glm::vec4(0))
				{
					m_mesher.PrimRect(rect.minp, rect.maxp, uv.minp, uv.maxp, color(0));
				}
				else
				{

				}
			}
			need_flush = true;
			break;

			case C_RectShadow:
			{
				glm::aabb2 rect;
				glm::vec4  radius;
				glm::vec2 dir;
				color col;
				float size;
				command_queue.Read(rect);
				command_queue.Read(radius);
				command_queue.Read(dir);
				command_queue.Read(col);
				command_queue.Read(size);

				if (radius == glm::vec4(0))
				{
					m_mesher.PrimRectShadow(rect.minp, rect.maxp, dir, col, size);
				}
				else
				{

				}
			}
			need_flush = true;
			break;

			case C_Shader:
			{
				glm::aabb2 rect;
				command_queue.Read(rect);
				command_queue.Read(tex);
				command_queue.Read(shader);
				glm::aabb2 uv(glm::vec2(0.0, 1.0), glm::vec2(1.0, 0.0));
				iSize = glm::vec4(rect.size(), 0.0f, 0.0f);

				m_mesher.PrimRect(rect.minp, rect.maxp, uv.minp, uv.maxp, color(0));
			}
			need_flush = true;
			break;

			case C_RectTexTr:
			{
				glm::aabb2 rect;
				glm::aabb2 uv;
				glm::mat2x3 transoform;
				command_queue.Read(rect);
				command_queue.Read(transoform);
				command_queue.Read(uv);
				command_queue.Read(tex);

				m_mesher.PrimRect(rect.minp, rect.maxp, transoform, uv.minp, uv.maxp, color(0));
			}
			need_flush = true;
			break;

			case C_Text:
			{
				glm::aabb2 rect;
				size_t len;
				uint8_t f_id;
				uint8_t f_size;
				uint8_t f_style;
				uint8_t f_stroke;
				uint32_t f_color;
				command_queue.Read(rect);
				command_queue.Read(f_id);
				command_queue.Read(f_size);
				command_queue.Read(f_style);
				command_queue.Read(f_color);
				command_queue.Read(f_stroke);
				command_queue.Read(len);
				auto ptr = (const char*)command_queue.GetDataPointer() + command_queue.Tell();
				command_queue.Seek(len, fsal::File::CurrentPosition);

				Scriber::FontStyle::Enum style = Scriber::FontStyle::Regular;
				Scriber::Align::Enum align = Scriber::Align::Left;
				Scriber::Deaggregate(f_style, style, align);

				m_text_driver.DrawLabel(ptr, rect.minp.x, rect.minp.y, Scriber::Font(f_id, f_size, style, f_color, f_stroke), align);
			}
			need_flush = true;
			break;
			case C_SetScissors:
			{
				command_queue.Read(current_sciscors);
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
				int num_vertex = m_mesher.vcount();
				int num_index = m_mesher.icount();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_mesher.vptr(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_mesher.iptr(), num_index * sizeof(uint16_t));

				m_mesher.PrimReset();

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
				int num_vertex = m_mesher.vcount();
				int num_index = m_mesher.icount();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_mesher.vptr(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_mesher.iptr(), num_index * sizeof(uint16_t));

				m_mesher.PrimReset();

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
			case C_RectShadow:
			{
				int num_vertex = m_mesher.vcount();
				int num_index = m_mesher.icount();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_mesher.vptr(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_mesher.iptr(), num_index * sizeof(uint16_t));

				m_mesher.PrimReset();

				bgfx::setVertexBuffer(0, &tvb, 0, num_vertex);
				bgfx::setIndexBuffer(&tib, 0, num_index);

				uint64_t state = 0
				                 | BGFX_STATE_WRITE_RGB
				                 | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				bgfx::setState(state);

				bgfx::submit(ViewIds::GUI, m_programShadow->GetHandle(), 0);
			}
			break;
			case C_Shader:
			{
				int num_vertex = m_mesher.vcount();
				int num_index = m_mesher.icount();
				bgfx::TransientVertexBuffer tvb;
				bgfx::TransientIndexBuffer tib;

				bgfx::allocTransientVertexBuffer(&tvb, num_vertex, m_vertexSpec);
				bgfx::allocTransientIndexBuffer(&tib, num_index);

				memcpy(tvb.data, m_mesher.vptr(), num_vertex * sizeof(Vertex));
				memcpy(tib.data, m_mesher.iptr(), num_index * sizeof(uint16_t));

				m_mesher.PrimReset();

				bgfx::setVertexBuffer(0, &tvb, 0, num_vertex);
				bgfx::setIndexBuffer(&tib, 0, num_index);

				if (bgfx::isValid(tex))
					bgfx::setTexture(0, u_texture.m_handle, tex);

				bgfx::setUniform(u_resolution, &iSize, 1);
				glm::vec4 vtime(time);
				bgfx::setUniform(u_time, &vtime, 1);

				uint64_t state = 0
				                 | BGFX_STATE_WRITE_RGB
				                 | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
				bgfx::setState(state);

				bgfx::submit(ViewIds::GUI, m_shaders[shader]->GetHandle(), 0);
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
	command_queue.Seek(0);
}
