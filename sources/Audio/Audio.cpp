#include "Audio.h"
#include "SineWave.h"
#include "MP3Stream.h"
#include "OGGStream.h"

#include <spdlog/spdlog.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <glm/glm.hpp>


using namespace Audio;


typedef struct ALCdevice_struct ALCdevice;

static constexpr int buffers_count = 20;

struct Audio::AudioImplementation
{
	std::vector<PlaybackContext*> m_contexts;
	float listenerVolume = 100.f;
	ALCdevice* m_device = nullptr;
};

struct Audio::PlaybackContext
{
	AudioStream m_stream;

    uint32_t buffers[buffers_count];
    unsigned int source;
    short pcm[1152*16];

	int current = 0;
	int last = 0;
	int buffers_in_queue = 0;

	bool last_byte_was_decoded = false;

	bool loop = false;
};

static void list_audio_devices(const ALCchar *devices)
{
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        spdlog::info("Devices list:");
        while (device && *device != '\0' && next && *next != '\0') {
                spdlog::info("\t{}", device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
}

#if defined AUDIO_SINGLETON

AudioContext::AudioContext()
{
	static std::shared_ptr<AudioImplementation> ptr(new AudioImplementation());
	m_impl = ptr;
}
#else
AudioContext::AudioContext()
{
	m_impl.reset(new AudioImplementation());
}
#endif


void AudioContext::Init()
{
	glm::vec3 listenerPosition (0.f, 0.f, 0.f);
	glm::vec3 listenerDirection(0.f, 0.f, -1.f);
	glm::vec3 listenerUpVector (0.f, 1.f, 0.f);

	m_impl->m_device = alcOpenDevice(nullptr);
	if (!m_impl->m_device)
	{
		spdlog::error("Error during opening OpenAL device");
	}

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCcontext *context;

	context = alcCreateContext(m_impl->m_device, NULL);
	if (!alcMakeContextCurrent(context))
	{
		spdlog::error("Error during making context current, OpenAL");
	}

    float orientation[] = {listenerDirection.x,
                           listenerDirection.y,
                           listenerDirection.z,
                           listenerUpVector.x,
                           listenerUpVector.y,
                           listenerUpVector.z};
    alListenerf(AL_GAIN, m_impl->listenerVolume * 0.01f);
    alListener3f(AL_POSITION, listenerPosition.x, listenerPosition.y, listenerPosition.z);
    alListenerfv(AL_ORIENTATION, orientation);
	alListener3f(AL_VELOCITY, 0, 0, 0);
}

void AudioContext::Destroy()
{
	alcCloseDevice(m_impl->m_device);
}

void AudioContext::InitContext(PlaybackContext& ctx)
{
    alGenSources(1, &ctx.source);
    alSourcei(ctx.source, AL_BUFFER, 0);
    alGenBuffers(buffers_count, ctx.buffers);

	ctx.current = 0;
	ctx.last = 0;
	ctx.buffers_in_queue = 0;
}

void AudioContext::DeleteContext(PlaybackContext& ctx)
{
    alDeleteSources(1, &ctx.source);
    alDeleteBuffers(buffers_count, ctx.buffers);
}

PlaybackContext* AudioContext::PlayFile(fsal::File file, bool loop)
{
    uint8_t magic[5];
	file.Read(magic, 4);
	magic[4] = 0;
	file.Seek(0);
	if (magic[0] == 'I' && magic[1] == 'D' && magic[2] == '3')
	{
		PlayStream(MakeMP3Stream(file), loop);
	}
	else if (magic[0] == 'O' && magic[1] == 'g' && magic[2] == 'g')
	{
		PlayStream(MakeOGGStream(file), loop);
	}
	else
	{
		spdlog::error("Audio file format \"{}\"not supported!", (const char*)magic);
	}
}

PlaybackContext* AudioContext::PlayFile(fsal::Location file, bool loop)
{
	PlayFile(fsal::FileSystem().Open(file), loop);
}

PlaybackContext* AudioContext::PlayStream(const Audio::AudioStream& stream, bool loop)
{
	auto ctx = new PlaybackContext;
	m_impl->m_contexts.push_back(ctx);
	ctx->m_stream = stream;
	InitContext(*ctx);
	ctx->loop = loop;
	return ctx;
}

void AudioContext::StopPlaying(PlaybackContext* ctx)
{
	auto ptr = m_impl.get();
	for (int i = 0, l = ptr->m_contexts.size(); i < l; ++i)
	{
		if (ctx == ptr->m_contexts[i])
		{
			alSourcei(ctx->source, AL_SOURCE_STATE, AL_STOPPED);
			DeleteContext(*ctx);
			delete ctx;
			ptr->m_contexts[i] = nullptr;
			if (i + 1 != l)
			{
				ptr->m_contexts[i] = ptr->m_contexts[l - 1];
			}
			ptr->m_contexts.resize(l - 1);
			break;
		}
	}
}

void AudioContext::Reset(PlaybackContext* ctx)
{
	auto ptr = m_impl.get();
	for (int i = 0, l = ptr->m_contexts.size(); i < l; ++i)
	{
		if (ctx == ptr->m_contexts[i])
		{
			ptr->m_contexts[i] = nullptr;
			if (i + 1 != l)
			{
				ptr->m_contexts[i] = ptr->m_contexts[l - 1];
			}
			ptr->m_contexts.resize(l - 1);
			break;
		}
	}
	ctx->current = 0;
	ctx->last = 0;
	ctx->buffers_in_queue = 0;
	ctx->last_byte_was_decoded = false;

	ctx->m_stream.Reset();

	ptr->m_contexts.push_back(ctx);
}


void AudioContext::Update()
{
	auto ptr = m_impl.get();
	for (size_t i = 0; i < ptr->m_contexts.size(); ++i)
	{
		auto ctx = ptr->m_contexts[i];
		if (ctx == nullptr)
			continue;

		bool just_read_first = true;
		while (ctx->buffers_in_queue < buffers_count && !ctx->last_byte_was_decoded)
		{
			int frames = MINIMP3_MAX_SAMPLES_PER_FRAME / ctx->m_stream.GetChannelCount();

			int samples = ctx->m_stream.Read((uint8_t*)ctx->pcm, frames);
			ctx->last_byte_was_decoded = samples == 0;

			if (samples < 0)
				continue;

            assert(ctx->m_stream.GetChannelCount() <= 2);
            assert(ctx->m_stream.GetDataType() == IAudioStream::S16I);
            ALenum format = ctx->m_stream.GetChannelCount() == 1 ?  AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			alBufferData(ctx->buffers[ctx->current], format, ctx->pcm, ctx->m_stream.GetChannelCount() * 2 * samples, ctx->m_stream.GetSamplingRate());
			alSourceQueueBuffers(ctx->source, 1, ctx->buffers + ctx->current);

			++ctx->buffers_in_queue;
			++ctx->current;
			ctx->current = ctx->current % buffers_count;

			if (just_read_first)
			{
				ALint state = 0;
				alGetSourcei(ctx->source, AL_SOURCE_STATE, &state);
				if (state != AL_PLAYING)
					alSourcePlay(ctx->source);
				just_read_first = false;
			}
		}
		int processed = 0;
		alGetSourcei(ctx->source, AL_BUFFERS_PROCESSED, &processed);

		for (int i = 0; i < processed; ++i)
		{
			alSourceUnqueueBuffers(ctx->source, 1, ctx->buffers + ctx->last);
			++ctx->last;
			ctx->last = ctx->last % buffers_count;
			--ctx->buffers_in_queue;
		}

		if (ctx->buffers_in_queue == 0 && ctx->last_byte_was_decoded)
		{
			ALint state = 0;
			alGetSourcei(ctx->source, AL_SOURCE_STATE, &state);
			if (state != AL_PLAYING)
			{
				if (ctx->loop)
				{
					ctx->current = 0;
					ctx->last = 0;
					ctx->buffers_in_queue = 0;
					ctx->last_byte_was_decoded = false;
					ctx->m_stream.Reset();
				}
				else
				{
					DeleteContext(*ctx);
					delete ptr->m_contexts[i];
					ptr->m_contexts[i] = nullptr;
					if (i + 1 != ptr->m_contexts.size())
					{
						ptr->m_contexts[i] = ptr->m_contexts[ptr->m_contexts.size() - 1];
						--i;
					}
					ptr->m_contexts.resize(ptr->m_contexts.size() - 1);
				}
			}
		}
	}
}
