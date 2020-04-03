#pragma once
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

		virtual Image ReadImage(int mipmap, int face) = 0;

		virtual glm::ivec2 GetSize(int mipmap) const = 0;

		virtual int GetFaceCount() const = 0;

		virtual Image::DataType GetDataType() const = 0;

		virtual size_t GetBPP() const
		{
			return Image::GetBPP(GetDataType());
		}

		virtual ~IReader() = default;
	};


	class TextureReader: private IReader
	{
	public:
		IReader::Blob Read(int mipmap, int face) final { return m_reader->Read(mipmap, face); }

		Image ReadImage(int mipmap, int face) final  { return m_reader->ReadImage(mipmap, face); }

		glm::ivec2 GetSize(int mipmap) const final  { return m_reader->GetSize(mipmap); }

		int GetFaceCount() const final   { return m_reader->GetFaceCount(); }

		Image::DataType GetDataType() const final { return m_reader->GetDataType(); }

		size_t GetBPP() const final  { return m_reader->GetBPP(); }

	private:
		IReaderPtr m_reader;
	};
}
