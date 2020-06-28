#pragma once
#include "utils/aabb.h"
#include <inttypes.h>


namespace UI
{
	struct View
	{
		explicit View(glm::aabb2 view_box, int dpi=72): view_box(view_box), dpi(dpi){}
		explicit View(glm::vec2 view_box, int dpi=72): view_box(glm::vec2(0, 0), view_box), dpi(dpi){}

		float GetAspect() const
		{
			return view_box.size().x / float(view_box.size().y);
		}

		float GetPixelPerDotScalingFactor() const
		{
			return dpi / 72.0f;
		}
		
		glm::aabb2 view_box;
		uint16_t dpi = 72;
	};
}
