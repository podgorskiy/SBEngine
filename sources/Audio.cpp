#include "Audio.h"
#include <vorbis/vorbisfile.h>

#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <unistd.h>


static constexpr int buffers_count = 20;

struct AudioContext
{
	enum Type
	{
		Unknown = 0,
		OGG = 1,
		MP3 = 2
	};

	Type type;

	fsal::File file;
    uint32_t buffers[buffers_count];
    unsigned int source;
    mp3dec_t mp3d;
    mp3dec_frame_info_t info;
    OggVorbis_File* ogg_file;
    short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    uint8_t data_buffer[1024 * 16 * 2];

    int data_start = 0;
    int data_end = 0;

	int current = 0;
	int last = 0;
	int buffers_in_queue = 0;

	bool last_byte_was_read = false;
	bool last_byte_was_decoded = false;

	bool loop = false;
};


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
}

static long tellOgg(void* fh)
{
	auto* of = reinterpret_cast<fsal::File*>(fh);
	return of->Tell();
}


static void list_audio_devices(const ALCchar *devices)
{
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        printf("Devices list:\n");
        printf("----------\n");
        while (device && *device != '\0' && next && *next != '\0') {
                printf("%s\n", device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        printf("----------\n");
}

void Audio::Init()
{
	glm::vec3 listenerPosition (0.f, 0.f, 0.f);
	glm::vec3 listenerDirection(0.f, 0.f, -1.f);
	glm::vec3 listenerUpVector (0.f, 1.f, 0.f);

	m_device = alcOpenDevice(nullptr);
	if (!m_device)
	{
		printf("Error\n");
	}

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCcontext *context;

	context = alcCreateContext(m_device, NULL);
	if (!alcMakeContextCurrent(context))
	{
		printf("Error\n");
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

void Audio::Destroy()
{
	alcCloseDevice(m_device);
}

void Audio::InitContext(AudioContext& ctx)
{
    alGenSources(1, &ctx.source);
    alSourcei(ctx.source, AL_BUFFER, 0);
    alGenBuffers(buffers_count, ctx.buffers);

    uint8_t magic[4];
	ctx.file.Read(magic, 4);
	ctx.file.Seek(0);

	if (magic[0] == 'I' && magic[1] == 'D' && magic[2] == '3')
	{
		ctx.type = AudioContext::MP3;
	}
	else if (magic[0] == 'O' && magic[1] == 'g' && magic[2] == 'g')
	{
		ctx.type = AudioContext::OGG;
	}

	if (ctx.type == AudioContext::OGG)
	{
		ov_callbacks callbacks;
		callbacks.read_func = readOgg;
		callbacks.seek_func = seekOgg;
		callbacks.close_func = closeOgg;
		callbacks.tell_func = tellOgg;

		ctx.ogg_file = new OggVorbis_File;
		memset(ctx.ogg_file, 0, sizeof(OggVorbis_File));

		if (auto r = ov_test_callbacks(&ctx.file, ctx.ogg_file, nullptr, 0, callbacks) != 0)
		{
			throw std::runtime_error("File is not a valid ogg vorbis file");
		}

        if (auto r = ov_test_open(ctx.ogg_file) != 0)
        {
            throw std::runtime_error("ov_test_open failed");
        }

        vorbis_info * ovInfo = ov_info(ctx.ogg_file, -1);

        if (auto r = ov_streams(ctx.ogg_file) != 1)
        {
            throw std::runtime_error( "Unsupported: file contains multiple bitstreams");
        }

        ctx.info.channels = ovInfo->channels;
        ctx.info.hz = ovInfo->rate;
	}
	else if (ctx.type == AudioContext::MP3)
	{
		mp3dec_init(&ctx.mp3d);
	}

    ctx.data_start = 0;
    ctx.data_end = 0;
	ctx.current = 0;
	ctx.last = 0;
	ctx.buffers_in_queue = 0;
}

void Audio::DeleteContext(AudioContext& ctx)
{
    alDeleteSources(1, &ctx.source);
    alDeleteBuffers(buffers_count, ctx.buffers);
}

AudioContext* Audio::PlayFile(fsal::File file, bool loop)
{
	auto ctx = new AudioContext;
	m_contexts.push_back(ctx);
	ctx->file = std::move(file);
	InitContext(*ctx);
	ctx->loop = loop;
}

void Audio::StopPlaying(AudioContext* ctx)
{
	for (int i = 0; i < m_contexts.size(); ++i)
	{
		if (ctx == m_contexts[i])
		{
			alSourcei(ctx->source, AL_SOURCE_STATE, AL_STOPPED);
			DeleteContext(*ctx);
			delete ctx;
			m_contexts[i] = nullptr;
			if (i + 1 != m_contexts.size())
			{
				m_contexts[i] = m_contexts[m_contexts.size() - 1];
			}
			m_contexts.resize(m_contexts.size() - 1);
			break;
		}
	}
}

void Audio::Reset(AudioContext* ctx)
{
	for (int i = 0; i < m_contexts.size(); ++i)
	{
		if (ctx == m_contexts[i])
		{
			m_contexts[i] = nullptr;
			if (i + 1 != m_contexts.size())
			{
				m_contexts[i] = m_contexts[m_contexts.size() - 1];
			}
			m_contexts.resize(m_contexts.size() - 1);
			break;
		}
	}
	ctx->data_start = 0;
	ctx->data_end = 0;
	ctx->current = 0;
	ctx->last = 0;
	ctx->buffers_in_queue = 0;
	ctx->last_byte_was_read = false;
	ctx->last_byte_was_decoded = false;
	if (ctx->type ==AudioContext::MP3)
	{
		ctx->file.Seek(0);
	}
	else if (ctx->type ==AudioContext::OGG)
	{
		ov_raw_seek(ctx->ogg_file, 0);
	}
	m_contexts.push_back(ctx);
}


void Audio::Update()
{
	for (int i = 0; i < m_contexts.size(); ++i)
	{
		auto ctx = m_contexts[i];
		if (ctx == nullptr)
			continue;

		bool just_read_first = true;
		while (ctx->buffers_in_queue < buffers_count && !ctx->last_byte_was_decoded)
		{
			int samples = 0;
			if (ctx->type == AudioContext::MP3)
			{
				size_t read_bytes = 0;
				size_t size_to_read = 1024 * 16 - (ctx->data_end - ctx->data_start);

				if (size_to_read > 0 && !ctx->last_byte_was_read)
				{
					if (size_to_read + ctx->data_end > 1024 * 16 * 2)
					{
						size_t size = ctx->data_end - ctx->data_start;
						memmove(ctx->data_buffer, ctx->data_buffer + ctx->data_start, size);
						ctx->data_end = size;
						ctx->data_start = 0;
					}

					ctx->file.Read(ctx->data_buffer + ctx->data_end, size_to_read, &read_bytes);
					ctx->data_end += read_bytes;
				}

				size_t buffer_size = ctx->data_end - ctx->data_start;
				ctx->last_byte_was_decoded = buffer_size == 0;
				samples = mp3dec_decode_frame(&ctx->mp3d, ctx->data_buffer + ctx->data_start, buffer_size, ctx->pcm,
                              &ctx->info);
				ctx->data_start += ctx->info.frame_bytes;

				ctx->last_byte_was_read = ctx->file.Tell() == ctx->file.GetSize();
			}
			else if (ctx->type == AudioContext::OGG)
			{
                float **buffer = nullptr;
                int bitstream = 0;
                int64_t framesRead = ov_read_float(ctx->ogg_file, &buffer, MINIMP3_MAX_SAMPLES_PER_FRAME, &bitstream);

	            if( !framesRead)
	            {
	            	ctx->last_byte_was_decoded = true;
	            	ctx->last_byte_was_read = true;
	            	break;
	            }

	            if (framesRead < 0)
	            {
	                continue;
	            }

	            for (int i = 0; i < framesRead; ++i)
	            {
	                for(int ch = 0; ch < ctx->info.channels; ch++)
	                {
						ctx->pcm[i * ctx->info.channels + ch] = short(buffer[ch][i] * 0xFFFF);
	                }
	            }
	            samples = framesRead;
			}

            ALenum format = ctx->info.channels == 1 ?  AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			alBufferData(ctx->buffers[ctx->current], format, ctx->pcm, ctx->info.channels * 2 * samples, ctx->info.hz);
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
				    ctx->data_start = 0;
				    ctx->data_end = 0;
					ctx->current = 0;
					ctx->last = 0;
					ctx->buffers_in_queue = 0;
					ctx->last_byte_was_read = false;
					ctx->last_byte_was_decoded = false;
					ctx->file.Seek(0);
				}
				else
				{
					// DeleteContext(*ctx);
					// delete m_contexts[i];
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
