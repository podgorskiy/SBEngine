#include "Game.h"
#include "Oquonie.h"
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
//	    Oquonie::GetInstance()->m_player.location = 29
//	    Oquonie::GetInstance()->m_player.set_id('necomedre')

	printf("Created a new game.\n");
}
