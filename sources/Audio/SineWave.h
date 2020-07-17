#pragma once
#include "IAudioStream.h"


namespace Audio
{
	class SineWave: public CIAudioStream
	{
	public:
		SineWave(float frequency, DataType data_type=DataType::S16I, int sample_rate=44100);

		Blob Read(int samples) final;

		int GetChannelCount() const final { return 1; };

		int GetSamplingRate() const final { return sample_rate; };

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(data_type); };

		DataType GetDataType() const final { return data_type; };
	private:
		float frequency;
		DataType data_type;
		int sample_rate;
	};

	inline AudioStream MakeSineWave(float frequency, IAudioStream::DataType data_type=IAudioStream::DataType::S16I, int sample_rate=44100)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<SineWave>(frequency, data_type, sample_rate);
		return Audio::AudioStream(stream);
	}
}
