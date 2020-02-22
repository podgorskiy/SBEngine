#include "Walkthrough.h"
#include "Oquonie.h"

/*
  let U = 'U'; let D = 'D'; let L = 'L'; let R = 'R'

  // Chapter 0

  let tutorial = [U, U, U, L, L, D, R, R, R, U, R, U, L, L, D, D, D, R, R, R, R, R, R]

  // Chapter 1

  let necomedre_lobby = [R, R, R, U, U, U, L, L, L, D, D, D, R, R, R]
  let necomedre_stage = [D, D, D, R, R, U, L, L, U, R, U, U, R, R, R, U, R, D, D, D, R, R, R, R, R, U, R, R, R]
  let necomedre = necomedre_lobby.concat(necomedre_stage)

  let nephtaline_lobby = [U, U]
  let nephtaline_stage = [U, U, R, R, R, R, R, D, D, D, D, D, L, L, L, L, L, U, U, U, U, U, R, R, D, D, U, R, L, D, L, D, D, D, U, D, D, L, L, L, L, U, U, U, R, U, R, R, R, U, U, U]
  let nephtaline = nephtaline_lobby.concat(nephtaline_stage)

  let neomine_lobby = [R, R, R, U, U, U, U, U]
  let neomine_stage = [U, R, D, R, R, U, U, D, L, L, U, U, R, D, D, L, L, L, U, U, D, R, R, U, R, D, L, L, U, U, D, R, R, U, U, L, D, D, R, U, L, L, U, R, R, D, L, U, R, R, R, R, R]
  let neomine = neomine_lobby.concat(neomine_stage)

  let nestorine_lobby = [R, R, R, U, U, U, L, L, L, D, D, D, D, D, D, R, R, R, R, R, R]
  let nestorine_stage = [D, D, D, D, D, D, L, L, U, L, L, L, L, L, L, U, L, D, D, R, D, D, D, D, D, D, D, D, D, D, L, L, L, L, L, L, D, D, R, D, R, R, R]
  let nestorine = nestorine_lobby.concat(nestorine_stage)

  let nemedique_lobby = [R, R, R, U, U, U, L, L, L, D, D, D, L, L, L, U, U, U]
  let nemedique_stage = [R, R, R, U, R, U, U, U, D, D, D, R, R, R, R]
  let nemedique = nemedique_lobby.concat(nemedique_stage)

  // Chapter 2

  let nephtaline_pillar_stage = [R, R, R, R, R, R, D, D, D, R, R, U, L, L, U, U, U, R, R, R, U, R, D, D, D, R, R, R, R, R, U, L, L, L, U, R, R, R, R, R, R, R, R, R, R, L, L, L, U, U]
  let nephtaline_pillar_pillar = [U, U, R, R, R, R, R, D, D, D, D, D, L, L, U, U, D, L, L, L, U, U, U, U, U, R, R, R, L, L, D, D, D, U, U, U, U, U]

  let nephtaline_pillar = []
  nephtaline_pillar = nephtaline_pillar.concat(nephtaline_pillar_stage)
  nephtaline_pillar = nephtaline_pillar.concat(nephtaline_pillar_pillar)

  let necomedre_pillar_stage = [R, R, R, U, U, U, L, L, L, D, D, D, L, L, L, U, U, U, R, R, R, U, U, U, U, D, D, D, L, L, L, L, U, R, R, R, R, R, R, R, R, R, R, U, U, U, R, R, R, D, D, D, R, U, U, U, L, L, L, D, D, D, R, R, R]
  let necomedre_pillar_pillar = [R, R, R, R, R, U, R, R, U, U, U]

  let necomedre_pillar = []
  necomedre_pillar = necomedre_pillar.concat(necomedre_pillar_stage)
  necomedre_pillar = necomedre_pillar.concat(necomedre_pillar_pillar)

  let nestorine_pillar_stage = ['Tneomine', U, U, U, R, R, R, U, U, U, R, D, R, R, U, U, D, L, L, U, U, R, D, D, L, U, R, D, L, L, U, U, D, R, R, U, U, L, D, D, R, U, L, L, U, R, R, U, L, D, R, R, R, L, U, L, D, R, R, R, R, R, R, R, R, R, U, U, U, L, L, L, D, D, D, D, D, D, R, R, R, R, R, R]
  let nestorine_pillar_pillar = [D, D, D, L, L, L, D, D, D, L, L, U, L, L, L, L, U, U, U]

  let nestorine_pillar = []
  nestorine_pillar = nestorine_pillar.concat(nestorine_pillar_stage)
  nestorine_pillar = nestorine_pillar.concat(nestorine_pillar_pillar)

  let neomine_pillar_stage = [L, L, L, U, U, U, U, U, U, R, R, R, D, D, D, L, L, L, U, U, U, U, R, R, R, R, R, D, D, D, D, D, L, L, L, L, L, U, U, U, U, U, R, R, D, D, U, R, L, D, L, D, D, D, U, D, D, L, L, L, U, U, U, R, R, R, R, L, L, U, R, R, D, D, U, D, D, L, L, L, U, U, U, R, R, R, U, U, U, R, R, R, R, U, U, U, U, U, U, R, U, R, R, R, D, R, R, R, R]
  let neomine_pillar_pillar = []

  let neomine_pillar = []
  neomine_pillar = neomine_pillar.concat(neomine_pillar_stage)
  neomine_pillar = neomine_pillar.concat(neomine_pillar_pillar)

  // Chapter 3

  let nastazie_pillar_begin = ['W3', L, L, L, U, U, U, U, U, L, U, R, U, R, R, L, L, L, U, U, D, R, R, R, R, R, D, U, D, L, U, L, L, L, U, U, D, L, L, D, R, R, R, R, R, U, L, U, U, U, U]
  let nastazie_pillar_neomine = [U, R, D, D, D, D, D, L, L, L, U, U, U, U, L, L, U, R, R, R, U, U, U, U, U, U]
  let nastazie_pillar_nephtaline = [U, R, D, D, D, D, L, L, L, L, U, U, U, U, L, D, L, D, D, R, D, D, D, R, R, R, U, L, L, U, U, U, U, U, R, U, U]
  let nastazie_pillar_nastazie = [U, R, D, D, D, D, D, L, D, R, R, R, R, R, U, L, L, L, L, U, U, U, D, R, D, D, D, D, D, L, D, R, R, D, R, R, R, R]

  let secrets = ['W3', R, R, R, R, 'W46', U, U, U, U, 'W102', R, R, R, U, 'W60', 'Tnastazie', D, R, R, R, U, U, U, U, U, R, R, R, R, 'W142', R, R, R, 'Tnastazie', 'W68', U, U, L, R, U]

  let catfishbird = ['W5', 'Tnecomedre', R, R, R, D, D, D, R, R, U, L, L, L, L, L, L, L, L, U, R, D, D, D, R, R, R, R, R, U, L, L, L, U, R, R, R, R, R, R, R, R, R, R, L, L, L, U, U, U, U, R, R, R, R, R, D, D, D, D, D, L, L, U, U, D, L, L, L, U, U, U, U, U, R, R, R, L, L, U, U, U, D, D, D, D, D, U, U, U, U, U, R, R, R, U, U, U, R, R, R, R, L, L, L, L, L, L, D, D, D, L, L, L, U, U, U, R, R, R, U, U, U, R, R, R, U, L, L, L, U]

  // Everything

  let full = []

  full = []
  full = full.concat(tutorial)
  let chapter_0 = full

  full = []
  full = full.concat(necomedre)
  full = full.concat(nephtaline)
  full = full.concat(neomine)
  full = full.concat(nestorine)
  full = full.concat(nemedique)
  let chapter_1 = full

  full = []
  full = full.concat(nephtaline_pillar)
  full = full.concat(necomedre_pillar)
  full = full.concat(nestorine_pillar)
  full = full.concat(neomine_pillar)
  let chapter_2 = full

  full = []
  full = full.concat(catfishbird)
  full = full.concat(nastazie_pillar_begin)
  full = full.concat(nastazie_pillar_neomine)
  full = full.concat(nastazie_pillar_nephtaline)
  full = full.concat(nastazie_pillar_nastazie)
  let chapter_3 = full

  let chapter_all = []
  chapter_all = chapter_all.concat(chapter_0)
  chapter_all = chapter_all.concat(chapter_1)
  chapter_all = chapter_all.concat(chapter_2)
  chapter_all = chapter_all.concat(chapter_3)
  chapter_all = chapter_all.concat(secrets)

  let counter = 0
  let section = ''
*/

void Walkthrough::Start()
{
	printf("Walkthrough has started.\n");
	Oquonie::GetInstance()->speed = 50;
	Release();
}

void Walkthrough::Release()
{
	Oquonie::GetInstance()->speed = 300;

	if (Oquonie::GetInstance()->m_game->IsFound())
	{
		printf("Found a saved game!\n");
		Oquonie::GetInstance()->m_game->Load();
	}
	else
	{
		printf("Found no saved game?\n");
		Oquonie::GetInstance()->m_game->New();
	}

	//Oquonie::GetInstance()->m_stage.EnterRoom(Oquonie::GetInstance()->m_player.location)
}
