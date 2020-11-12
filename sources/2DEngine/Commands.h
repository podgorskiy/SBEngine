#pragma once
#include <inttypes.h>

namespace Render
{
	enum Command : uint8_t
	{
		C_RectCol,
		C_RectTex,
		C_RectTexTr,
		C_RectShadow,
		C_Text,
		C_SetScissors,
		C_ResetScissors,
		C_Shader,
		C_End,
	};

	inline uint64_t Pack(Render::Command cmd, int depth, int start, int size)
	{
		return (uint64_t(uint64_t(depth) + 0x7FFFFFFFULL)) << 32ULL |
		(start & 0xFFFFULL) << 16ULL |
		(cmd & 0xFFULL) << 8ULL |
		(size & 0xFFULL);
	}

	inline void UnPack(uint64_t in, Render::Command& cmd, int& depth, int& start, int& size)
	{
		size = in & 0xFFULL;
		in = in >> 8ULL;
		cmd = (Render::Command)(in & 0xFFULL);
		in = in >> 8ULL;
		start = in & 0xFFFFULL;
		in = in >> 16ULL;
		depth = int(in & 0xFFFFULL) - 0x7FFFFFFF;
	}
}
