#pragma once
#include <string>
#include <stdarg.h>
#include <string.h>
#include <spdlog/spdlog.h>


namespace ShaderReader
{
	class ErrorHandler
	{
	public:
		ErrorHandler(): m_error(false), m_pointer(0)
		{
			memset(m_errorBuffer, 0, bufferSize);
		}

		bool HasErrorOccurred()
		{
			return m_error;
		}

		std::string GetErrorMessage()
		{
			return m_errorBuffer;
		}

		void ResetError()
		{
			m_error = false;
			m_errorBuffer[0] = '\0';
			m_pointer = 0;
		}

	protected:
		void SetError(const char* fmt, ...)
		{
			if (!m_error)
			{
				va_list args;
				va_start(args, fmt);
				m_pointer += vsprintf(m_errorBuffer + m_pointer, fmt, args);
				va_end(args);
				m_error = true;
                spdlog::error("{}", m_errorBuffer);
			}
		}

	private:
		enum BufferSize
		{
			bufferSize = 2048
		};
		char m_errorBuffer[bufferSize];
		bool m_error;
		int m_pointer;
	};
}
