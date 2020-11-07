#pragma once
#include "IAudioStream.h"
#include <fsal.h>

struct OggVorbis_File;

namespace Audio
{
	class OGGStream : public CIAudioStream
	{
	public:
		explicit OGGStream(fsal::File file);

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

		typedef struct
		{
		    int channels, hz;
		} info_t;
	    info_t info;
        OggVorbis_File* ogg_file;
	};

	inline AudioStream MakeOGGStream(fsal::File file)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<OGGStream>(file);
		return Audio::AudioStream(stream);
	}
}
