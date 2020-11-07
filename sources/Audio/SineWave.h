#pragma once
#include "IAudioStream.h"
#include <vector>


namespace Audio
{
	class SineWave: public CIAudioStream
	{
	public:
		explicit SineWave(float frequency, int sample_rate=44100):
			frequency(frequency), sample_rate(sample_rate), frame(0) {}

		int Read(uint8_t* dst, int frames) final
		{
			constexpr float two_pi = 6.283185307179586476925286766559;
			int step = GetBitsPerSample();
			for (int n = 0; n < frames; n++)
			{
				*(float*)dst = sinf((two_pi * (n + frame) * frequency) / sample_rate);
				dst += step;
			}
			frame += frames;
			return frames;
		}

		void Reset() final { frame = 0; }

		int GetChannelCount() const final { return 1; };

		int GetSamplingRate() const final { return sample_rate; };

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(DataType::F32); };

		DataType GetDataType() const final { return DataType::F32; };
	private:
		float frequency;
		int sample_rate;
		int64_t frame;
	};

	inline AudioStream MakeSineWave(float frequency, int sample_rate=44100)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<SineWave>(frequency, sample_rate);
		return Audio::AudioStream(stream);
	}

	class FModulatedSineWave: public CIAudioStream
	{
	public:
		explicit FModulatedSineWave(AudioStream input, float a=1.0f, float b=1.0f):
			input(input.get()), a(a), b(b)
		{
			memset(phase, 0, sizeof(float) * 6);
		}

		int Read(uint8_t* _dst, int frames) final
		{
			int input_step = input->GetBitsPerSample();
			data.resize(input_step * frames * GetChannelCount());
			int read = input->Read(&data[0], frames);

			constexpr float two_pi = 6.283185307179586476925286766559;
			int channel = GetChannelCount();
			const float* src = (float*)&data[0];
			float* dst = (float*)_dst;
			int sample_rate = GetSamplingRate();
			for (int n = 0; n < read; n++)
			{
				for (int c = 0; c < channel; c++)
				{
					float f = src[c] * a * b + b;
					float phase_delta = (two_pi * f) / sample_rate;
					phase[c] += phase_delta;
					dst[c] = sinf(phase[c]);
				}
				src += channel;
				dst += channel;
			}
			for (int c = 0; c < channel; c++)
			{
				phase[c] = fmodf(phase[c], two_pi);
			}
			return read;
		}

		void Reset() final { input->Reset(); }

		int GetChannelCount() const final { return input->GetChannelCount(); }

		int GetSamplingRate() const final { return input->GetSamplingRate(); }

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(DataType::F32); }

		DataType GetDataType() const final { return DataType::F32; };
	private:
		IAudioStreamPtr input;
		float phase[6];
		std::vector<uint8_t> data;
		float a, b;
	};

	inline AudioStream MakeFModulatedSineWave(AudioStream input, float a, float b)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<FModulatedSineWave>(input, a, b);
		return Audio::AudioStream(stream);
	}

	class FloatTo: public CIAudioStream
	{
	public:
		explicit FloatTo(AudioStream input, DataType data_type=DataType::S16I):
			input(input.get()), data_type(data_type) {}

		int Read(uint8_t* dst, int frames) final
		{
			int step = input->GetBitsPerSample();
			int dst_step = IAudioStream::GetBitsPerSample(data_type);
			data.resize(step * frames * GetChannelCount());
			int read = input->Read(&data[0], frames);
			const float* src = (float*)&data[0];
			for (int n = 0; n < read * GetChannelCount(); ++n)
			{
				write_frame(src + n, 1, dst, data_type);
				dst += dst_step;
			}
			return read;
		}

		void Reset() final { input->Reset(); }

		int GetChannelCount() const final { return input->GetChannelCount(); }

		int GetSamplingRate() const final { return input->GetSamplingRate(); }

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(data_type); }

		DataType GetDataType() const final { return data_type; };
	private:
		IAudioStreamPtr input;
		DataType data_type;
		std::vector<uint8_t> data;
	};

	inline AudioStream Convert(AudioStream input, IAudioStream::DataType data_type=IAudioStream::DataType::S16I)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<FloatTo>(input, data_type);
		return Audio::AudioStream(stream);
	}

	inline AudioStream UhrrSound()
	{
		return Audio::MakeFModulatedSineWave(
					Audio::MakeFModulatedSineWave(Audio::MakeSineWave(26.0f), 0.5, 126.0), 0.3f, 349.228f * 2
			);
	}

	class Envelope: public CIAudioStream
	{
	public:
		explicit Envelope(float a, float d, float s, float r, float s_time, int sample_rate=44100):
			a(a), d(d), s(s), r(r), s_time(s_time), sample_rate(sample_rate), time(0) {}

		int Read(uint8_t* dst, int frames) final
		{
			int step = GetBitsPerSample();
			for (int n = 0; n < frames; n++)
			{
				float m;
				if (time < a)
				{
					m = time / a;
				}
				else if (time < d + a)
				{
					m = (1.0 - (time - a) / d) * (1.0 - s) + s;
				}
				else if (time < d + a + s_time)
				{
					m = s;
				}
				else if (time < d + a + s_time + r)
				{
					m = s * (1.0 - (time - (a + d + s_time)) / r);
				}
				else
				{
					m = 0;
				}
				time += 1.0f / sample_rate;

				time = fmodf(time, 10.0f);

				*(float*)dst = m;
				dst += step;
			}
			return frames;
		}

		void Reset() final { time = 0; }

		int GetChannelCount() const final { return 1; };

		int GetSamplingRate() const final { return sample_rate; };

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(DataType::F32); };

		DataType GetDataType() const final { return DataType::F32; };
	private:
		float a, d, s, r, s_time;
		int sample_rate;
		float time;
	};

	inline AudioStream MakeEnvelope(float a, float d, float s, float r, float s_time)
	{
		return Audio::AudioStream(std::make_shared<Audio::Envelope>(a, d, s, r, s_time));
	}

	class AModulation: public CIAudioStream
	{
	public:
		explicit AModulation(AudioStream input1, AudioStream input2):
			input1(input1.get()), input2(input2.get())
		{
		}

		int Read(uint8_t* _dst, int frames) final
		{
			int input_step1 = input1->GetBitsPerSample() * input1->GetChannelCount();
			int input_step2 = input2->GetBitsPerSample() * input2->GetChannelCount();
			data1.resize(input_step1 * frames);
			int read1 = input1->Read(&data1[0], frames);
			data2.resize(input_step2 * frames);
			int read2 = input2->Read(&data2[0], frames);
			int channel = input1->GetChannelCount();
			int achannel = input2->GetChannelCount();
			int read = std::min(read1, read2);

			const float* src = (float*)&data1[0];
			const float* asrc = (float*)&data2[0];
			float* dst = (float*)_dst;
			for (int n = 0; n < read; n++)
			{
				for (int c = 0; c < channel; c++)
				{
					float m = asrc[c % achannel];
					dst[c] = m * src[c];
				}
				src += channel;
				asrc += achannel;
				dst += channel;
			}
			return read;
		}

		void Reset() final { input1->Reset(); input2->Reset(); }

		int GetChannelCount() const final { return input1->GetChannelCount(); }

		int GetSamplingRate() const final { return input1->GetSamplingRate(); }

		size_t GetBitsPerSample() const  final { return IAudioStream::GetBitsPerSample(DataType::F32); }

		DataType GetDataType() const final { return DataType::F32; };
	private:
		IAudioStreamPtr input1;
		IAudioStreamPtr input2;
		std::vector<uint8_t> data1;
		std::vector<uint8_t> data2;
	};

	inline AudioStream MakeAModulation(AudioStream input, AudioStream inputa)
	{
		Audio::IAudioStreamPtr stream = std::make_shared<AModulation>(input, inputa);
		return Audio::AudioStream(stream);
	}

//	inline AudioStream MakeSineWave(float frequency, int sample_rate=44100)
//	{
//		Audio::IAudioStreamPtr stream = std::make_shared<SineWave>(frequency, sample_rate);
//		return Audio::AudioStream(stream);
//	}

}
