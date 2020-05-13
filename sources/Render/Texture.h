#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Render/TextureReaders/IReader.h"
#include <bgfx/bgfx.h>


namespace Render
{
	class Texture;

	typedef std::shared_ptr<Texture> TexturePtr;

	class Texture
	{
		Texture(const Texture&) = delete; // non construction-copyable
		Texture& operator=( const Texture& ) = delete; // non copyable
	public:
		enum TextureType
		{
			Texture_1D,
			Texture_2D,
			Texture_3D,
			Texture_Cube,
			Invalid,
		};

		Texture();

		~Texture();

		static TexturePtr LoadTexture(TextureReader reader);

		bgfx::TextureHandle m_handle;

		glm::ivec3 GetSize() { return m_size; }
		TextureType GetType() { return m_type; }
	private:
		glm::ivec3 m_size;
		int m_mipmap_count;
		TextureType m_type;
		bool m_compressed;
	};
}
