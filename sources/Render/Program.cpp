#include "Shader.h"
#include <stdio.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <type_traits>
#include <map>
#include <fsal.h>


namespace Render
{
	Program::Program(): m_program({bgfx::kInvalidHandle})
	{}

	Program::~Program()
	{
		bgfx::destroy(m_program);
	}

	void Program::CollectUniforms(bgfx::ShaderHandle shader)
	{
		constexpr int max_handles = 128;
		bgfx::UniformHandle handles[max_handles];
		int num = bgfx::getShaderUniforms(shader, handles, max_handles);

		for (int i = 0; i < num; ++i)
		{
			bgfx::UniformInfo info = {{0}, bgfx::UniformType::End, 0};
			bgfx::getUniformInfo(handles[i], info);
			m_uniforms.push_back(handles[i]);
			m_uniformMap[info.name] = static_cast<int>(m_uniforms.size()) - 1;
		}
	}

	void Program::Link(const Shader& shader)
	{
		m_uniforms.clear();
		m_uniformMap.clear();
		CollectUniforms(shader.m_shader);
		m_program = bgfx::createProgram(shader.m_shader);
	}

	void Program::Link(const Shader& vs, const Shader& fs)
	{
		m_uniforms.clear();
		m_uniformMap.clear();
		CollectUniforms(vs.m_shader);
		CollectUniforms(fs.m_shader);
		m_program = bgfx::createProgram(vs.m_shader, fs.m_shader);
	}

	Uniform Program::GetUniform(const char* name)
	{
		std::string n(name);
		auto it = m_uniformMap.find(name);
		if (it != m_uniformMap.end())
		{
			auto handle = m_uniforms[it->second];
			return Uniform(handle);
		}
		return Uniform();
	}

	Uniform::Uniform(bgfx::UniformHandle handle, bool owns): m_handle({bgfx::kInvalidHandle}), m_owns(owns)
	{
		bgfx::UniformInfo info = {{0}, bgfx::UniformType::End, 0};
		bgfx::getUniformInfo(handle, info);
		m_handle = handle;
		m_type = VarType::FromBGFXMapping(info.type);
		m_num = info.num;
	}

	ProgramPtr MakeProgram(const char* vertex_shader, const char* fragment_shader)
	{
		const char* shaderPath = "???";

		switch (bgfx::getRendererType())
		{
		case bgfx::RendererType::Noop:
		case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
		case bgfx::RendererType::Gnm:        shaderPath = "shaders/pssl/";  break;
		case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
		case bgfx::RendererType::Nvn:        shaderPath = "shaders/nvn/";   break;
		case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
		case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
		case bgfx::RendererType::Vulkan:     shaderPath = "shaders/spirv/"; break;

		case bgfx::RendererType::Count:
			throw utils::runtime_error("Unknow backend %s", bgfx::getRendererType());
		}

		std::string vs_path = std::string(shaderPath) + vertex_shader;
		std::string fs_path = std::string(shaderPath) + fragment_shader;
		std::string vs = fsal::FileSystem().Open(vs_path);
		std::string fs = fsal::FileSystem().Open(fs_path);

		ProgramPtr program(new Program);

		program->Link(
				Shader(vertex_shader, (const uint8_t*)vs.data(), vs.size()),
				Shader(fragment_shader, (const uint8_t*)fs.data(), fs.size()));
		return program;
	}

	template<typename T>
	void Uniform::ApplyValue(const T& value) const
	{
		spdlog::error("Unsupported type");
		assert(false);
	}

	template<typename T>
	void Uniform::ApplyValue(const T* value, int count) const
	{
		spdlog::error("Unsupported type");
		assert(false);
	}

	template<>
	void Uniform::ApplyValue<int>(const int& value) const
	{
		assert(VarType::IsSignedInteger(m_type) || VarType::IsSampler(m_type) );
		bgfx::setUniform(m_handle, &value);
	}

	template<>
	void Uniform::ApplyValue<int>(const int* value, int count) const
	{
		assert(VarType::IsSignedInteger(m_type) || VarType::IsSampler(m_type) );
		bgfx::setUniform(m_handle, value, count);
	}

	template<>
	void Uniform::ApplyValue<unsigned int>(const unsigned int& value) const
	{
		assert(VarType::IsUnsignedInteger(m_type) || VarType::IsSampler(m_type) );
		bgfx::setUniform(m_handle, &value);
	}

	template<>
	void Uniform::ApplyValue<unsigned int>(const unsigned int* value, int count) const
	{
		assert(VarType::IsUnsignedInteger(m_type) || VarType::IsSampler(m_type) );
		bgfx::setUniform(m_handle, value, count);
	}

#define ADD_SPEC(TA) \
	template<> \
	void Uniform::ApplyValue<TA>(const TA& value) const \
	{ \
		assert(m_type == VarType::GetType<TA>()); \
		bgfx::setUniform(m_handle, &value); \
	} \
	template<> \
	void Uniform::ApplyValue<TA>(const TA* value, int count) const \
	{ \
		assert(m_type == VarType::GetType<TA>()); \
		bgfx::setUniform(m_handle, value, count); \
	}

#define ADD_SPEC_A(TA) \
	template<> \
	void Uniform::ApplyValue<TA>(const std::vector<TA>& value) const \
	{ \
		assert(m_type == VarType::GetType<TA>()); \
		bgfx::setUniform(m_handle, value.data(), static_cast<int>(value.size())); \
	}

	ADD_SPEC(float)
	ADD_SPEC(glm::vec2)
	ADD_SPEC(glm::vec3)
	ADD_SPEC(glm::vec4)
	ADD_SPEC(glm::quat)

	ADD_SPEC(glm::ivec2)
	ADD_SPEC(glm::ivec3)
	ADD_SPEC(glm::ivec4)

	ADD_SPEC(glm::mat2)
	ADD_SPEC(glm::mat3)
	ADD_SPEC(glm::mat4)

	ADD_SPEC_A(float)
	ADD_SPEC_A(glm::vec2)
	ADD_SPEC_A(glm::vec3)
	ADD_SPEC_A(glm::vec4)
	ADD_SPEC_A(glm::quat)

	ADD_SPEC_A(glm::ivec2)
	ADD_SPEC_A(glm::ivec3)
	ADD_SPEC_A(glm::ivec4)

	ADD_SPEC_A(glm::mat2)
	ADD_SPEC_A(glm::mat3)
	ADD_SPEC_A(glm::mat4)
}

//#ifndef __EMSCRIPTEN__
//#include <doctest.h>
//
//TEST_CASE("[Render] Shaders")
//{
//	SUBCASE("Basic")
//	{
//		const char* vertex_shader_src = R"(
//			attribute vec3 a_position;
//			attribute vec2 a_uv;
//			uniform mat4 u_modelView;
//			uniform mat4 u_projection;
//			varying vec2 v_uv;
//
//			void main()
//			{
//				vec4 pos = u_modelView * vec4(a_position, 1.0);
//				gl_Position = u_projection * pos;
//			}
//		)";
//
//		const char* fragment_shader_src = R"(
//			varying vec2 v_uv;
//			uniform sampler2D u_texture;
//
//			void main()
//			{
//				vec3 color = texture2D(u_texture, v_uv).rgb;
//				color = pow(color, vec3(2.2));
//				gl_FragColor = vec4(pow(color, vec3(1.0/2.2)), 1.0);
//			}
//		)";
//
//		auto program = Render::MakeProgram(vertex_shader_src, fragment_shader_src);
//		CHECK(program);
//
//		auto modelView = program->GetUniformLocation("u_modelView");
//		auto projection = program->GetUniformLocation("u_projection");
//		auto uniform_texture = program->GetUniformLocation("u_texture");
//
//		auto does_not_exist = program->GetUniformLocation("does_not_exist");
//
//		CHECK_NE(modelView, -1);
//		CHECK_NE(projection, -1);
//		CHECK_NE(uniform_texture, -1);
//		CHECK_EQ(does_not_exist, -1);
//
//		program->GetUniform("u_texture").ApplyValue(1);
//		program->GetUniform("u_projection").ApplyValue(glm::mat4(1.0));
//
////		REQUIRE_THROWS({
////			Render::Uniform aa(-1, Render::VarType::BYTE, 1);
////			aa.ApplyValue("!");
////		});
//
//
//	}
//}
//#endif