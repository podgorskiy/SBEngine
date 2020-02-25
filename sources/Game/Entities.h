#pragma once
#include "Oquonie.h"
#include "Player.h"
#include "Music.h"
#include "Stage.h"
#include "Dialog.h"
#include <spdlog/spdlog.h>
#include <functional>
#include <algorithm>


class Blocker: public Event
{
public:
	Blocker(int x, int y, int id): Event("blocker"), m_id(id)
	{
		m_x = x;
		m_y = y;
        //if (this.id != 0) { oquonie.artbook.set_art(this.element, 'media/graphics/blocker/' + this.id + '.png') }
	}

	virtual bool IsCollider()
	{
		return true;
	}

	virtual bool ElicitsCollisionBump()
	{
		return m_id > 0;
	}

	virtual void OnCollision()
	{
        if (m_id != 0)
        {
        	Oquonie::GetInstance()->m_music->PlayEffect("bump.1");
        }
        // if (oquonie.player.location == 43) { this.rez_easteregg() }
	}
	int m_id;
};

class Plan: public Event
{
public:
	Plan(int x, int y, std::string): Event("plan")
	{}
};

class Gate: public Event
{
public:
	Gate(std::string requirement, int x, int y, int room, int to_x, int to_y):
		Event("gate"), m_requirement(requirement), m_room(room), m_to_x(to_x), m_to_y(to_y)
	{
		m_x = x;
		m_y = y;
	}

	virtual bool IsCollider()
	{
		return true;
	}

	virtual void OnCollision()
	{
	    if (m_requirement != Oquonie::GetInstance()->m_player->m_id && Oquonie::GetInstance()->m_player->m_id != "nastazie")
	    {
			spdlog::info("Gate requires: {}, is {}", m_requirement.c_str(), Oquonie::GetInstance()->m_player->m_id.c_str());
			Oquonie::GetInstance()->m_dialog->Show("owl", {"door", "locked", m_requirement});
			return;
	    }
	    Oquonie::GetInstance()->m_stage->EnterRoom(m_room, m_to_x, m_to_y);
        Oquonie::GetInstance()->m_music->PlayEffect("bump.2");
	}

	virtual void OnSight()
	{
	    int wall_id = Oquonie::GetInstance()->m_stage->WallAt(m_x, m_y);
	    if (wall_id != -1)
	    {
	        // oquonie.artbook.set_art('#wall_' + wall_id, 'media/graphics/wall/gate.' + this.requirement + '.' + (this.requirement == oquonie.player.id || oquonie.player.id == 'nastazie' ? 'open' : 'close') + '.png')
	    }
	}

	std::string m_requirement;
	int m_room;
	int m_to_x;
	int m_to_y;
};

class Door: public Event
{
public:
	struct Destination
	{
		std::function<bool()> fn;
		int room;
		int to_x;
		int to_y;
	};

	Door(int x, int y, int room, int to_x, int to_y):
		Event("door"), m_room(room), m_to_x(to_x), m_to_y(to_y)
	{
		m_x = x;
		m_y = y;
        AddDestination([]{ return true; }, room, to_x, to_y);
	}

	virtual bool IsCollider()
	{
		return true;
	}

	virtual void OnCollision()
	{
	    for (const auto& dest: m_destinations)
	    {
	        if (dest.fn())
	        {
	            Oquonie::GetInstance()->m_stage->EnterRoom(dest.room, dest.to_x, dest.to_y);
	            break;
	        }
        }
        Oquonie::GetInstance()->m_music->PlayEffect("bump.2");
	}

	void AddDestination(const std::function<bool()>& fn, int room, int to_x, int to_y)
	{
		m_destinations.push_back({ fn, room, to_x, to_y });
        std::rotate(m_destinations.rbegin(), m_destinations.rbegin() + 1, m_destinations.rend());
    }

	std::vector<Destination> m_destinations;

	int m_room;
	int m_to_x;
	int m_to_y;
};

class Red: public Event
{
public:
	Red(int, int, int): Event("red")
	{}
};



class Credit: public Event
{
public:
	Credit(int, int, std::string, std::vector<std::string>, std::string): Event("credit")
	{}
	Credit(int, int, std::string, std::vector<std::string>, bool): Event("credit")
	{}
	Credit(int, int, std::string, std::vector<std::string>): Event("credit")
	{}
};


class Pillar: public Event
{
public:
	Pillar(int, int, std::string, int): Event("pillar")
	{}

	Event* mat;
};

class PillarGate: public Event
{
public:
	PillarGate(int, int, int, int, int): Event("pillargate")
	{}
};

class PillarBase: public Event
{
public:
	PillarBase(int x, int y, std::string): Event("pillarbase")
	{}
};



class Teleport: public Event
{
public:
	Teleport(int, int, int): Event("teleport")
	{}
	Teleport(int, int, int, int, int): Event("teleport")
	{}
	Teleport(int, int, int, int, int, bool): Event("teleport")
	{}
};


class Ramen: public Event
{
public:
	Ramen(int, int): Event("ramen")
	{}
	Ramen(int, int, std::string): Event("ramen")
	{}

	Event* mat = new Event("ramen");
};

class Broken: public Event
{
public:
	Broken(int, int, int, int, int, int): Event("broken")
	{}
};


class Owl: public Event
{
public:
	Owl(int, int): Event("owl")
	{}
};

class HiversairesGate: public Event
{
public:
	HiversairesGate(int, int, int, int, int): Event("hiversaires_gate")
	{}
};


class HiversairesSauvegarde: public Event
{
public:
	HiversairesSauvegarde(int, int): Event("hiversaires")
	{}
};


class Petunia: public Event
{
public:
	Petunia(int, int): Event("petunia")
	{}
};


class Ghost: public Event
{
public:
	Ghost(int, int): Event("ghost")
	{}
};

class Shark: public Event
{
public:
	Shark(int, int): Event("shark")
	{}
	Shark(int, int, bool): Event("shark")
	{}
};

class Speaker: public Event
{
public:
	Speaker(int, int): Event("speaker")
	{}
	Speaker(int, int, std::string): Event("speaker")
	{}
};

class Noface: public Event
{
public:
	Noface(int, int): Event("noface")
	{}
};

class Wizard: public Event
{
public:
	Wizard(int, int, bool): Event("wizard")
	{}
	Wizard(int, int, std::string, bool): Event("wizard")
	{}
};

class Tree: public Event
{
public:
	Tree(int, int): Event("tree")
	{}
};

class Boss: public Event
{
public:
	Boss(int, int): Event("boss")
	{}
	Boss(int, int, bool): Event("boss")
	{}
};
