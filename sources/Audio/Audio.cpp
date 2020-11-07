#include "Audio.h"

#include <spdlog/spdlog.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <glm/glm.hpp>


using namespace Audio;

static constexpr int buffers_count = 20;

struct PlaybackContext
{
	AudioStream m_stream;

	fsal::File file;
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

void AudioContext::Init()
{
	glm::vec3 listenerPosition (0.f, 0.f, 0.f);
	glm::vec3 listenerDirection(0.f, 0.f, -1.f);
	glm::vec3 listenerUpVector (0.f, 1.f, 0.f);

	m_device = alcOpenDevice(nullptr);
	if (!m_device)
	{
		spdlog::error("Error during opening OpenAL device");
	}

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCcontext *context;

	context = alcCreateContext(m_device, NULL);
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
    alListenerf(AL_GAIN, listenerVolume * 0.01f);
    alListener3f(AL_POSITION, listenerPosition.x, listenerPosition.y, listenerPosition.z);
    alListenerfv(AL_ORIENTATION, orientation);
	alListener3f(AL_VELOCITY, 0, 0, 0);
}

void AudioContext::Destroy()
{
	alcCloseDevice(m_device);
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
	auto ctx = new PlaybackContext;
	m_contexts.push_back(ctx);
	ctx->file = std::move(file);
	InitContext(*ctx);
	ctx->loop = loop;
	return ctx;
}


PlaybackContext* AudioContext::PlayStream(const Audio::AudioStream& stream, bool loop)
{
	auto ctx = new PlaybackContext;
	m_contexts.push_back(ctx);
	ctx->m_stream = stream;
	InitContext(*ctx);
	ctx->loop = loop;
	return ctx;
}

void AudioContext::StopPlaying(PlaybackContext* ctx)
{
	for (int i = 0, l = m_contexts.size(); i < l; ++i)
	{
		if (ctx == m_contexts[i])
		{
			alSourcei(ctx->source, AL_SOURCE_STATE, AL_STOPPED);
			DeleteContext(*ctx);
			delete ctx;
			m_contexts[i] = nullptr;
			if (i + 1 != l)
			{
				m_contexts[i] = m_contexts[l - 1];
			}
			m_contexts.resize(l - 1);
			break;
		}
	}
}

void AudioContext::Reset(PlaybackContext* ctx)
{
	for (int i = 0, l = m_contexts.size(); i < l; ++i)
	{
		if (ctx == m_contexts[i])
		{
			m_contexts[i] = nullptr;
			if (i + 1 != l)
			{
				m_contexts[i] = m_contexts[l - 1];
			}
			m_contexts.resize(l - 1);
			break;
		}
	}
	ctx->current = 0;
	ctx->last = 0;
	ctx->buffers_in_queue = 0;
	ctx->last_byte_was_decoded = false;

	ctx->m_stream.Reset();

	m_contexts.push_back(ctx);
}


void AudioContext::Update()
{
	for (size_t i = 0; i < m_contexts.size(); ++i)
	{
		auto ctx = m_contexts[i];
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
					delete m_contexts[i];
					m_contexts[i] = nullptr;
					if (i + 1 != m_contexts.size())
					{
						m_contexts[i] = m_contexts[m_contexts.size() - 1];
						--i;
					}
					m_contexts.resize(m_contexts.size() - 1);
				}
			}
		}
	}
}
