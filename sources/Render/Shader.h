#pragma once
#include "Types.h"
#include "utils/common.h"
#include <vector>
#include <bgfx/bgfx.h>
#include <array>
#include <map>
#include <memory>


namespace Render
{
	class Shader
	{
		friend class Program;
	public:
		Shader(const Shader& other) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader(const char* name, const uint8_t* data, size_t size): m_shader(bgfx::createShader(bgfx::copy(data, size)))
		{
			bgfx::setName(m_shader, name);
		}

		~Shader()
		{
			bgfx::destroy(m_shader);
		}

	private:
		bgfx::ShaderHandle m_shader;
	};

	class Uniform
	{
	public:
		Uniform(const Uniform& other): m_owns(false)
		{
			if (other.m_owns)
				throw utils::runtime_error("Can't copy uniform if it owns handle. Use UniformPtr");
			m_handle = other.m_handle;
			m_type = other.m_type;
			m_num = other.m_num;
		};

		Uniform& operator=(const Uniform& other)
		{
			if (other.m_owns)
				throw utils::runtime_error("Can't copy uniform if it owns handle. Use UniformPtr");
			m_handle = other.m_handle;
			m_type = other.m_type;
			m_num = other.m_num;
			m_owns = false;
			return *this;
		};

		Uniform(): m_handle({bgfx::kInvalidHandle}), m_type(VarType::INVALID), m_num(0), m_owns(false) {}
		explicit Uniform(bgfx::UniformHandle handle, bool owns=false);

		~Uniform()
		{
			if (m_owns && valid())
				bgfx::destroy(m_handle);
		}

		bool valid() const { return bgfx::isValid(m_handle); }

		VarType::Type type() const { return m_type; };

		int num() const { return m_num; };

		template<typename T>
		void ApplyValue(const T& value) const;

		template<typename T>
		void ApplyValue(const T* value, int count = 1) const;

		template<typename T>
		void ApplyValue(const std::vector<T>& value) const;

		bgfx::UniformHandle m_handle;
	protected:
		VarType::Type m_type;
		uint16_t m_num;
		bool m_owns;
	};

	typedef std::shared_ptr<Uniform> UniformPtr;

	template<typename T>
	UniformPtr MakeUniform(const char* name, int num=1)
	{
		auto type = VarType::GetType<T>();
		auto bgfxtype = VarType::GetBGFXMapping(type);
		auto handle = bgfx::createUniform(name, bgfxtype, num);
		return std::make_shared<Uniform>(handle, true);
	}

	class Program
	{
	public:
		Program(const Program& other) = delete;
		Program& operator=(const Program&) = delete;

		Program();
		~Program();

		void Link(const Shader& shader);

		void Link(const Shader& vs, const Shader& fs);

		Uniform GetUniform(const char* name);

		Uniform GetUniform(int id) const { return Uniform(m_uniforms[id]); }

		const auto& UniformMap() const { return m_uniformMap; }

		bgfx::ProgramHandle GetHandle() const { return m_program; }
	private:
		void CollectUniforms(bgfx::ShaderHandle shader);

		std::vector<bgfx::UniformHandle> m_uniforms;
		std::map<std::string, uint16_t> m_uniformMap;

		bgfx::ProgramHandle m_program;
	};


	typedef std::shared_ptr<Program> ProgramPtr;

	ProgramPtr MakeProgram(const char* vertex_shader, const char* fragment_shader);
}
