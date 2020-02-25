#pragma once
#include "../Audio.h"

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
	    printf("Interface: %s\n", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/interface/" + name + ".ogg");
	}

	void PlayDialog(const std::string& name)
	{
	    printf("Dialog: %s\n", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/dialog/" + name + ".ogg");
	}

	void PlayEffect(const std::string& name)
	{
	    printf("Effect: %s\n", name.c_str());
	    m_track_effect = FetchAudio(name, "effect", "audio/effect/" + name + ".ogg");
	}

	void PlayAmbient(const std::string& name)
	{
	    if (track_ambient_name == name) { return; }
	    track_ambient_name = name;
	    printf("Music: %s\n", name.c_str());
        m_audio.StopPlaying(m_track_ambient);
	    m_track_ambient = FetchAudio(name, "ambient", "audio/ambient/" + name + ".mp3", true);
	}

	AudioContext* FetchAudio(const std::string& name, const std::string& role, const std::string& src, bool loop = false)
	{
	    std::string audio_id = role + '_' + name;
	    if (audio_catalog.find(audio_id) == audio_catalog.end())
	    {
	    	fsal::FileSystem fs;
	        audio_catalog[audio_id] = m_audio.PlayFile(fs.Open(src), loop);
	    }
	    // this.audio_catalog[audio_id].currentTime = 0
	    return audio_catalog[audio_id];
	}

	Audio m_audio;
	bool m_is_muted = false;

	std::map<std::string, AudioContext*> audio_catalog;

	AudioContext* m_track_ambient = nullptr;
	AudioContext* m_track_effect = nullptr;
	AudioContext* m_track_interface = nullptr;
	AudioContext* m_track_dialog = nullptr;

	std::string track_ambient_name;
};

/*
function Music () {
  this.track_ambient = new Audio()
  this.track_effect = new Audio()
  this.track_interface = new Audio()
  this.track_dialog = new Audio()

  this.audio_catalog = {}

  this.is_muted = false
  this.track_ambient.volume = 0

  this.play_effect = function (name) {
  }

  this.play_interface = function (name) {
    console.log('Interface: ', name)
    this.track_interface = this.fetch_audio(name, 'interface', 'media/audio/interface/' + name + '.ogg')
    this.track_interface.play()
  }

  this.play_dialog = function (name) {
    console.log('Dialog: ', name)
    this.track_dialog = this.fetch_audio(name, 'dialog', 'media/audio/dialog/' + name + '.ogg')
    this.track_dialog.play()
  }

  this.play_ambient = function (name) {
    if (this.track_ambient.name == name) { return }
    if (DEBUG) { return }
    if (!this.track_ambient.name) { this.introduce(name); return }

    // Fadeout
    $(this.track_ambient).animate({ volume: 0 }, 1000, function () {
      console.log('Music: ', name)

      oquonie.music.track_ambient.pause()
      oquonie.music.track_ambient = oquonie.music.fetch_audio(name, 'ambient', 'media/audio/ambient/' + name + '.mp3', true)
      if (oquonie.music.is_muted == false) { oquonie.music.track_ambient.play() }
      $(oquonie.music.track_ambient).animate({ volume: 1 }, 1000)
    })
  }

  this.introduce = function (name) {
    oquonie.music.track_ambient.pause()
    oquonie.music.track_ambient = oquonie.music.fetch_audio(name, 'ambient', 'media/audio/ambient/' + name + '.mp3', true)
    this.track_ambient.volume = 0
    if (oquonie.music.is_muted == false) { oquonie.music.track_ambient.play() }
    $(oquonie.music.track_ambient).animate({ volume: 1 }, 1000)
  }

  this.fetch_audio = function (name, role, src, loop = false) {
    let audio_id = role + '_' + name
    if (!(audio_id in this.audio_catalog)) {
      let audio = new Audio()
      audio.name = name
      audio.src = src
      audio.loop = loop
      this.audio_catalog[audio_id] = audio
    }
    this.audio_catalog[audio_id].currentTime = 0
    return this.audio_catalog[audio_id]
  }

  this.pause_ambience = function () {
    this.is_muted = true

    $(this.track_ambient).animate({ volume: 0 }, 1000, function () {
      oquonie.music.track_ambient.pause()
    })
  }

  this.resume_ambience = function () {
    this.track_ambient.play()
    this.track_ambient.volume = 0
    $(this.track_ambient).animate({ volume: 1 }, 1000)
    this.is_muted = false
  }

  this.toggle_ambience = function () {
    if (this.track_ambient.volume == 1) {
      this.pause_ambience()
    } else {
      this.resume_ambience()
    }
  }
}
*/