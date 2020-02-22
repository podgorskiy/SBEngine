#pragma once
#include "World.h"
#include "Game.h"
#include "Stage.h"
#include "Walkthrough.h"
#include <stdio.h>

class Oquonie
{
public:
	static Oquonie* GetInstance()
	{
		static Oquonie instance;
		return &instance;
	}

	Oquonie()
	{
//		m_artbook = new Artbook();
		m_game = std::make_shared<Game>();
		m_world = std::make_shared<World>();
//		m_music = new Music();
//		m_dialog = new Dialog();
//		m_overlay = new Overlay();
//		m_interface = new Interface();
		m_stage = std::make_shared<Stage>();
//		m_player = new Player();
//		m_spellbook = new Spellbook();
		m_walkthrough = std::make_shared<Walkthrough>();
		m_animation_timer = nullptr;
		m_started = false;
		speed = 0;
	}

	void Install()
	{
		// TODO document.body.appendChild(m_element)
		
	    m_world->Install();
//	    m_dialog.install()
//	    m_overlay.install()
//	    m_interface.install()
//	    m_stage.install()
//	    m_spellbook.install()
	}

	void Start()
	{
		printf("Starting Oquonie\n");
		// TODO this.element.style.opacity = 1

		m_walkthrough->Start();
//		m_spellbook->Hide();

		Animate();
		m_started = true;
	}

	void Reset()
	{
	    m_game->New();
        // m_stage.enter_room(25, 0, 0);
	}

	void Animate()
	{
		// m_animation_timer = setTimeout(function () { animate() }, 200)

		//m_player.animator.animate();

	    //for (int i = 0; i < m_stage->room->events.size(); ++i)
	    {
			//m_stage->room->events[i].animator.animate();
	    }
	}

	void StopAnimation()
	{
        // clearTimeout(this.animation_timer)
	}

	int speed;
	GamePtr m_game;
	WorldPtr m_world;
	StagePtr m_stage;
private:
//	ArtbookPtr m_artbook;
//	MusicPtr m_music;
//	DialogPtr m_dialog;
//	OverlayPtr m_overlay;
//	InterfacePtr m_interface;
//	PlayerPtr m_player;
//	SpellbookPtr m_spellbook;
	WalkthroughPtr m_walkthrough;

	void* m_animation_timer;
	bool m_started;
};
