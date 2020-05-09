#pragma once
#include <inttypes.h>
#include <bgfx/bgfx.h>


namespace Render
{
	struct VarType
	{
		enum Type: uint16_t
		{
			INVALID = 0,
			BYTE,
			UNSIGNED_BYTE,
			SHORT,
			UNSIGNED_SHORT,
			INT,
			UNSIGNED_INT,
			FLOAT,
			SAMPLER_1D,
			SAMPLER_2D,
			SAMPLER_3D,
			SAMPLER_CUBE,
			SAMPLER_1D_SHADOW,
			SAMPLER_2D_SHADOW,
			INT_VEC2,
			FLOAT_VEC2,
			INT_VEC3,
			FLOAT_VEC3,
			INT_VEC4,
			FLOAT_VEC4,
			FLOAT_MAT2,
			FLOAT_MAT3,
			FLOAT_MAT4,
			COUNT
		};

		template<typename T>
		static VarType::Type GetType() { return INVALID; }

		template<typename T>
		static const char* GetTypeName();

		template<VarType::Type T>
		static const char* GetTypeName();

		static bgfx::UniformType::Enum GetBGFXMapping(VarType::Type t);

		static const char* GetTypeName(VarType::Type t);

		static VarType::Type FromBGFXMapping(bgfx::UniformType::Enum t);


		static bool IsInteger(VarType::Type t);
		static bool IsSignedInteger(VarType::Type t);
		static bool IsUnsignedInteger(VarType::Type t);
		static bool IsSampler(VarType::Type t);
	};
}
