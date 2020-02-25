#include "Game.h"
#include "Oquonie.h"
#include "Player.h"
#include <spdlog/spdlog.h>
#include <stdio.h>


Game::Game()
{}

void Game::Save()
{
	spdlog::info("Saving..");
}

void Game::Load()
{
	spdlog::info("Loading..");
}

bool Game::IsFound()
{
	// if (localStorage.character && parseInt(localStorage.room) > 0) {
	return false;
}

void Game::New()
{
	spdlog::info("New Game..");

//	    Oquonie::GetInstance()->m_spellbook.reset()
//
	Oquonie::GetInstance()->m_player->m_location = 29;
	Oquonie::GetInstance()->m_player->SetId("necomedre");

	spdlog::info("Created a new game.");
}
