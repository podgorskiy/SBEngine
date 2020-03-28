#pragma once
#include "Shader.h"
#include <GL/gl3w.h>
#include <vector>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>


namespace Render
{
	class VertexSpec
	{
	public:
		struct Attribute
		{
			std::string name;
			int32_t components = 0;
			uint32_t type = 0;
			uint8_t normalized = 0;
			int32_t stride = 0;
			int32_t offset = 0;
			uint32_t handle = 0;
		};

		VertexSpec() = default;

		explicit VertexSpec(const std::vector<Attribute>& attributes)
		{
			int i = 0;
			for (const auto& a: attributes)
			{
				m_attributes[i++] = a;
			}
		}

		void CollectHandles(const ProgramPtr& program)
		{
			for (int i = 0; m_attributes[i].components != 0; ++i)
			{
				Attribute& attr = m_attributes[i];
				attr.handle = program->GetAttribLocation(attr.name.c_str());
			}
		}

		void Enable()
		{
			for (int i = 0; m_attributes[i].components != 0; ++i)
			{
				Attribute& attr = m_attributes[i];
				glEnableVertexAttribArray(attr.handle);
				auto offset_ = static_cast<size_t>(attr.offset);
				glVertexAttribPointer(attr.handle, attr.components, attr.type, attr.normalized, attr.stride,
				                      (void*) offset_);
			}
		}

		void Disable()
		{
			for (int i = 0; m_attributes[i].components != 0; ++i)
			{
				Attribute& attr = m_attributes[i];
				glDisableVertexAttribArray(attr.handle);
			}
		}

		Attribute m_attributes[16];
	};
}
