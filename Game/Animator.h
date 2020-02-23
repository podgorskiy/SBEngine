#pragma once
#include "Animation.h"
#include "Host.h"

#include <string>
#include <vector>
#include <map>
#include <set>


class Animator;
typedef std::shared_ptr<Animator> AnimatorPtr;


class Animator
{
public:
	Animator(Host* host): m_host(host), m_state("idle"), m_orientation(-1)
	{
	}

	void Add(const Animation& animation)
	{
        m_animations[animation.m_name] = animation;
        m_animations[animation.m_name].host = m_host;
	}

	void Animate()
	{
        if (m_animations.find(m_state) == m_animations.end()) { return; }

	    auto& anim = m_animations.find(m_state)->second;

//	    let width = $(this.host.element).width()
//	    let height = $(this.host.element).height()
		int width = 100;
		int height = 100;

		int frames = std::set<int>(anim.m_frames.begin(), anim.m_frames.end()).size();

		char buffer[1024];
		sprintf(buffer, "media/graphics/%s/%s%s.png", m_host->GetHostHame().c_str(),
		        (m_host->GetHostID() > 0 ? "%s." : ""), m_state.c_str());
		if (m_host->GetHostID() > 0)
		{
			sprintf(buffer, std::string(buffer).c_str(), m_host->GetHostID());
		}

	    std::string art_id = buffer;

        if (m_last_art_id != art_id)
        {
        	m_last_art_id = art_id;
        	// TODO: oquonie.artbook.set_art(this.host.element, art_id)
        }

        //this.host.element.style.backgroundSize = `${width * frames}px ${width * 1.5}px`
        //this.host.element.style.backgroundPosition = `${anim.run() * -width + width}px center`
	}

	void SetState(const std::string& new_state, bool update_immediately = true)
	{
        m_state = new_state;
        if (update_immediately)
        {
        	Animate();
        }
	}

	std::string m_state;
private:
	Host* m_host;
	int m_orientation;
	std::string m_last_art_id;
	std::map<std::string, Animation> m_animations;
};
