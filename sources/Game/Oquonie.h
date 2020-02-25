#pragma once
#include <memory>
#include <stdio.h>


class Game;
typedef std::shared_ptr<Game> GamePtr;

class Music;
typedef std::shared_ptr<Music> MusicPtr;

class World;
typedef std::shared_ptr<World> WorldPtr;

class Stage;
typedef std::shared_ptr<Stage> StagePtr;

class Player;
typedef std::shared_ptr<Player> PlayerPtr;

class Walkthrough;
typedef std::shared_ptr<Walkthrough> WalkthroughPtr;


class Oquonie
{
public:
	static Oquonie* GetInstance();

	Oquonie();

	void Install();

	void Start();

	void Reset();

	void Animate();

	void StopAnimation();

	int speed;
	GamePtr m_game;
	WorldPtr m_world;
	StagePtr m_stage;
	PlayerPtr m_player;
	MusicPtr m_music;
private:
//	ArtbookPtr m_artbook;
//	DialogPtr m_dialog;
//	OverlayPtr m_overlay;
//	InterfacePtr m_interface;
//	SpellbookPtr m_spellbook;
	WalkthroughPtr m_walkthrough;

	void* m_animation_timer;
	bool m_started;
};
