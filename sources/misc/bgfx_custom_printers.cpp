#pragma once
#include "bgfx_custom_printers.h"
#include "utils/string_format.h"
#include <bgfx/bgfx.h>
#include <spdlog/spdlog.h>
#include <mutex>


void CustomPrinters::fatal(
			  const char* _filePath
			, uint16_t _line
			, bgfx::Fatal::Enum _code
			, const char* _str)
{
	const char* code = "";
	switch(_code)
	{
		case bgfx::Fatal::DebugCheck: code = "DebugCheck"; break;
		case bgfx::Fatal::InvalidShader: code = "InvalidShader"; break;
		case bgfx::Fatal::UnableToInitialize: code = "UnableToInitialize"; break;
		case bgfx::Fatal::UnableToCreateTexture: code = "UnableToCreateTexture"; break;
		case bgfx::Fatal::DeviceLost: code = "DeviceLost"; break;
	}
	spdlog::error("Fatal error ({}): {} ({}): {}", code, _filePath, _line, _str);
}

void CustomPrinters::traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList)
{
	int final_n, n = ((int) strlen(_format)) * 2;
	static std::mutex m;
	m.lock();

	static char* formatted = nullptr;
	static int size = 0;

	while (true)
	{
		va_list ap_copy;
		va_copy(ap_copy, _argList);

		if (n > size)
		{
			size = n;
			formatted = (char*)realloc(formatted, size);
		}

		strcpy(formatted, _format);
		final_n = vsnprintf(formatted, n, _format, ap_copy);
		if (formatted[final_n - 1] == '\n')
		{
			formatted[final_n - 1] = '\0';
		}

		if (final_n < 0 || final_n >= n)
		{
			n += abs(final_n - n + 1);
		}
		else
		{
			break;
		}
	}
	int max_path_len = 32;
	int len_path = strlen(_filePath);

	if (len_path > max_path_len)
	{
		spdlog::info("...{} ({:6}): {}", _filePath + std::max((len_path - max_path_len + 3), 0), _line, formatted);
	}
	else
	{
		spdlog::info("{} ({:6}): {}", _filePath, _line, formatted);
	}

	m.unlock();
}

uint32_t CustomPrinters::CustomPrinters::cacheReadSize(uint64_t _id)
{
	return 0;
}

bool CustomPrinters::cacheRead(uint64_t _id, void* _data, uint32_t _size)
{
	return false;
}

void CustomPrinters::cacheWrite(uint64_t _id, const void* _data, uint32_t _size)
{}

void CustomPrinters::screenShot(
		const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size,
		bool _yflip
)
{}

void CustomPrinters::captureBegin(
		uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx::TextureFormat::Enum _format, bool _yflip
)
{}

void CustomPrinters::captureEnd()
{}

void CustomPrinters::captureFrame(const void* _data, uint32_t _size)
{}
