#pragma once
#include "IAudioStream.h"
#include "SineWave.h"
#include "MP3Stream.h"
#include "OGGStream.h"
#include <fsal.h>
#include <vector>


typedef struct ALCdevice_struct ALCdevice;
struct PlaybackContext;

namespace Audio
{
	class AudioContext
	{
	public:
		void Init();

		void Destroy();

		PlaybackContext* PlayFile(fsal::File file, bool loop = false);
		PlaybackContext* PlayStream(const AudioStream& stream, bool loop = false);

		void Update();

		void StopPlaying(PlaybackContext* ctx);

		void Reset(PlaybackContext* ctx);

	private:
		void InitContext(PlaybackContext& ctx);

		void DeleteContext(PlaybackContext& ctx);

		std::vector<PlaybackContext*> m_contexts;

		float listenerVolume = 100.f;
		ALCdevice* m_device;
	};
}
