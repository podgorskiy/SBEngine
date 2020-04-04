#include "TextureFormat.h"
#include <spdlog/spdlog.h>

using namespace Render;


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
