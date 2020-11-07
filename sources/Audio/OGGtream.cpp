#include "OGGStream.h"
#define OV_EXCLUDE_STATIC_CALLBACKS
#include <vorbis/vorbisfile.h>
#include <assert.h>

using namespace Audio;

#define MINIMP3_MAX_SAMPLES_PER_FRAME (1152*2)

static size_t readOgg(void* dst, size_t size1, size_t size2, void* fh)
{
	auto* of = reinterpret_cast<fsal::File*>(fh);
	size_t len = size1 * size2;
	size_t read;
	of->Read((uint8_t*)dst, len, &read);
	return read;
}

static int seekOgg(void* fh, ogg_int64_t to, int type)
{
	auto* of = reinterpret_cast<fsal::File*>(fh);
	of->Seek(to, (fsal::File::Origin)type);
	return 0;
}

static int closeOgg(void* fh)
{
	return 0;
}

static long tellOgg(void* fh)
{
	auto* of = reinterpret_cast<fsal::File*>(fh);
	return of->Tell();
}


OGGStream::OGGStream(fsal::File file): m_file(file)
{
    uint8_t magic[4];
	m_file.Read(magic, 4);
	m_file.Seek(0);
	assert (magic[0] == 'O' && magic[1] == 'g' && magic[2] == 'g');

	ov_callbacks callbacks;
	callbacks.read_func = readOgg;
	callbacks.seek_func = seekOgg;
	callbacks.close_func = closeOgg;
	callbacks.tell_func = tellOgg;

	ogg_file = new OggVorbis_File;
	memset(ogg_file, 0, sizeof(OggVorbis_File));

	if (ov_test_callbacks(&m_file, ogg_file, nullptr, 0, callbacks) != 0)
	{
		throw std::runtime_error("File is not a valid ogg vorbis file");
	}

	if (ov_test_open(ogg_file) != 0)
	{
		throw std::runtime_error("ov_test_open failed");
	}

	vorbis_info* ovInfo = ov_info(ogg_file, -1);

	if (ov_streams(ogg_file) != 1)
	{
		throw std::runtime_error("Unsupported: file contains multiple bitstreams");
	}

	info.channels = ovInfo->channels;
	info.hz = ovInfo->rate;
}

int OGGStream::Read(uint8_t* dst, int frames)
{
	float** buffer = nullptr;
	int bitstream = 0;
	int64_t framesRead = ov_read_float(ogg_file, &buffer, MINIMP3_MAX_SAMPLES_PER_FRAME, &bitstream);

	if (!framesRead)
	{
		return 0;
	}

	if (framesRead < 0)
	{
		return -1;
	}

	for (int i = 0; i < framesRead; ++i)
	{
		for (int ch = 0; ch < info.channels; ch++)
		{
			((short*)dst)[i * info.channels + ch] = short(buffer[ch][i] * 0xFFFF);
		}
	}
	return framesRead;
}

void OGGStream::Reset()
{
	ov_raw_seek(ogg_file, 0);
}


int OGGStream::GetChannelCount() const
{
	return info.channels;
}

int OGGStream::GetSamplingRate() const
{
	return info.hz;
}
