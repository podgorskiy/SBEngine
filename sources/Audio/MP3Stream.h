#pragma once
#include "IAudioStream.h"
#include <fsal.h>
#define MINIMP3_ONLY_MP3
#include "minimp3.h"


namespace Audio
{
	class MP3Stream : public CIAudioStream
	{
	public:
		explicit MP3Stream(fsal::File file);

		int Read(uint8_t* dst, int frames) final;

		void Reset() final;

		int GetChannelCount() const final;

		int GetSamplingRate() const final;

		size_t GetBitsPerSample() const final
		{ return IAudioStream::GetBitsPerSample(DataType::S16I); };

		DataType GetDataType() const final
		{ return DataType::S16I; };
	private:
		fsal::File m_file;
	    mp3dec_t mp3d;
	    mp3dec_frame_info_t info;

	    uint8_t data_buffer[1024 * 16 * 2];

	    int data_start = 0;
	    int data_end = 0;

		int current = 0;
		int last = 0;
		int buffers_in_queue = 0;
		bool last_byte_was_read = false;
	};

	inline AudioStream MakeMP3Stream(fsal::File file)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<MP3Stream>(file);
		return Audio::AudioStream(stream);
	}
}
