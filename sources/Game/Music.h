#pragma once
#include "../Audio.h"

#include <spdlog/spdlog.h>
#include <map>
#include <string>

class Music
{
public:
	Music()
	{
		m_audio.Init();
	}

	void PlayInterface(const std::string& name)
	{
	    spdlog::info("Interface:{}", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/interface/" + name + ".ogg");
	}

	void PlayDialog(const std::string& name)
	{
	    spdlog::info("Dialog: {}", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/dialog/" + name + ".ogg");
	}

	void PlayEffect(const std::string& name)
	{
	    spdlog::info("Effect: {}", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/effect/" + name + ".ogg");
	}

	void PlayAmbient(const std::string& name)
	{
	    if (track_ambient_name == name) { return; }
	    track_ambient_name = name;
	    spdlog::info("Music: {}", name.c_str());
        m_audio.StopPlaying(m_track_ambient);
	    m_track_ambient = FetchAudio(name, "ambient", "audio/ambient/" + name + ".mp3", true);
	}

	AudioContext* FetchAudio(const std::string& name, const std::string& role, const std::string& src, bool loop = false)
	{
		fsal::FileSystem fs;
		auto file = fs.Open(src);
		if (file)
		{
	        return m_audio.PlayFile(file, loop);
		}
	}

	Audio m_audio;
	bool m_is_muted = false;

	AudioContext* m_track_ambient = nullptr;
	AudioContext* m_track_effect = nullptr;
	AudioContext* m_track_interface = nullptr;
	AudioContext* m_track_dialog = nullptr;

	std::string track_ambient_name;
};
