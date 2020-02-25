#pragma once
#include <fsal.h>
#include <vector>


typedef struct ALCdevice_struct ALCdevice;
struct AudioContext;

class Audio
{
public:
	void Init();
	void Destroy();

	AudioContext* PlayFile(fsal::File file, bool loop = false);

	void Update();
	void StopPlaying(AudioContext* ctx);
private:
	void InitContext(AudioContext& ctx);
	void DeleteContext(AudioContext& ctx);

	std::vector<AudioContext*> m_contexts;

	float listenerVolume = 100.f;
	ALCdevice* m_device;
};
