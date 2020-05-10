#include "Texture.h"
#include "Render/TextureReaders/PVRReader.h"
#include "Render/GLCompressionTypes.h"
#include <fsal.h>
#include <stdio.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace Render;


Texture::Texture(): m_size({0, 0, 0}), m_mipmap_count(0), m_type(Invalid),  m_compressed(false),
	m_handle({bgfx::kInvalidHandle})
{
}

Texture::~Texture()
{
	if (bgfx::isValid(m_handle))
	{
		bgfx::destroy(m_handle);
	}
}

inline const bgfx::Memory* GetMemRef(IReader::Blob blob)
{
	return bgfx::makeRef(
		blob.data.get(),
		blob.size, [](void* ptr, void* userData)
		{
			delete (std::shared_ptr<uint8_t>*)userData;
		},
		new std::shared_ptr<uint8_t>(blob.data));
}

TexturePtr Texture::LoadTexture(TextureReader reader)
{
	TexturePtr texture = std::make_shared<Texture>();
	texture->m_size = reader.GetSize(0);

	auto decoded = Render::DecodePixelType((uint64_t)reader.GetFormat().pixel_format);
	// int channel_count = decoded.channel_names.size();
	int dimensionality = 3;
	texture->m_type = Texture::Texture_3D;
	if (texture->m_size.z == 1)
	{
		dimensionality = 2;
		texture->m_type = Texture::Texture_2D;
	}
	bool cubemap = reader.GetFaceCount() == 6;
	if (cubemap)
	{
		assert(dimensionality == 2);
		texture->m_type = Texture::Texture_Cube;
	}

	assert(glm::all(glm::greaterThanEqual(texture->m_size, glm::ivec3(0))));
	assert(reader.GetFaceCount() == 1 || reader.GetFaceCount() == 6);

	texture->m_mipmap_count = reader.GetMipmapCount();
	texture->m_compressed = decoded.compressed;

	auto bgfx_format = Render::GetBGFXMappedTypes(reader.GetFormat());

	uint64_t flags = BGFX_SAMPLER_UVW_CLAMP | BGFX_TEXTURE_SRGB;

	switch(texture->m_type)
	{
		case Texture::Texture_2D:
			texture->m_handle = bgfx::createTexture2D(texture->m_size.x, texture->m_size.y, texture->m_mipmap_count > 1, 1, bgfx_format, flags);
			break;
		case Texture::Texture_3D:
			texture->m_handle = bgfx::createTexture3D(texture->m_size.x, texture->m_size.y, texture->m_size.z, texture->m_mipmap_count > 1, bgfx_format, flags);
			break;
		case Texture::Texture_Cube:
			assert(texture->m_size.x == texture->m_size.y);
			texture->m_handle = bgfx::createTextureCube(texture->m_size.x, texture->m_mipmap_count > 1, 1, bgfx_format, flags);
			break;
		default:
			throw utils::runtime_error("Unkown type %d", texture->m_type);
	}

	for (int mipmap = 0; mipmap < reader.GetMipmapCount(); ++mipmap)
	{
		for (int face = 0; face < reader.GetFaceCount(); ++face)
		{
			auto block_size = reader.GetSize(mipmap);
			auto blob = reader.Read(mipmap, face);
			auto size = reader.GetSize(mipmap);

			switch(texture->m_type)
			{
				case Texture::Texture_2D:
					bgfx::updateTexture2D(texture->m_handle, 0, mipmap, 0, 0, size.x, size.y, GetMemRef(blob));
					break;
				case Texture::Texture_3D:
					bgfx::updateTexture3D(texture->m_handle, mipmap, 0, 0, 0, size.x, size.y, size.z, GetMemRef(blob));
					break;
				case Texture::Texture_Cube:
					assert(texture->m_size.x == texture->m_size.y);
					bgfx::updateTextureCube(texture->m_handle, 0, face, mipmap, 0, 0, size.x, size.y, GetMemRef(blob));
					break;
				default:
					throw utils::runtime_error("Unkown type %d", texture->m_type);
			}
		}
	}

	return texture;
}

#ifndef __EMSCRIPTEN__
#include <doctest.h>

TEST_CASE("[Render] PVRReader")
{
	spdlog::info("TexturePtr size: {}", sizeof(TexturePtr));
}
#endif
