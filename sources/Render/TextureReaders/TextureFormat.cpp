#include "TextureFormat.h"
#include <spdlog/spdlog.h>

using namespace Render;


uint64_t Render::ParsePixelType(const char* s)
{
	char buff[255];
	strncpy(buff, s, 255);

	int i = 0;
	while (buff[i] && i < 255)
	{
		buff[i] = (tolower(buff[i]));
		i++;
	}

	uint64_t type = 0;
	uint64_t mul = 1;
	for (i = 0; i < 4; i++)
	{
		if (buff[i] == 'r' || buff[i] == 'g' || buff[i] == 'b' || buff[i] == 'a' || buff[i] == 'l' ||
		    buff[i] == 'd')
		{
			type += buff[i] * mul;
			mul = mul << 8u;
		}
		else
		{
			break;
		}
	}
	if (i == 0)
	{
		throw runtime_error("Format should start from channel names, such as: r, g, b, a, l, d. Got %s", buff[0]);
	}
	for (int _i = i; _i < 4; _i++)
	{
		mul = mul << 8u;
	}

	if (!(buff[i] >= '0' && buff[i] <= '9'))
	{
		throw runtime_error("Unexpected symbol after channel names: %c. Expected digits", buff[i]);
	}

	int j = 0;
	const char* p = buff + i;
	for (; j < 4; j++)
	{
		if (*p >= '0' && *p <= '9')
		{
			int v = *p - '0';
			if (*p == '1' && *(p + 1) == '0')
			{
				v = 10;
				++p;
			}
			if (*p == '1' && *(p + 1) == '1')
			{
				v = 11;
				++p;
			}
			if (*p == '1' && *(p + 1) == '2')
			{
				v = 12;
				++p;
			}
			if (*p == '1' && *(p + 1) == '6')
			{
				v = 16;
				++p;
			}
			if (*p == '3' && *(p + 1) == '2')
			{
				v = 32;
				++p;
			}
			++p;
			type += v * mul;
			mul = mul << 8u;
		}
		else
		{
			break;
		}
	}

	if (*p != '\0')
	{
		throw runtime_error("Unexpected symbol at the end of the format: %c", *p);
	}

	if (i != j)
	{
		throw runtime_error("Number of channel names and sizes do not match, got %d and %d", i, j);
	}
	return type;
}


DecodedType Render::DecodePixelType(uint64_t pixel_format)
{
	bool compressed = (pixel_format & 0xFFFFFFFF00000000) == 0;
	if (compressed)
	{
		return {compressed, {}, {}};
	}
	std::vector<char> channels;
	std::vector<uint8_t> sizes;
	for (int i = 0; i < 4; ++i)
	{
		char v = pixel_format & 0xffu;
		if (v != 0)
		{
			channels.push_back(v);
		}
		pixel_format = pixel_format >> 8u;
	}
	for (int i = 0; i < 4; ++i)
	{
		uint8_t v = pixel_format & 0xffu;
		if (v != 0)
		{
			sizes.push_back(v);
		}
		pixel_format = pixel_format >> 8u;
	}

	return {false, channels, sizes};
}


size_t TextureFormat::GetBitsPerPixel(Format f)
{
	switch (f)
	{
		case R8:
		case A8:
		case L8:
			return 8;

		case R16:
		case RG88:
		case LA88:
		case RGB565:
		case RGBA4444:
		case RGBA5551:
			return 16;

		case RGB888:
			return 24;

		case RG1616:
		case RGBA8888:
		case RGBA1010102:
		case BGRA8888:
		case BGR101111:
			return 32;

		case RGB161616:
			return 48;

		case RGBA16161616:
		case RG3232:
			return 64;

		case RGB323232:
			return 96;

		case RGBA32323232:
			return 128;

		case PVRTCI_2bpp_RGB:
		case PVRTCI_2bpp_RGBA:
		case PVRTCII_2bpp:
			return 2;

		case PVRTCI_4bpp_RGB:
		case PVRTCI_4bpp_RGBA:
		case PVRTCII_4bpp:
		case ETC1:
		case EAC_R11:
		case ETC2_RGB:
		case ETC2_RGB_A1:
		case DXT1:
			return 4;

		case DXT2:
		case DXT3:
		case DXT4:
		case DXT5:
		case EAC_RG11:
		case ETC2_RGBA:
			return 8;
		case RGBG8888:
		case GRGB8888:
			return 16;
	}
	spdlog::error("Unsupported texture format {}", f);
	return 0;
}


glm::ivec2 TextureFormat::GetMinBlockSize(Format f)
{
	if (uint64_t(f) >> 32u)
	{
		return glm::ivec2(1);
	}
	else
	{
		switch (f)
		{
			case DXT1:
			case DXT2:
			case DXT3:
			case DXT4:
			case DXT5:
			case ETC1:
			case ETC2_RGB:
			case ETC2_RGB_A1:
			case ETC2_RGBA:
			case EAC_R11:
			case EAC_RG11:
				return glm::ivec2(4);

			case PVRTCI_4bpp_RGB:
			case PVRTCI_4bpp_RGBA:
				return glm::ivec2(8);

			case PVRTCI_2bpp_RGB:
			case PVRTCI_2bpp_RGBA:
				return glm::ivec2(16, 8);

			case PVRTCII_4bpp:
				return glm::ivec2(4);

			case PVRTCII_2bpp:
				return glm::ivec2(8, 4);

			case RGBG8888:
			case GRGB8888:
				return glm::ivec2(2, 1);

#define _ASTC(W, H) case ASTC##_##W##x##H: return glm::ivec2(W, H);

			_ASTC(4, 4)
			_ASTC(5, 4)
			_ASTC(5, 5)
			_ASTC(6, 5)
			_ASTC(6, 6)
			_ASTC(8, 5)
			_ASTC(8, 6)
			_ASTC(8, 8)
			_ASTC(10, 5)
			_ASTC(10, 6)
			_ASTC(10, 8)
			_ASTC(10, 10)
			_ASTC(12, 10)
			_ASTC(12, 12)
#undef _ASTC
			default:
				spdlog::error("Unsupported texture format {}", f);
				return glm::ivec2(1);
		}
	}
}


#include <doctest.h>

TEST_CASE("[Render] PVRReader")
{
	CHECK_EQ(Render::PixelType<'r', 8, 'g', 8, 'b', 8, 'a', 8>::ID, Render::TextureFormat::RGBA8888);
	CHECK_EQ(Render::PixelType<'r', 8, 'g', 8, 'b', 8, 'a', 8>::ID, Render::getPixelType('r', 8, 'g', 8, 'b', 8, 'a', 8));
	CHECK_EQ(Render::PixelType<'r', 8, 'g', 8, 'b', 8, 'a', 8>::ID, Render::ParsePixelType("RGBA8888"));
	CHECK_EQ(Render::PixelType<'r', 8, 'g', 8, 'b', 8>::ID, Render::ParsePixelType("RGB888"));
	CHECK_EQ(Render::PixelType<'r', 5, 'g', 6, 'b', 5>::ID, Render::ParsePixelType("RGB565"));
	CHECK_EQ(Render::PixelType<'r', 32, 'g', 32, 'b', 32, 'a', 32>::ID, Render::ParsePixelType("RGBA32323232"));
	CHECK_NE(Render::PixelType<'r', 8, 'g', 8, 'b', 8, 'a', 8>::ID, Render::ParsePixelType("RGB888"));
	REQUIRE_THROWS(Render::ParsePixelType("RGBA888"));
	REQUIRE_THROWS(Render::ParsePixelType("RGB8888"));
	REQUIRE_THROWS(Render::ParsePixelType(" RGBA8888"));
	REQUIRE_THROWS(Render::ParsePixelType("RGBA8888 "));


	CHECK_EQ(Render::TextureFormat::GetChannelCount(Render::TextureFormat::RGBA8888), 4);
	CHECK_EQ(Render::TextureFormat::GetChannelCount(Render::TextureFormat::RGB888), 3);
	CHECK_EQ(Render::TextureFormat::GetChannelCount(Render::TextureFormat::RG88), 2);
	CHECK_EQ(Render::TextureFormat::GetChannelCount(Render::TextureFormat::R8), 1);
}

