#include "Game.h"
#include "Oquonie.h"
#include "Player.h"
#include <stdio.h>


Game::Game()
{}

void Game::Save()
{
	printf("Saving..\n");
}

void Game::Load()
{
	printf("Loading..\n");
}

bool Game::IsFound()
{
	// if (localStorage.character && parseInt(localStorage.room) > 0) {
	return false;
}

void Game::New()
{
	printf("New Game..\n");

//	    Oquonie::GetInstance()->m_spellbook.reset()
//
	Oquonie::GetInstance()->m_player->m_location = 29;
	Oquonie::GetInstance()->m_player->SetId("necomedre");

	printf("Created a new game.\n");
}
