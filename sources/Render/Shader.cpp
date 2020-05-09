#include "Shader.h"
#include "Render/gl_headers.h"
#include <spdlog/spdlog.h>
#include <stdio.h>


namespace Render
{
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
}
