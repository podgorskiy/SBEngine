#pragma once
#include "Tile.h"
#include <memory>

struct LocalStorage
{
    std::string character;
    int room;

    bool ramen_necomedre;
    bool ramen_nephtaline;
    bool ramen_neomine;
    bool ramen_nestorine;
    bool ramen_nemedique;
    bool ramen_nastazie;

    bool pillar_necomedre;
    bool pillar_nephtaline;
    bool pillar_neomine;
    bool pillar_nestorine;
    bool pillar_nemedique;
    bool pillar_nastazie;

    bool is_muted;
};


class Game;
typedef std::shared_ptr<Game> GamePtr;


class Game
{
public:
	Game();

	void Save();

	void Load();

	bool IsFound();

	void New();
};
