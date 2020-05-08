#include "Shader.h"
#include "Render/gl_headers.h"
#include <spdlog/spdlog.h>
#include <stdio.h>


namespace Render
{
	void ShaderCstr(GLHandle& handle, SHADER_TYPE::Type T)
	{
		GLuint type;
		switch (T)
		{
			case SHADER_TYPE::COMPUTE_SHADER:
				type = GL_COMPUTE_SHADER;
				break;
			case SHADER_TYPE::GEOMETRY_SHADER:
				type = GL_GEOMETRY_SHADER;
				break;
			case SHADER_TYPE::VERTEX_SHADER:
				type = GL_VERTEX_SHADER;
				break;
			case SHADER_TYPE::FRAGMENT_SHADER:
				type = GL_FRAGMENT_SHADER;
				break;
		}
		handle = glCreateShader(type);
	}

	std::string GetShaderTypeName(SHADER_TYPE::Type T)
	{
		switch (T)
		{
			case SHADER_TYPE::COMPUTE_SHADER:
				return "COMPUTE_SHADER";
			case SHADER_TYPE::GEOMETRY_SHADER:
				return "GEOMETRY_SHADER";
			case SHADER_TYPE::VERTEX_SHADER:
				return "VERTEX_SHADER";
			case SHADER_TYPE::FRAGMENT_SHADER:
				return "FRAGMENT_SHADER";
		}
		return "INVALID_SHADER";
	}

	void ShaderDstr(GLHandle handle)
	{
		glDeleteShader(handle);
	}

	static void PrintSource(const char* source, int startLine)
	{
		int line = startLine;
		while (true)
		{
			const char* start = source;
			while (*source != '\n' && *source != '\0')
			{
				++source;
			}
			std::string l(start, source);
			spdlog::info("{:3d}:\t{}", line++, l);

			if (*source == '\0')
			{
				break;
			}
			++source;
		}
	}

	bool CompileShader(GLHandle& handle, const char* src, SHADER_TYPE::Type T)
	{
		glShaderSource(handle, 1, &src, NULL);

		glCompileShader(handle);
		GLint compiled = 0;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
		GLint infoLen = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			spdlog::warn("{} during {} shader compilation.", compiled == GL_TRUE ? "Warning" : "Error",
			             GetShaderTypeName(T));

			PrintSource(src, 1);

			char* buf = new char[infoLen];
			glGetShaderInfoLog(handle, infoLen, NULL, buf);
			spdlog::warn("Compilation log: {}", buf);
			delete[] buf;
		}
		return compiled == GL_TRUE;
	}
}
