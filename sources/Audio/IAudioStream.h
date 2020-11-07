#pragma once
#include <memory>
#include <cmath>
#include <string.h>


namespace Audio
{
	class IAudioStream;
	typedef std::shared_ptr<IAudioStream> IAudioStreamPtr;

	class IAudioStream
	{
	public:
		enum DataType
		{
			S8I,
			S16I,
			S32I,
			F32,
			D64
		};

		virtual int Read(uint8_t* dst, int frames) = 0;

		virtual void Reset() = 0;

		virtual int GetChannelCount() const = 0;

		virtual int GetSamplingRate() const = 0;

		virtual size_t GetBitsPerSample() const = 0;

		virtual DataType GetDataType() const = 0;

		virtual ~IAudioStream() = default;

		static int GetBitsPerSample(DataType dt)
		{
			switch(dt)
			{
				case S8I: return 1;
				case S16I: return 2;
				case S32I: return 4;
				case F32: return 4;
				case D64: return 8;
			}
			return 0;
		}
	};

	template<typename T>
	inline T sign(T v) { return v < T(0) ? T(-1) : (v == T(0) ? T(0) : T(1)); }

	template<typename T>
	inline T abs(T v) { return v < T(0) ? -v : v; }

	template<typename T>
	inline T pow(T v, T y) { return std::pow(v, y); }

	template<typename T>
	inline T clamp(T v, T min, T max) { return v < min ? min : (v > max ? max : v); }

	template<typename T>
	inline T min(T v1, T v2) { return v1 < v2 ? v1 : v2; }

	template<typename T>
	inline T max(T v1, T v2) { return v1 < v2 ? v2 : v1; }

	inline void write_frame(const float* samples, int channels, uint8_t* dst, IAudioStream::DataType dt)
	{
		if(dt == IAudioStream::F32)
		{
			memcpy(dst, samples, channels * sizeof(float));
		}
		else if (channels == 1)
		{
			switch(dt)
			{
				case IAudioStream::S8I: *(int8_t*)dst = (int8_t)(clamp(samples[0] * 127.f, -127.f, 127.f)); break;
				case IAudioStream::S16I: *(int16_t*)dst = (int16_t)(clamp(samples[0] * 32767.f, -32768.f, 32767.f)); break;
				case IAudioStream::S32I: *(int32_t*)dst = (int32_t)(clamp(samples[0] * 2147483647.f, -2147483648.f, 2147483647.f)); break;
				case IAudioStream::D64: *(double*)dst = samples[0]; break;
			}
		}
		else
		{
			for (int c = 0; c < channels; ++c)
			{
				switch(dt)
				{
					case IAudioStream::S8I: *(int8_t*)dst = (int8_t)(clamp(samples[c] * 127.f, -127.f, 127.f)); break;
					case IAudioStream::S16I: *(int16_t*)dst = (int16_t)(clamp(samples[c] * 32767.f, -32768.f, 32767.f)); break;
					case IAudioStream::S32I: *(int32_t*)dst = (int32_t)(clamp(samples[c] * 2147483647.f, -2147483648.f, 2147483647.f)); break;
					case IAudioStream::D64: *(double*)dst = samples[c]; break;
				}
			}
		}
	}

	class CIAudioStream: public IAudioStream
	{
	public:
		CIAudioStream() {};
		CIAudioStream(const CIAudioStream&) = delete; // non construction-copyable
		CIAudioStream& operator=(const CIAudioStream&) = delete; // non copyable
	};

	class AudioStream: private IAudioStream
	{
	public:
		explicit AudioStream(IAudioStreamPtr stream): m_stream(std::move(stream)) {}
		AudioStream() = default;
		AudioStream(const AudioStream& other) = default;

		int Read(uint8_t* dst, int frames) final { return m_stream->Read(dst, frames); };

		void Reset() final { return m_stream->Reset(); };

		int GetChannelCount() const final { return m_stream->GetChannelCount(); };

		int GetSamplingRate() const final { return m_stream->GetSamplingRate(); };

		size_t GetBitsPerSample() const  final { return m_stream->GetBitsPerSample(); };

		DataType GetDataType() const final { return m_stream->GetDataType(); };

		IAudioStreamPtr get() { return m_stream; }
	private:
		IAudioStreamPtr m_stream;
	};
}
