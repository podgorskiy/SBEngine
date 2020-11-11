#pragma once
#include <2DEngine/Renderer2D.h>
#include "Render/Texture.h"
#include "Enums.h"


namespace UI
{
	class Block;

	class IEmitter
	{
	public:
		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd) = 0;

		virtual ~IEmitter() = default;
	};

	class SFillEmitter : public IEmitter
	{
	public:
		explicit SFillEmitter(Render::color c);

		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd);

		Render::color col;
	};

	class SImageEmitter : public IEmitter
	{
	public:
		SImageEmitter(Render::TexturePtr tex, ImSize::Enum size, ImPos::Enum pos, ImTransform::Enum t);

		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd);

	private:
		glm::ivec2 image_size;
		Render::TexturePtr tex;
		UI::ImSize::Enum size;
		UI::ImPos::Enum pos;
		ImTransform::Enum t;
	};

	class SShaderEmitter : public IEmitter
	{
	public:
		SShaderEmitter(Render::TexturePtr tex, int shader);

		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd);

	private:
		Render::TexturePtr tex;
		int shader;
	};

	class SShadowEmitter : public IEmitter
	{
	public:
		SShadowEmitter(Render::color col, float size_value, Constraint::Unit size_unit);

		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd);

	private:
		Render::color col;
		float size_value;
		Constraint::Unit size_unit;
	};

	class STextEmitter : public IEmitter
	{
	public:
		STextEmitter(uint8_t f_id, std::string text, float height_value, Constraint::Unit height_unit, Render::color f_color, uint8_t f_style=0, uint8_t f_stroke=0);

		virtual void operator()(Render::Encoder* encoder, const Block*, float time, int flags, float ppd);

	private:
		std::string text;

		Render::color f_color;
		float height_value;
		Constraint::Unit height_unit;

		uint8_t f_id;
		uint8_t f_style;
		uint8_t f_stroke;
	};

	struct EmitterSizeCheck
	{
		enum
		{
			DataSize = 60,
		};

		void check()
		{
			static_assert(DataSize >= sizeof(IEmitter), "");
			static_assert(DataSize >= sizeof(SFillEmitter), "");
			static_assert(DataSize >= sizeof(STextEmitter), "");
			static_assert(DataSize >= sizeof(SImageEmitter), "");
		}
	};
}
