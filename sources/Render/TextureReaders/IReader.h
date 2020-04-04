#pragma once
#include "TextureFormat.h"
#include "Render/Image/Image.h"
#include <glm/glm.hpp>
#include <memory>


namespace Render
{
	class IReader;
	typedef std::shared_ptr<IReader> IReaderPtr;

	class IReader
	{
	public:
		struct Blob
		{
			std::shared_ptr<uint8_t> data;
			size_t size;
		};

		virtual Blob Read(int mipmap, int face) = 0;

		virtual glm::ivec2 GetSize(int mipmap) const = 0;

		virtual int GetFaceCount() const = 0;

		virtual TextureFormat GetFormat() const = 0;

		virtual size_t GetBitsPerPixel() const
		{
			return TextureFormat::GetBitsPerPixel(GetFormat().pixel_format);
		}

		virtual glm::ivec2 GetBlockSize() const = 0;

		virtual ~IReader() = default;
	};


	class TextureReader: private IReader
	{
	public:
		IReader::Blob Read(int mipmap, int face) final { return m_reader->Read(mipmap, face); }

		glm::ivec2 GetSize(int mipmap) const final  { return m_reader->GetSize(mipmap); }

		int GetFaceCount() const final   { return m_reader->GetFaceCount(); }

		TextureFormat GetFormat() const final { return m_reader->GetFormat(); }

		size_t GetBitsPerPixel() const final  { return m_reader->GetBitsPerPixel(); }

		virtual glm::ivec2 GetBlockSize() const = 0;

	private:
		IReaderPtr m_reader;
	};
}
