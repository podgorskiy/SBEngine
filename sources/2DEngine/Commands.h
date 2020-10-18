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
}
