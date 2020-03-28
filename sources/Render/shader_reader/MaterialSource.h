#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>


struct MaterialSource
{
	enum ExecutionModel
	{
	    ExecutionModelVertex = 0,
	    ExecutionModelGeometry = 3,
	    ExecutionModelFragment = 4,
	    ExecutionModelGLCompute = 5,
	};

	struct Property
	{
		enum PropertyType
		{
			Float,
			Int,
			Range,
			Color,
			Vector,
			Sampler2D,
			SamplerCube,
			Sampler3D,
		};

		std::string name;
		PropertyType type;

		Property(std::string name, int x): name(std::move(name)), type(Int), int_value(x) {}
		Property(std::string name, float x): name(std::move(name)), type(Float), float_value(x) {}
		Property(std::string name, float x, float low, float high): name(std::move(name)), type(Range), range({x, low, high}) {}
		Property(std::string name, glm::vec4 x, PropertyType type): name(std::move(name)), type(type), vec4_value(x) {}
		Property(std::string name, PropertyType type): name(std::move(name)), type(type) {}

		union
		{
			int int_value;
			float float_value;
			struct {
				float value;
				float low;
				float high;
			} range;
			glm::vec4 vec4_value;
		};
	};

	std::string name;
	std::map<ExecutionModel, std::string> m_sources;
	std::vector<Property> properties;
};

template<>
struct fmt::formatter<MaterialSource::ExecutionModel>: fmt::formatter<string_view>
{
	template<typename FormatContext>
	auto format(const MaterialSource::ExecutionModel& m, FormatContext& ctx)
	{
		switch (m)
		{
			case MaterialSource::ExecutionModelVertex:
				return fmt::formatter<string_view>::format("ExecutionModelVertex", ctx);
			case MaterialSource::ExecutionModelGeometry:
				return fmt::formatter<string_view>::format("ExecutionModelGeometry", ctx);
			case MaterialSource::ExecutionModelFragment:
				return fmt::formatter<string_view>::format("ExecutionModelFragment", ctx);
			case MaterialSource::ExecutionModelGLCompute:
				return fmt::formatter<string_view>::format("ExecutionModelGLCompute", ctx);
			default:
				return fmt::formatter<string_view>::format("Unknown", ctx);
		}
	}
};

template<>
struct fmt::formatter<MaterialSource::Property>: fmt::formatter<string_view>
{
	template<typename FormatContext>
	auto format(const MaterialSource::Property& v, FormatContext& ctx)
	{
		switch (v.type)
		{
			case MaterialSource::Property::Float:
				return fmt::format_to(ctx.out(), "\t{}: {}", v.name, v.float_value);
			case MaterialSource::Property::Int:
				return fmt::format_to(ctx.out(), "\t{}: {}", v.name, v.int_value);
			case MaterialSource::Property::Range:
				return fmt::format_to(ctx.out(), "\t{}: {} ({}, {})", v.name, v.range.value, v.range.low, v.range.high);
			case MaterialSource::Property::Color:
			case MaterialSource::Property::Vector:
				return fmt::format_to(ctx.out(), "\t{}: [{}, {}, {}, {}]", v.name, v.vec4_value.x, v.vec4_value.y, v.vec4_value.z, v.vec4_value.w);
			default:
				return fmt::formatter<string_view>::format("\tUnknown", ctx);
		}
	}
};

template<>
struct fmt::formatter<MaterialSource>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx){return ctx.begin();}

	template<typename FormatContext>
	auto format(const MaterialSource& m, FormatContext& ctx)
	{
		std::vector<std::pair<MaterialSource::ExecutionModel, std::string> > sources(m.m_sources.begin(), m.m_sources.end());
		return fmt::format_to(ctx.out(), "{}:\nProperties:\n{}\nSources:\n{}", m.name, fmt::join(m.properties, "\n "), fmt::join(sources, "\n "));
	}
};

template<>
struct fmt::formatter<std::pair<MaterialSource::ExecutionModel, std::string> >: fmt::formatter<string_view>
{
	template<typename FormatContext>
	auto format(const std::pair<MaterialSource::ExecutionModel, std::string>& v, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "\tModel:{}\n\tSource:\n\n{}", v.first, v.second);
	}
};
