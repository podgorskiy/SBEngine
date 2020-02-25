#include "Oquonie.h"
#include "World.h"
#include "Stage.h"
#include "Game.h"
#include "Music.h"
#include "Walkthrough.h"


Oquonie* Oquonie::GetInstance()
{
	static Oquonie instance;
	return &instance;
}

Oquonie::Oquonie()
{
//		m_artbook = new Artbook();
	m_game = std::make_shared<Game>();
	m_world = std::make_shared<World>();
	m_music = std::make_shared<Music>();
	m_dialog = std::make_shared<Dialog>();
//		m_overlay = new Overlay();
//		m_interface = new Interface();
	m_stage = std::make_shared<Stage>();
	m_player = std::make_shared<Player>();
//		m_spellbook = new Spellbook();
	m_walkthrough = std::make_shared<Walkthrough>();
	m_animation_timer = nullptr;
	m_started = false;
	speed = 0;
}

void Oquonie::Install()
{
	// TODO document.body.appendChild(m_element)

	m_world->Install();
	m_dialog->Install();
//	    m_overlay.install()
//	    m_interface.install()
	m_stage->Install();
//	    m_spellbook.install()
}

void Oquonie::Start()
{
	printf("Starting Oquonie\n");
	// TODO this.element.style.opacity = 1

	m_walkthrough->Start();
//		m_spellbook->Hide();

	Animate();
	m_started = true;
}

void Oquonie::Reset()
{
	m_game->New();
	m_stage->EnterRoom(25, 0, 0);
}

void Oquonie::Animate()
{
	// m_animation_timer = setTimeout(function () { animate() }, 200)

	m_player->m_animator->Animate();

	for (int i = 0; i < m_stage->m_room->m_events.size(); ++i)
	{
		m_stage->m_room->m_events[i]->m_animator->Animate();
	}
}

void Oquonie::StopAnimation()
{
	// clearTimeout(this.animation_timer)
}
