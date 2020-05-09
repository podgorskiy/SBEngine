#include "Types.h"
#include "utils/common.h"
#include <inttypes.h>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>


namespace Render
{
#define DECLARE(Native, Enum) \
	template<> VarType::Type VarType::GetType<Native>() { return VarType::Enum; } \
	template<> const char* VarType::GetTypeName<Native>() { return #Enum; } \
	template<> const char* VarType::GetTypeName<VarType::Enum>() { return #Enum; } \

#define DECLARE_(Enum) \
	template<> const char* VarType::GetTypeName<VarType::Enum>() { return #Enum; } \

	DECLARE(int8_t, BYTE)
	DECLARE(uint8_t, UNSIGNED_BYTE)
	DECLARE(int16_t, SHORT)
	DECLARE(uint16_t, UNSIGNED_SHORT)
	DECLARE(int32_t, INT)
	DECLARE(uint32_t, UNSIGNED_INT)
	DECLARE(float, FLOAT)
	DECLARE_(SAMPLER_1D)
	DECLARE_(SAMPLER_2D)
	DECLARE_(SAMPLER_3D)
	DECLARE_(SAMPLER_CUBE)
	DECLARE_(SAMPLER_1D_SHADOW)
	DECLARE_(SAMPLER_2D_SHADOW)
	DECLARE(glm::ivec2, INT_VEC2)
	DECLARE(glm::vec2, FLOAT_VEC2)
	DECLARE(glm::ivec3, INT_VEC3)
	DECLARE(glm::vec3, FLOAT_VEC3)
	DECLARE(glm::ivec4, INT_VEC4)
	DECLARE(glm::vec4, FLOAT_VEC4)
	DECLARE(glm::mat2, FLOAT_MAT2)
	DECLARE(glm::mat3, FLOAT_MAT3)
	DECLARE(glm::mat4, FLOAT_MAT4)
}

#define CASE(P, ENUM, FUNC) case P(ENUM): return FUNC <P(ENUM)>();

#define _SWITCH(P, FUNC, DEFAULT)\
		switch (t) { \
			CASE(P,BYTE, FUNC);\
			CASE(P,UNSIGNED_BYTE, FUNC);\
			CASE(P,SHORT, FUNC);\
			CASE(P,UNSIGNED_SHORT, FUNC);\
			CASE(P,INT, FUNC);\
			CASE(P,UNSIGNED_INT, FUNC);\
			CASE(P,FLOAT, FUNC);\
			CASE(P,SAMPLER_1D, FUNC);\
			CASE(P,SAMPLER_2D, FUNC);\
			CASE(P,SAMPLER_3D, FUNC);\
			CASE(P,SAMPLER_CUBE, FUNC);\
			CASE(P,SAMPLER_1D_SHADOW, FUNC);\
			CASE(P,SAMPLER_2D_SHADOW, FUNC);\
			CASE(P,INT_VEC2, FUNC);\
			CASE(P,FLOAT_VEC2, FUNC);\
			CASE(P,INT_VEC3, FUNC);\
			CASE(P,FLOAT_VEC3, FUNC);\
			CASE(P,INT_VEC4, FUNC);\
			CASE(P,FLOAT_VEC4, FUNC);\
			CASE(P,FLOAT_MAT2, FUNC);\
			CASE(P,FLOAT_MAT3, FUNC);\
			CASE(P,FLOAT_MAT4, FUNC);\
			default: return DEFAULT; \
		}

#define MAKE_ENUM(X) VarType::X


#define SWITCH(FUNC, DEFAULT) _SWITCH(MAKE_ENUM, FUNC, DEFAULT)


using namespace Render;


bgfx::UniformType::Enum VarType::GetBGFXMapping(VarType::Type t)
{
	switch(t)
	{
		case VarType::SAMPLER_1D:
		case VarType::SAMPLER_2D:
		case VarType::SAMPLER_3D:
		case VarType::SAMPLER_1D_SHADOW:
		case VarType::SAMPLER_2D_SHADOW:
		case VarType::SAMPLER_CUBE: return bgfx::UniformType::Sampler;
		case VarType::FLOAT_VEC4: return bgfx::UniformType::Vec4;
		case VarType::FLOAT_MAT3: return bgfx::UniformType::Mat3;
		case VarType::FLOAT_MAT4: return bgfx::UniformType::Mat4;
		default:
			throw utils::runtime_error("BGFX does not support uniform type: %s", GetTypeName(t));
	}
}

const char* VarType::GetTypeName(VarType::Type t)
{
	SWITCH(GetTypeName, "INVALID")
}

VarType::Type VarType::FromBGFXMapping(bgfx::UniformType::Enum t)
{
	switch(t)
	{
		case bgfx::UniformType::Sampler: return VarType::SAMPLER_2D;
		case bgfx::UniformType::Vec4: return VarType::FLOAT_VEC4;
		case bgfx::UniformType::Mat3: return VarType::FLOAT_MAT3;
		case bgfx::UniformType::Mat4: return VarType::FLOAT_MAT4;
		default:
			throw utils::runtime_error("Unknown BGFX uniform type: %d", t);
	}
}

bool VarType::IsInteger(Render::VarType::Type t)
{
	return ((int)t > 0) && ((int)t < (int)VarType::FLOAT);
}

bool VarType::IsSignedInteger(Render::VarType::Type t)
{
	return t == VarType::BYTE || t == VarType::SHORT || t == VarType::INT;
}

bool VarType::IsUnsignedInteger(Render::VarType::Type t)
{
	return t == VarType::UNSIGNED_BYTE || t == VarType::UNSIGNED_SHORT || t == VarType::UNSIGNED_INT;
}

bool VarType::IsSampler(Render::VarType::Type t)
{
	return t == VarType::SAMPLER_1D || t == VarType::SAMPLER_2D || t == VarType::SAMPLER_3D || t == VarType::SAMPLER_CUBE || t == VarType::SAMPLER_1D_SHADOW || t == VarType::SAMPLER_2D_SHADOW;
}



#ifndef __EMSCRIPTEN__
#include <doctest.h>
#include <spdlog/spdlog.h>

TEST_CASE("GL_TYPES")
{
	for (int i = 0; i < Render::VarType::COUNT; ++i)
	{
		auto type = (Render::VarType::Type)i;

		auto gltype = Render::VarType::GetBGFXMapping(type);
		auto enumtype = Render::VarType::FromBGFXMapping(gltype);

		spdlog::info("{:<25}{:#x}", Render::VarType::GetTypeName(type), gltype);
		CHECK_EQ(enumtype, i);
	}

	CHECK(Render::VarType::IsInteger(Render::VarType::BYTE));
	CHECK(Render::VarType::IsInteger(Render::VarType::UNSIGNED_INT));
	CHECK(!Render::VarType::IsInteger(Render::VarType::FLOAT));
}
#endif
