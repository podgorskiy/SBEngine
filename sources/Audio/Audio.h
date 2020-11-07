#pragma once
#include "IAudioStream.h"
#include <fsal.h>

#define AUDIO_SINGLETON

namespace Audio
{
	struct PlaybackContext;
	struct AudioImplementation;

	class AudioContext
	{
	public:
		AudioContext();

		void Init();

		void Destroy();

		PlaybackContext* PlayFile(fsal::File file, bool loop = false);
		PlaybackContext* PlayFile(fsal::Location file, bool loop = false);
		PlaybackContext* PlayStream(const AudioStream& stream, bool loop = false);

		void Update();

		void StopPlaying(PlaybackContext* ctx);

		void Reset(PlaybackContext* ctx);

	private:
		void InitContext(PlaybackContext& ctx);

		void DeleteContext(PlaybackContext& ctx);
		std::shared_ptr<AudioImplementation> m_impl;
	};
}
