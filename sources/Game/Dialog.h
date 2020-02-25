#pragma once
#include "Oquonie.h"
#include "Music.h"
#include <string>
#include <stdio.h>
#include <spdlog/spdlog.h>


class Dialog
{
public:
// this.element = this.element = document.createElement('dialog')
//
//  this.portrait = document.createElement('portrait')
//  this.bubble = document.createElement('bubble')
//  this.letter1 = document.createElement('letter')
//  this.letter2 = document.createElement('letter')
//  this.letter3 = document.createElement('letter')
	void Install()
	{
//    oquonie.element.appendChild(this.element)
//    this.element.appendChild(this.portrait)
//    this.element.appendChild(this.bubble)
//    this.bubble.appendChild(this.letter1)
//    this.bubble.appendChild(this.letter2)
//    this.bubble.appendChild(this.letter3)
	}

	void Show(const std::string& host_name, const std::vector<std::string>& message, std::string path = "", std::string audio_name = "")
	{
        spdlog::info("Dialog with: {}", host_name.c_str());

        if (path.empty())
        {
        	path = "graphics/" + host_name + "/";
        }
        if (audio_name.empty())
        {
        	audio_name = host_name;
        }

        //oquonie.artbook.set_art(this.portrait, path + 'portrait.png')
        //$(this.element).animate({ opacity: 1 }, oquonie.speed, function () {})

	    //oquonie.artbook.set_art(this.letter1, 'media/graphics/camilare/' + message[0] + '.png')
	    //oquonie.artbook.set_art(this.letter2, 'media/graphics/camilare/' + message[1] + '.png')
	    //oquonie.artbook.set_art(this.letter3, 'media/graphics/camilare/' + message[2] + '.png')

	    m_content = message;
        Oquonie::GetInstance()->m_music->PlayDialog(audio_name);
        Oquonie::GetInstance()->m_music->PlayInterface("interface.dialog.open");
	}

	void Hide()
	{
//    $(this.element).animate({ opacity: 0 }, oquonie.speed, function () {})
//    this.content = null
		m_content.clear();
        Oquonie::GetInstance()->m_music->PlayInterface("interface.dialog.close");
	}

	std::vector<std::string> m_content;
};
