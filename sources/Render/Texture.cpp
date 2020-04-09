#include "Texture.h"
#include "Render/TextureReaders/PVRReader.h"
#include "Render/GLCompressionTypes.h"
#include <GL/gl3w.h>
#include <fsal.h>
#include <stdio.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace Render;

Texture::Texture(): header({{0, 0, 0}, 0, false, false, Invalid}), m_textureHandle(uint32_t(-1))
{
	glGenTextures(1, &m_textureHandle);
}


void Texture::Bind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_textureHandle);
}

void Texture::BindCube(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureHandle);
}

void Texture::UnBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	if (m_textureHandle != uint32_t(-1))
	{
		glDeleteTextures(1, &m_textureHandle);
		m_textureHandle = -1;
	}
}

TexturePtr Texture::LoadTexture(TextureReader reader)
{
	TexturePtr texture = std::make_shared<Texture>();
	texture->header.size = reader.GetSize(0);

	auto decoded = Render::DecodePixelType((uint64_t)reader.GetFormat().pixel_format);
	// int channel_count = decoded.channel_names.size();
	int dimensionality = 3;
	texture->header.type = Texture::Texture_3D;
	if (texture->header.size.z == 1)
	{
		dimensionality = 2;
		texture->header.type = Texture::Texture_2D;
	}
	if (texture->header.size.y == 1)
	{
		dimensionality = 1;
		texture->header.type = Texture::Texture_1D;
	}
	texture->header.cubemap = reader.GetFaceCount() == 6;
	if (texture->header.cubemap)
	{
		assert(dimensionality == 2);
		texture->header.type = Texture::Texture_Cube;
	}

	assert(glm::all(glm::greaterThanEqual(texture->header.size, glm::ivec3(0))));
	assert(reader.GetFaceCount() == 1 || reader.GetFaceCount() == 6);

	texture->header.MIPMapCount = reader.GetMipmapCount();
	texture->header.cubemap = reader.GetFaceCount() == 6;
	texture->header.compressed = decoded.compressed;

	if (texture->header.cubemap)
	{
		texture->BindCube(0);
	}
	else
	{
		texture->Bind(0);
	}

	uint32_t gltextype = 0;
	switch(texture->header.type)
	{
		case Texture::Texture_1D:
			gltextype = GL_TEXTURE_1D;
			break;
		case Texture::Texture_2D:
			gltextype = GL_TEXTURE_2D;
			break;
		case Texture::Texture_3D:
			gltextype = GL_TEXTURE_3D;
			break;
		case Texture::Texture_Cube:
			gltextype = GL_TEXTURE_CUBE_MAP;
			break;
	}

	auto glformat = Render::GetGLMappedTypes(reader.GetFormat());
	uint32_t internal_format = glformat[0];
	uint32_t import_format = glformat[1];
	uint32_t channel_type = glformat[2];

	for (int mipmap = 0; mipmap < reader.GetMipmapCount(); ++mipmap)
	{
		for (int face = 0; face < reader.GetFaceCount(); ++face)
		{
			auto block_size = reader.GetSize(mipmap);
			auto blob = reader.Read(mipmap, face);

			if (texture->header.compressed)
			{
				glCompressedTexImage2D(gltextype + face, mipmap, internal_format, block_size.x, block_size.y, 0, blob.size, blob.data.get());
			}
			else
			{
				glTexImage2D(gltextype + face, mipmap, internal_format, block_size.x, block_size.y, 0, import_format, channel_type, blob.data.get());
			}
		}
	}

	if (texture->header.MIPMapCount > 1)
	{
		glTexParameteri(gltextype, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		glTexParameteri(gltextype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(gltextype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture->UnBind();

	return texture;
}
