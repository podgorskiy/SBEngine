#include "UI_Backend.h"
#include "Render/Shader.h"
#include <tuple>
#include <MemRefFile.h>
#include <GL/gl3w.h>
#include <spdlog/spdlog.h>


using namespace Render;
using namespace UI;

class TextBackend: public Scriber::IRenderAPI
{
public:
	TextBackend(): m_textureHandle(0)
	{
		int size = 1024;
		glGenTextures(1, &m_textureHandle);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, size, size, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		const char* text_vertex_shader_src = R"(
			attribute vec2 a_position;
			attribute vec2 a_uv;
			attribute vec4 a_color;

			uniform mat4 u_transform;

			varying vec4 v_color;
			varying vec2 v_uv;

			void main()
			{
				v_color = a_color;
				v_uv = a_uv;
				gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
			}
		)";

		const char* text_fragment_shader_src = R"(
			varying vec4 v_color;
			varying vec2 v_uv;
			uniform sampler2D u_texture;

			void main()
			{
				vec2 color = texelFetch(u_texture, ivec2(v_uv), 0).rg;

				vec4 color_strock = vec4(vec3(0.0, 0.0, 0.0), color.g * v_color.a);
				vec4 color_fill = vec4(v_color.rgb, color.r * v_color.a);

				// backward blending order
				float alpha = color_strock.a - color_strock.a * color_fill.a + color_fill.a;

				vec4 final = vec4((color_strock.rgb * color_strock.a * (1.0 - color_fill.a) + color_fill.rgb * color_fill.a) / alpha, alpha);

				gl_FragColor = final;
			}
		)";

		m_program = Render::MakeProgram(text_vertex_shader_src, text_fragment_shader_src);

		m_vertexSpec = Render::VertexSpecMaker()
				.PushType<glm::vec<2, int16_t> >("a_position")
				.PushType<glm::vec<2, uint16_t> >("a_uv")
				.PushType<glm::vec<4, uint8_t> >("a_color", true);

		m_vertexSpec.CollectHandles(m_program);

		glGenBuffers(1, &m_indexBufferHandle);
		glGenBuffers(1, &m_vertexBufferHandle);

		u_transform = m_program->GetUniform("u_transform");
		u_texture = m_program->GetUniform("u_texture");
	}

	void SaveTextureToFile() override {};

	void UpdateTexture(Scriber::Image glyph, Scriber::u16vec2 pos) override
	{
		Scriber::ivec2 p(pos);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureHandle);

		GLint oldUnpackAlignment;
		glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexSubImage2D(GL_TEXTURE_2D, 0, p.x, p.y, glyph.GetSize().x, glyph.GetSize().y, GL_RG, GL_UNSIGNED_BYTE, glyph.ptr<const uint8_t*>(0));

		glBindTexture(GL_TEXTURE_2D, 0);

		if (oldUnpackAlignment != 1)
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpackAlignment);
		}
	}

	void ClearTexture() override
	{
		char data[2] = { 0 };
		glClearTexImage(GL_TEXTURE_2D, 0, GL_RG8, GL_UNSIGNED_BYTE, &data);
	}

	void Render(Scriber::Vertex* vertexBuffer, uint16_t* indexBuffer, uint16_t vertex_count, uint16_t primitiveCount) override
	{
		m_program->Use();

		u_transform.ApplyValue(m_transform);
		u_texture.ApplyValue(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureHandle);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);

		m_vertexSpec.Enable();

		if (primitiveCount > 0)
		{
			glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Scriber::Vertex), vertexBuffer, GL_DYNAMIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitiveCount * 3 * sizeof(uint16_t), indexBuffer, GL_DYNAMIC_DRAW);
			glDrawElements(GL_TRIANGLES, (GLsizei)primitiveCount * 3, GL_UNSIGNED_SHORT, nullptr); //m_indexArray.data());
		}

		m_vertexSpec.Disable();
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	unsigned int m_textureHandle;
	Render::ProgramPtr m_program;
	Render::VertexSpec m_vertexSpec;
	glm::mat4 m_transform;
	Render::Uniform u_transform;
	Render::Uniform u_texture;
	uint32_t m_vertexBufferHandle;
	uint32_t m_indexBufferHandle;
};


Renderer::Renderer()
{
}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
	const char* vertex_shader_src = R"(
		attribute vec2 a_position;
		attribute vec2 a_uv;
		attribute vec4 a_color;

		uniform mat4 u_transform;

		varying vec4 v_color;

		void main()
		{
			v_color = a_color;
			gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
		}
	)";

	const char* fragment_shader_src = R"(
		varying vec4 v_color;

		void main()
		{
			gl_FragColor = v_color;
		}
	)";

	m_program = Render::MakeProgram(vertex_shader_src, fragment_shader_src);

	m_vertexSpec = Render::VertexSpecMaker()
			.PushType<glm::vec2>("a_position")
			.PushType<glm::vec2>("a_uv")
			.PushType<glm::vec<4, uint8_t> >("a_color", true);

	m_vertexSpec.CollectHandles(m_program);

	m_uniform_transform = m_program->GetUniformLocation("u_transform");

	m_command_queue = fsal::File(new fsal::MemRefFile());

	glGenBuffers(1, &m_indexBufferHandle);
	glGenBuffers(1, &m_vertexBufferHandle);

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

	m_fs.PushSearchPath("resources");
	m_fs.PushSearchPath("../resources");
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
	return r->m_textureHandle;
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
				m_text_driver.DrawLabel(ptr, rect.minp.x, rect.minp.y, Scriber::Font(0, 16, Scriber::FontStyle::Regular, 0x8FFFFFFF));
			}
			break;
		}
	}
	m_command_queue.Seek(0);

	GLint id;
	glGetIntegerv(GL_CURRENT_PROGRAM, &id);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	m_program->Use();
	glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &m_prj[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);

	m_vertexSpec.Enable();

	glBufferData(GL_ARRAY_BUFFER, m_vertexArray.size() * sizeof(Vertex), m_vertexArray.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexArray.size() * sizeof(int), m_indexArray.data(), GL_DYNAMIC_DRAW);

	if (m_vertexArray.size() > 2)
	{
		glDrawElements(GL_TRIANGLES, (GLsizei)m_indexArray.size(), GL_UNSIGNED_INT, 0); //m_indexArray.data());
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_vertexSpec.Disable();

	auto r = std::static_pointer_cast<TextBackend>(m_text_backend);
	r->m_transform = m_prj;
	m_text_driver.Render();

	glUseProgram(id);

	m_vertexArray.resize(0);
	m_indexArray.resize(0);
}
