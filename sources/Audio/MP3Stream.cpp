#include "MP3Stream.h"

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3.h"
#include <assert.h>

using namespace Audio;


MP3Stream::MP3Stream(fsal::File file): m_file(file)
{
    uint8_t magic[4];
	m_file.Read(magic, 4);
	m_file.Seek(0);
	assert (magic[0] == 'I' && magic[1] == 'D' && magic[2] == '3');
	mp3dec_init(&mp3d);

	size_t size_to_read = 1024 * 16;
	size_t read_bytes = 0;
	m_file.Read(data_buffer + data_end, size_to_read, &read_bytes);
	m_file.Seek(0);
	mp3dec_decode_frame(&mp3d, data_buffer + data_start, read_bytes, nullptr, &info);
}

int MP3Stream::Read(uint8_t* dst, int frames)
{
	size_t read_bytes = 0;
	size_t size_to_read = 1024 * 16 - (data_end - data_start);

	if (size_to_read > 0 && !last_byte_was_read)
	{
		if (size_to_read + data_end > 1024 * 16 * 2)
		{
			size_t size = data_end - data_start;
			memmove(data_buffer, data_buffer + data_start, size);
			data_end = size;
			data_start = 0;
		}

		m_file.Read(data_buffer + data_end, size_to_read, &read_bytes);
		data_end += read_bytes;
	}

	size_t buffer_size = data_end - data_start;
	int samples = mp3dec_decode_frame(&mp3d, data_buffer + data_start, buffer_size, (short*)dst, &info);
	data_start += info.frame_bytes;

	last_byte_was_read = m_file.Tell() == m_file.GetSize();
	return samples;
}

void MP3Stream::Reset()
{
	data_start = 0;
	data_end = 0;
	current = 0;
	last = 0;
	buffers_in_queue = 0;
	last_byte_was_read = false;
	m_file.Seek(0);
}


int MP3Stream::GetChannelCount() const
{
	return info.channels;
}

int MP3Stream::GetSamplingRate() const
{
	return info.hz;
}
