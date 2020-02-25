#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3.h"

#include <fsal.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <unistd.h>


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

int main()
{
	float     listenerVolume = 100.f;
	glm::vec3 listenerPosition (0.f, 0.f, 0.f);
	glm::vec3 listenerDirection(0.f, 0.f, -1.f);
	glm::vec3 listenerUpVector (0.f, 1.f, 0.f);

	ALCdevice* device;

	device = alcOpenDevice(nullptr);
	if (!device)
	{
		printf("Error\n");
	}

	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

	ALCcontext *context;

	context = alcCreateContext(device, NULL);
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

    unsigned int source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, 0);


    constexpr int buffers_count = 20;
    ALuint buffers[buffers_count];
    alGenBuffers(buffers_count, buffers);

    std::vector<uint8_t> soundData;

    static mp3dec_t mp3d;
    mp3dec_init(&mp3d);

    mp3dec_frame_info_t info;
    short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

    fsal::FileSystem fs;
    auto file = fs.Open("/home/stpidhorskyi/Utsu-P - ALGORITHM 4th FULL ALBUM HD.mp3");

    uint8_t data_buffer[1024 * 16 * 2];
    int data_start = 0;
    int data_end = 0;

	int current = 0;
	int last = 0;
	int buffers_in_queue = 0;

	bool terminate = false;
	for (int i = 0; !terminate; ++i)
	{
		while (buffers_in_queue < buffers_count)
		{
			size_t read_bytes = 0;
			size_t size_to_read = 1024 * 16 - (data_end - data_start);

			if (size_to_read > 0)
			{
				if (size_to_read + data_end > 1024 * 16 * 2)
				{
					size_t size = data_end - data_start;
					memmove(data_buffer, data_buffer + data_start, size);
					data_end = size;
					data_start = 0;
				}

				file.Read(data_buffer + data_end, size_to_read, &read_bytes);
				data_end += read_bytes;
			}

			size_t buffer_size = data_end - data_start;
			int samples = mp3dec_decode_frame(&mp3d, data_buffer + data_start, buffer_size, pcm, &info);
			data_start += info.frame_bytes;

			terminate = file.Tell() == file.GetSize();
			if (terminate)
				break;

            ALenum format = info.channels == 1 ?  AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			alBufferData(buffers[current], format, pcm, 4 * samples, info.hz);
			alSourceQueueBuffers(source, 1, buffers + current);

			++buffers_in_queue;
			++current;
			current = current % buffers_count;
		}
		ALint state = 0;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING)
			alSourcePlay(source);

		int processed = 0;
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

		for (int i = 0; i < processed; ++i)
		{
			alSourceUnqueueBuffers(source, 1, buffers + last);
			++last;
			last = last % buffers_count;
			--buffers_in_queue;
		}
		usleep(200000);
	}

    ALint state = AL_PLAYING;

    while(state == AL_PLAYING)
    {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }

	alcCloseDevice(device);
}
