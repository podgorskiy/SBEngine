#pragma once

#include <glm/glm.hpp>


namespace UI
{
	typedef glm::vec<4, uint8_t> color;
	typedef glm::vec<4, uint16_t> color16;

	inline color operator "" _c(unsigned long long c)
	{
		uint32_t x = c;
		return (color&) (x);
	}

	inline color operator/(const color16& c, int d)
	{
		return color(c[0] / d, c[1] / d, c[2] / d, c[3] / d);
	}

	inline color operator*(const color& ca, const color& cb)
	{
		return (color16(ca) * color16(cb)) / 255;
	}
}
