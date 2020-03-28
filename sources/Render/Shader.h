#pragma once
#include "Types.h"
#include <vector>
#include <map>
#include <memory>


namespace Render
{
	typedef unsigned int GLHandle;
	struct SHADER_TYPE
	{
		enum Type
		{
			VERTEX_SHADER,
			GEOMETRY_SHADER,
			FRAGMENT_SHADER,
			COMPUTE_SHADER,
		};
	};

	template<SHADER_TYPE::Type T>
	class Shader
	{
		Shader(const Shader& other) = delete;
		Shader& operator=(const Shader&) = delete;
		friend class Program;
	public:
		Shader();
		~Shader();

		bool CompileShader(const char* src);

	private:
		GLHandle m_shader;
	};


	class Uniform
	{
	public:
		Uniform(int handle, VarType::Type type, int num): m_handle(handle), m_type(type), m_num(num)
		{};

		Uniform(const Uniform& other) = default;

		bool valid() const { return m_handle != -1; }

		VarType::Type type() const { return m_type; };

		int num() const { return m_num; };

		template<typename T>
		void ApplyValue(const T& value) const;

		template<typename T>
		void ApplyValue(const std::vector<T>& value) const;

	protected:
		const int m_handle;
		const VarType::Type m_type;
		const int m_num;
	};


	class Program
	{
	public:
		Program(const Program& other) = delete;
		Program& operator=(const Program&) = delete;

		Program();
		~Program();

		bool Link(const Shader<SHADER_TYPE::COMPUTE_SHADER>& shader);

		bool Link(const Shader<SHADER_TYPE::VERTEX_SHADER>& vs, const Shader<SHADER_TYPE::FRAGMENT_SHADER>& fs);

		bool Link(const Shader<SHADER_TYPE::VERTEX_SHADER>& vs, const Shader<SHADER_TYPE::GEOMETRY_SHADER>& gs, const Shader<SHADER_TYPE::FRAGMENT_SHADER>& fs);

		void Use() const;

		int GetAttribLocation(const char *name) const;

		int GetUniformLocation(const char *name) const;

	private:
		bool LinkImpl();
		void InitUniforms();

		std::vector<Uniform> m_uniforms;

		GLHandle m_program;
	};

	typedef std::shared_ptr<Program> ProgramPtr;

	ProgramPtr MakeProgram(const char* vertex_shader, const char* fragment_shader);
}

