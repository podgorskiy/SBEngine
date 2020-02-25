#include "Stage.h"
#include "Oquonie.h"
#include "World.h"
#include "Music.h"


Stage::Stage(): m_room(nullptr)
{
	//this.element = document.createElement('stage')
	//this.parallax_over = document.createElement('parallax')
	//this.parallax_under = document.createElement('parallax')
}

void Stage::Install()
{
//	    oquonie.element.appendChild(this.element)
//	    this.element.appendChild(this.parallax_over)
//	    this.element.appendChild(this.parallax_under)
//	    this.parallax_over.setAttribute('class', 'over')
//	    this.parallax_under.setAttribute('class', 'under')
}

void Stage::EnterRoom(int room_id, int x, int y)
{
	printf("Entering Room: %d\n", room_id);

	auto world = Oquonie::GetInstance()->m_world;

	if (world->m_rooms.find(room_id) == world->m_rooms.end())
	{
		printf("Missing room:(%d)\n", room_id);
		return;
	}

//    $(this.element).finish()
//    $(this.parallax_over).finish()
//    $(this.parallax_under).finish()
//    oquonie.player.stand_by_door(x, y)

    if (m_room)
    {
    	//m_room.element).empty();
    	//m_room.element).remove();
    }

    m_room = world->m_rooms[room_id];

    //this.element.appendChild(this.room.element)

    m_room->Show();

    m_room->m_is_known = true;

    Oquonie::GetInstance()->m_player->m_location = room_id;
    Oquonie::GetInstance()->m_player->MoveAt(x, y);

    int numPillars = 0; //Oquonie::GetInstance()->m_spellbook.pillars.size();
    // std::string theme = (numPillars >= 5 && m_room.theme != "pillars") ? "black" : m_room.theme;

    // SetTheme(theme);

    Look();

    Center(Oquonie::GetInstance()->m_player->m_x, Oquonie::GetInstance()->m_player->m_y);

    // $(this.element).css('opacity', 0)
    // $(this.element).animate({ opacity: '1' }, oquonie.speed / 2)

    auto audio = m_room->m_audio;
    if (audio == "lobby")
    {
        if (numPillars >= 5)
        {
            audio = "lobby.3";
        } else if (numPillars > 0)
        {
            audio = "lobby.2";
        }
    }

    Oquonie::GetInstance()->m_music->PlayAmbient(audio);
}

std::vector<EventPtr> Stage::TilesAt(int x, int y)
{
    std::vector<EventPtr> tiles;
    for (int i = 0; i < m_room->m_events.size(); ++i)
    {
        EventPtr tile = m_room->m_events[i];
        if (tile->m_x == x && tile->m_y == y)
        {
        	tiles.push_back(tile);
        }
    }
    return tiles;
}

int Stage::FloorAt(int x, int y)
{
    if (x == -1 && y == 1) { return m_room->m_floors[0]; }
    if (x == 0 && y == 1) { return m_room->m_floors[1]; }
    if (x == 1 && y == 1) { return m_room->m_floors[2]; }

    if (x == -1 && y == 0) { return m_room->m_floors[3]; }
    if (x == 0 && y == 0) { return m_room->m_floors[4]; }
    if (x == 1 && y == 0) { return m_room->m_floors[5]; }

    if (x == -1 && y == -1) { return m_room->m_floors[6]; }
    if (x == 0 && y == -1) { return m_room->m_floors[7]; }
    if (x == 1 && y == -1) { return m_room->m_floors[8]; }
    return -1;
}


int Stage::WallAt(int x, int y)
{
    if (x == -1 && y == 2) { return 0; }
    if (x == 0 && y == 2) { return 1; }
    if (x == 1 && y == 2) { return 2; }

    if (x == 2 && y == 1) { return 3; }
    if (x == 2 && y == 0) { return 4; }
    if (x == 2 && y == -1) { return 5; }
    return -1;
}


void Stage::Animate(int x, int y)
{
	Center(x, y);
}


void Stage::Center(int x, int y)
{
    int xSlant = x - y;
    int ySlant = -x - y;
    // this.move_parallax(this.element, 0.5, xSlant, ySlant)
    // this.move_parallax(this.parallax_over, 1.0, xSlant, ySlant)
    // this.move_parallax(this.parallax_under, 0.125, xSlant, ySlant)
}

void Stage::Look()
{
    for (int i = 0; i < m_room->m_events.size(); ++i)
    {
		m_room->m_events[i]->OnSight();
    }
}
