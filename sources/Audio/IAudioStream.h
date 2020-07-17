#pragma once
#include <memory>


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
		struct Blob
		{
			std::shared_ptr<uint8_t> data;
			size_t offset;
			size_t size;
		};

		virtual Blob Read(int samples) = 0;

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
		}
	};

	class CIAudioStream: public IAudioStream
	{
	public:
		CIAudioStream(const CIAudioStream&) = delete; // non construction-copyable
		CIAudioStream& operator=(const CIAudioStream&) = delete; // non copyable
	};

	class AudioStream: private IAudioStream
	{
	public:
		explicit AudioStream(IAudioStreamPtr stream): m_stream(std::move(stream)) {}
		AudioStream() = default;
		AudioStream(const AudioStream& other) = default;

		Blob Read(int samples) final { return m_stream->Read(samples); };

		int GetChannelCount() const final { return m_stream->GetChannelCount(); };

		int GetSamplingRate() const final { return m_stream->GetSamplingRate(); };

		size_t GetBitsPerSample() const  final { return m_stream->GetBitsPerSample(); };

		DataType GetDataType() const final { return m_stream->GetDataType(); };

	private:
		IAudioStreamPtr m_stream;
	};
}
