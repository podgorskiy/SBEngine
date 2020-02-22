#pragma once
#include "Room.h"
#include <map>


class Blocker: public Event
{
public:
	Blocker(int x, int y, int id): Event("blocker")
	{}
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
	Gate(std::string, int, int, int, int, int): Event("gate")
	{}
};

class Door: public Event
{
public:
	Door(int, int, int, int, int): Event("door")
	{}
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


class World;
typedef std::shared_ptr<World> WorldPtr;


class World
{
public:
	void Install()
	{
	    CreateLobby();
	    CreateNecomedre();
	    CreateNephtaline();
	    CreateNeomine();
	    CreateNestorine();
	    CreateNemedique();
	    CreateNastazie();
	    CreateSecrets();
	}

	void CreateLobby()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(1);
		room->m_floors = {0, 25, 2, 2, 8, 4, 2, 2, 2};
		room->m_walls = {0, 5, 2, 2, 12, 15};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 10));
		room->AddEvent(new PillarBase(1, -1, "nephtaline"));
		room->AddEvent(new Gate("nephtaline", 0, 2, 40, -1, -1)); // 1 - 40
		room->AddEvent(new Door(2, 0, 2, -1, 0));
		m_rooms[1] = room;

		room = std::make_shared<Room>(2);
		room->m_floors = {1, 4, 1, 5, 5, 1, 1, 1, 1};
		room->m_walls = {16, 12, 19, 25, 25, 25};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 24));
		Ramen* ramen = new Ramen(1, 0);
		room->AddEvent(ramen->mat);
		room->AddEvent(ramen);
		room->AddEvent(new Plan(-1, 2, "lobby"));
		room->AddEvent(new Door(-2, 0, 1, 1, 0));
		room->AddEvent(new Door(0, 2, 3, 0, -1));
		m_rooms[2] = room;

		room = std::make_shared<Room>(3);
		room->m_floors = {1, 23, 1, 6, 5, 1, 1, 4, 1};
		room->m_walls = {1, 7, 1, 1, 1, 1};
		room->m_steps = {0, 7, 0, 0, 7, 0};
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new PillarBase(-1, 1, "neomine"));
		room->AddEvent(new Owl(1, 1), true);
		room->AddEvent(new Blocker(-1, -1, 1));
		room->AddEvent(new Gate("neomine", 0, 2, 60, 0, -1)); // 3 - 60
		room->AddEvent(new Door(-2, 0, 4, 1, 0));
		room->AddEvent(new Door(0, -2, 2, 0, 1));
		room->AddEvent(new Broken(2, 0, 1, 148, -1, 0));
		m_rooms[3] = room;

		room = std::make_shared<Room>(4);
		room->m_floors = { 1, 1, 1, 1, 5, 6, 1, 6, 1 };
		room->m_walls = { 26, 14, 26, 1, 12, 1 };
		room->m_steps = { 0, 0, 0, 0, 7, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 9));
		room->AddEvent(new PillarBase(1, -1, "nastazie"));
		room->AddEvent(new Speaker(-1, 1), true);
		room->AddEvent(new Door(0, 2, 14, 0, -1));
		room->AddEvent(new Door(2, 0, 3, -1, 0));
		room->AddEvent(new Door(0, -2, 5, 0, 1));
		m_rooms[4] = room;

		room = std::make_shared<Room>(5);
		room->m_floors = { 9, 10, 9, 4, 5, 14, 9, 6, 9 };
		room->m_walls = { 3, 14, 3, 3, 4, 3 };
		room->m_steps = { 0, 7, 0, 0, 7, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new PillarBase(1, 1, "necomedre"));
		room->AddEvent(new Blocker(-1, 1, 30));
		room->AddEvent(new Door(0, 2, 4, 0, -1));
		room->AddEvent(new Gate("necomedre", 2, 0, 32, -1, 0)); // 5 - 32
		room->AddEvent(new Door(-2, 0, 9, 1, 0));
		room->AddEvent(new Door(0, -2, 6, 0, 1));
		m_rooms[5] = room;

		room = std::make_shared<Room>(6);
		room->m_floors = { 9, 4, 9, 10, 5, 6, 9, 10, 9 };
		room->m_walls = { 3, 13, 3, 26, 13, 26 };
		room->m_steps = { 0, 0, 0, 0, 0, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 5));
		room->AddEvent(new Shark(1, -1, false));
		room->AddEvent(new Door(0, 2, 5, 0, -1));
		room->AddEvent(new Door(2, 0, 7, -1, 0));
		m_rooms[6] = room;

		room = std::make_shared<Room>(7);
		room->m_floors = { 9, 10, 0, 4, 5, 12, 9, 0, 0 };
		room->m_walls = { 19, 0, 0, 0, 8, 0 };
		room->m_steps = { 0, 7, 0, 0, 0, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new PillarBase(-1, -1, "nestorine"));
		room->AddEvent(new Blocker(0, 1, 11));
		room->AddEvent(new Gate("nestorine", 2, 0, 96, -1, 0)); // 7 - 96
		room->AddEvent(new Door(-2, 0, 6, 1, 0));
		m_rooms[7] = room;

		// 8

		room = std::make_shared<Room>(9);
		room->m_floors = { 10, 27, 10, 9, 4, 5, 0, 6, 10 };
		room->m_walls = { 18, 9, 18, 3, 13, 3 };
		room->m_steps = { 0, 0, 0, 0, 7, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new PillarBase(-1, 1, "nemedique"));
		room->AddEvent(new Gate("nemedique", 0, 2, 100, 0, -1)); // 9 - 100
		room->AddEvent(new Door(2, 0, 5, -1, 0));
		room->AddEvent(new Door(0, -2, 11, 0, 1));
		m_rooms[9] = room;

		// 10

		room = std::make_shared<Room>(11);
		room->m_floors = { 4, 5, 6, 5, 33, 4, 4, 6, 5 };
		room->m_walls = { 36, 13, 36, 3, 37, 3 };
		room->m_steps = { 0, 0, 0, 0, 0, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 9, 0, -1));
		room->AddEvent(new Tree(0, 0));
		m_rooms[11] = room;

		// 12-13

		room = std::make_shared<Room>(14);
		room->m_floors = { 0, 0, 0, 0, 5, 0, 0, 5, 0 };
		room->m_walls = { 0, 0, 0, 0, 0, 0 };
		room->m_steps = { 0, 0, 0, 0, 7, 0 };
		room->m_audio = "lobby";
		room->m_theme = "white";
		room->AddEvent(new Noface(0, 1), true);
		room->AddEvent(new Door(0, -2, 4, 0, 1));
		m_rooms[14] = room;

		room = std::make_shared<Room>(15);
		room->m_floors = { 98, 7, 98, 98, 98, 97, 97, 98, 98 };
		room->m_walls = { 15, 97, 1, 98, 0, 0 };
		room->m_steps = { 0, 0, 0, 0, 98, 0 };
		room->m_audio = "office";
		room->m_theme = "tear";
		room->AddEvent(new Teleport(0, 1, 29, 0, 0, true));
		room->AddEvent(new Blocker(1, 1, 15), true);
		room->AddEvent(new Door(0, -2, 6, 0, -1));
		room->AddEvent(new Boss(1, -1, true));
		m_rooms[15] = room;

		// 16-20
	}

	void CreateNecomedre()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(21);
		room->m_floors = {21, 21, 21, 21, 21, 21, 21, 21, 21};
		room->m_walls = {20, 20, 20, 24, 21, 20};
		room->m_steps = {0, 0, 0, 10, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(0, 0, 14));
		room->AddEvent(new Door(2, 1, 24, -1, 1));
		room->AddEvent(new Door(-1, -2, 22, 1, 1));
		m_rooms[21] = room;

		room = std::make_shared<Room>(22);
		room->m_floors = {21, 21, 21, 21, 21, 21, 21, 21, 21};
		room->m_walls = {24, 20, 20, 20, 21, 20};
		room->m_steps = {0, 0, 0, 0, 10, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 15));
		room->AddEvent(new Blocker(-1, 0, 16));
		room->AddEvent(new Door(-1, 2, 21, -1, -1));
		room->AddEvent(new Door(0, -2, 23, 0, 1));
		m_rooms[22] = room;

		room = std::make_shared<Room>(23);
		room->m_floors = {21, 21, 21, 21, 17, 21, 21, 21, 21};
		room->m_walls = {19, 1, 19, 19, 1, 19};
		room->m_steps = {0, 10, 0, 0, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Wizard(-1, 1, "document"), true);
		room->AddEvent(new Blocker(1, 1, 15));
		room->AddEvent(new Blocker(1, -1, 15));
		room->AddEvent(new Door(-2, 0, 28, 1, 0));
		m_rooms[23] = room;

		room = std::make_shared<Room>(24);
		room->m_floors = {21, 21, 21, 21, 21, 21, 21, 21, 21};
		room->m_walls = {20, 24, 20, 20, 24, 21};
		room->m_steps = {10, 0, 0, 0, 10, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 14));
		room->AddEvent(new Blocker(-1, -1, 16));
		room->AddEvent(new Door(0, 2, 23, 0, -1));
		room->AddEvent(new Door(2, 0, 27, -1, 0));
		room->AddEvent(new Door(-2, 1, 21, 1, 1));
		room->AddEvent(new Door(0, -2, 25, 0, 1));
		m_rooms[24] = room;

		room = std::make_shared<Room>(25);
		room->m_floors = {21, 21, 21, 21, 18, 21, 21, 21, 21};
		room->m_walls = {21, 20, 20, 20, 24, 20};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 15));
		room->AddEvent(new Blocker(1, 1, 16));
		room->AddEvent(new Wizard(1, -1, "document"));
		room->AddEvent(new Door(2, 0, 28, -1, 0));
		m_rooms[25] = room;

		room = std::make_shared<Room>(26);
		room->m_floors = {21, 21, 21, 21, 21, 21, 21, 21, 21};
		room->m_walls = {20, 24, 20, 20, 20, 21};
		room->m_steps = {0, 10, 0, 0, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 14));
		room->AddEvent(new Blocker(-1, 0, 16));
		room->AddEvent(new Door(0, 2, 25, 0, -1));
		room->AddEvent(new Door(-2, 0, 23, 1, 0));
		m_rooms[26] = room;

		room = std::make_shared<Room>(27);
		room->m_floors = {21, 21, 21, 21, 19, 21, 21, 21, 21};
		room->m_walls = {19, 19, 19, 19, 21, 20};
		room->m_steps = {0, 0, 0, 0, 10, 0};
		room->m_audio = "office";
		room->m_theme = "black";
		room->AddEvent(new Blocker(-1, 1, 15));
		room->AddEvent(new Speaker(0, 1, "radio"), true);
		room->AddEvent(new Blocker(1, 1, 15));
		room->AddEvent(new Blocker(1, -1, 16));
		room->AddEvent(new Door(0, -2, 28, 0, 1));
		m_rooms[27] = room;

		room = std::make_shared<Room>(28);
		room->m_floors = {21, 21, 21, 21, 21, 21, 21, 21, 21};
		room->m_walls = {20, 24, 20, 20, 24, 21};
		room->m_steps = {0, 10, 0, 0, 10, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 14));
		room->AddEvent(new Wizard(1, 1, "document"), true);
		room->AddEvent(new Blocker(-1, -1, 16));
		room->AddEvent(new Door(0, 2, 27, 0, -1));
		room->AddEvent(new Door(2, 0, 23, -1, 0));
		room->AddEvent(new Door(-2, 0, 25, 1, 0));
		room->AddEvent(new Door(0, -2, 29, 0, 1));
		m_rooms[28] = room;

		room = std::make_shared<Room>(29);
		room->m_floors = {21, 21, 21, 21, 8, 21, 21, 21, 21};
		room->m_walls = {20, 24, 20, 20, 24, 21};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 15));
		room->AddEvent(new Blocker(1, -1, 16));
		room->AddEvent(new Door(0, 2, 28, 0, -1));
		room->AddEvent(new Gate("document", 2, 0, 30, -1, 0)); // 29 - 30
		m_rooms[29] = room;

		room = std::make_shared<Room>(30);
		room->m_floors = {21, 21, 21, 21, 1, 1, 21, 21, 1};
		room->m_walls = {20, 20, 20, 20, 21, 20};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "office";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 16));
		room->AddEvent(new Blocker(1, -1, 1));
		room->AddEvent(new Boss(1, 0));
		room->AddEvent(new Door(2, 0, 1, 0, 0));
		m_rooms[30] = room;

		room = std::make_shared<Room>(31);
		room->m_floors = {4, 5, 6, 4, 5, 4, 5, 5, 6};
		room->m_walls = {26, 26, 26, 26, 40, 15};
		room->m_steps = {0, 3, 0, 0, 0, 0};
		room->m_audio = "pillar";
		room->m_theme = "black";
		m_rooms[31] = room;

		room = std::make_shared<Room>(32);
		room->m_floors = {33, 33, 33, 14, 4, 5, 33, 6, 33};
		room->m_walls = {36, 16, 36, 19, 12, 19};
		room->m_steps = {0, 5, 0, 0, 6, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 23));
		room->AddEvent(new Owl(1, 1));
		room->AddEvent(new Plan(0, 2, "necomedre"));
		room->AddEvent(new Door(2, 0, 33, -1, 0));
		room->AddEvent(new Gate("necomedre", -2, 0, 5, 1, 0)); // 32 - 5
		room->AddEvent(new Door(0, -2, 35, 0, 1));
		m_rooms[32] = room;

		room = std::make_shared<Room>(33);
		room->m_floors = {33, 33, 33, 5, 4, 6, 33, 33, 4};
		room->m_walls = {36, 36, 26, 19, 12, 15};
		room->m_steps = {0, 7, 0, 0, 0, 6};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Wizard(0, 1, "nephtaline"), true);
		room->AddEvent(new Door(2, 0, 34, -1, 0));
		room->AddEvent(new Door(-2, 0, 32, 1, 0));
		room->AddEvent(new Door(1, -2, 36, 1, 1));
		m_rooms[33] = room;

		room = std::make_shared<Room>(34);
		room->m_floors = {33, 35, 35, 4, 6, 5, 33, 35, 35};
		room->m_walls = {36, 25, 8, 36, 12, 26};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Wizard(-1, 1, "nestorine"), true);
		room->AddEvent(new Blocker(1, -1, 11));
		room->AddEvent(new Gate("nestorine", 1, 2, 120, 0, -1)); // 34 - 120
		room->AddEvent(new Door(2, 0, 35, -1, 0));
		room->AddEvent(new Door(-2, 0, 33, 1, 0));
		m_rooms[34] = room;

		room = std::make_shared<Room>(35);
		room->m_floors = {33, 33, 33, 33, 33, 33, 33, 33, 33};
		room->m_walls = {26, 12, 26, 19, 12, 19};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		Ramen* ramen = new Ramen(1, 1, "necomedre");
		room->AddEvent(ramen->mat);
		room->AddEvent(ramen);
		room->AddEvent(new Speaker(1, -1));
		room->AddEvent(new Door(0, 2, 32, 0, -1));
		room->AddEvent(new Door(2, 0, 36, -1, 0));
		room->AddEvent(new Door(-2, 0, 34, 1, 0));
		m_rooms[35] = room;

		room = std::make_shared<Room>(36);
		room->m_floors = {33, 0, 4, 33, 0, 4, 33, 0, 4};
		room->m_walls = {26, 0, 12, 19, 12, 19};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Wizard(-1, 1, "nephtaline"), true);
		room->AddEvent(new Ghost(1, -1));
		room->AddEvent(new Door(1, 2, 33, 1, -1));
		room->AddEvent(new Door(2, 0, 37, -1, 0));
		room->AddEvent(new Door(-2, 0, 35, 1, 0));
		m_rooms[36] = room;

		room = std::make_shared<Room>(37);
		room->m_floors = {33, 4, 4, 33, 33, 4, 33, 4, 4};
		room->m_walls = {26, 26, 26, 26, 12, 26};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Wizard(0, 1, "nemedique"), true);
		room->AddEvent(new Blocker(1, -1, 23));
		room->AddEvent(new Door(2, 0, 38, -1, 0));
		room->AddEvent(new Door(-2, 0, 36, 1, 0));
		m_rooms[37] = room;

		room = std::make_shared<Room>(38);
		room->m_floors = {9, 33, 33, 4, 33, 13, 9, 33, 33};
		room->m_walls = {26, 26, 26, 18, 5, 18};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "necomedre";
		room->m_theme = "white";
		room->AddEvent(new Wizard(0, 1, "nephtaline"), true);
		room->AddEvent(new Blocker(-1, -1, 5));
		room->AddEvent(new Gate("nephtaline", 2, 0, 39, -1, 0)); // 38 - 39
		room->AddEvent(new Door(-2, 0, 37, 1, 0));
		m_rooms[38] = room;

		room = std::make_shared<Room>(39);
		room->m_floors = {33, 33, 4, 18, 7, 33, 5, 33, 33};
		room->m_walls = {26, 19, 36, 36, 19, 19};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "warp";
		room->m_theme = "white";
		room->AddEvent(new Teleport(0, 0, 1));
		room->AddEvent(new Gate("nephtaline", -2, 0, 38, 1, 0));
		m_rooms[39] = room;

		room = std::make_shared<Room>(120);
		room->m_floors = {4, 9, 5, 9, 8, 9, 5, 6, 9};
		room->m_walls = {36, 37, 36, 36, 37, 36};
		room->m_steps = {0, 0, 0, 0, 6, 0};
		room->m_audio = "pillar";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "necomedre", 5));
		room->AddEvent(new Gate("nestorine", 0, -2, 34, 1, 1));
		m_rooms[120] = room;
	}


	void CreateNephtaline()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(40);
		room->m_floors = {18, 6, 4, 5, 0, 30, 25, 0, 32};
		room->m_walls = {0, 0, 0, 12, 0, 3};
		room->m_steps = {0, 0, 0, 6, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Ghost(1, -1));
		room->AddEvent(new Door(2, 1, 41, -1, 1));
		room->AddEvent(new Gate("nephtaline", -1, -2, 1, 0, 1)); // 40 - 1
		m_rooms[40] = room;

		room = std::make_shared<Room>(41);
		room->m_floors = {4, 5, 19, 0, 0, 6, 32, 30, 5};
		room->m_walls = {0, 0, 0, 0, 0, 0};
		room->m_steps = {7, 0, 0, 0, 0, 7};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Door(-2, 1, 40, 1, 1));
		room->AddEvent(new Door(1, -2, 42, 1, 1));
		m_rooms[41] = room;

		room = std::make_shared<Room>(42);
		room->m_floors = {32, 0, 5, 30, 0, 4, 4, 6, 17};
		room->m_walls = {3, 0, 13, 0, 0, 0};
		room->m_steps = {0, 0, 7, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Wizard(-1, 1, "nemedique"), true);
		room->AddEvent(new Door(1, 2, 41, 1, -1));
		room->AddEvent(new Door(-2, -1, 43, 1, -1));
		m_rooms[42] = room;

		room = std::make_shared<Room>(43);
		room->m_floors = {4, 30, 32, 4, 0, 0, 16, 4, 4};
		room->m_walls = {13, 0, 3, 3, 0, 13};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Blocker(1, 1, 10));
		room->AddEvent(new Door(-1, 2, 44, -1, -1));
		room->AddEvent(new Door(2, -1, 42, -1, -1));
		m_rooms[43] = room;

		room = std::make_shared<Room>(44);
		room->m_floors = {18, 4, 4, 4, 0, 30, 4, 0, 32};
		room->m_walls = {0, 0, 0, 12, 0, 0};
		room->m_steps = {0, 0, 0, 7, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, -1, "neomine"));
		room->AddEvent(new Door(2, 1, 45, -1, 1));
		room->AddEvent(new Door(-1, -2, 43, -1, 1));
		m_rooms[44] = room;

		room = std::make_shared<Room>(45);
		room->m_floors = {4, 4, 17, 30, 0, 4, 32, 0, 4};
		room->m_walls = {0, 0, 0, 0, 0, 0};
		room->m_steps = {7, 0, 0, 0, 0, 7};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, -1, 7));
		room->AddEvent(new Door(-2, 1, 46, 1, 1));
		room->AddEvent(new Door(1, -2, 42, 1, 1));
		//    room->AddEvent(new Wizard(1,-2, spell: catDoorFork));
		m_rooms[45] = room;

		room = std::make_shared<Room>(46);
		room->m_floors = {3, 5, 6, 17, 4, 3, 3, 3, 3};
		room->m_walls = {18, 18, 18, 13, 18, 18};
		room->m_steps = {0, 7, 0, 0, 7, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 13));
		room->AddEvent(new Wizard(1, 0, "neomine"));
		room->AddEvent(new Broken(0, 2, 2, 149, 0, -1));
		room->AddEvent(new Door(2, 1, 41, -1, 1));
		room->AddEvent(new Door(-2, 0, 47, 1, 0));
		room->AddEvent(new Door(0, -2, 51, 0, 1));
		m_rooms[46] = room;

		room = std::make_shared<Room>(47);
		room->m_floors = {0, 0, 4, 0, 10, 5, 0, 5, 0};
		room->m_walls = {0, 0, 0, 3, 13, 3};
		room->m_steps = {0, 0, 0, 0, 7, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 1, "necomedre"), true);
		room->AddEvent(new Door(2, 0, 46, -1, 0));
		room->AddEvent(new Door(0, -2, 48, 0, 1));
		m_rooms[47] = room;

		room = std::make_shared<Room>(48);
		room->m_floors = {0, 5, 0, 0, 10, 5, 0, 0, 0};
		room->m_walls = {0, 13, 0, 0, 13, 0};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 47, 0, -1));
		room->AddEvent(new Door(2, 0, 49, -1, 0));
		m_rooms[48] = room;

		room = std::make_shared<Room>(49);
		room->m_floors = {0, 5, 0, 5, 10, 0, 0, 0, 0};
		room->m_walls = {0, 13, 0, 0, 0, 0};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 50, 0, -1));
		room->AddEvent(new Door(-2, 0, 48, 1, 0));
		m_rooms[49] = room;

		room = std::make_shared<Room>(50);
		room->m_floors = {3, 32, 3, 32, 31, 32, 3, 32, 3};
		room->m_walls = {3, 9, 3, 2, 16, 2};
		room->m_steps = {0, 7, 0, 0, 7, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Owl(1, 1));
		room->AddEvent(new Blocker(1, -1, 21));
		room->AddEvent(new Gate("nemedique", 0, 2, 121, 0, -1)); // 50 - 121
		room->AddEvent(new Plan(2, 0, "nephtaline"));
		room->AddEvent(new Door(-2, 0, 47, 1, 0));
		room->AddEvent(new Door(0, -2, 55, 0, 1));
		m_rooms[50] = room;

		room = std::make_shared<Room>(51);
		room->m_floors = {0, 5, 5, 5, 10, 5, 0, 0, 0};
		room->m_walls = {3, 13, 3, 0, 0, 0};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Speaker(1, 1), true);
		room->AddEvent(new Door(0, 2, 50, 0, -1));
		room->AddEvent(new Door(-2, 0, 48, 1, 0));
		m_rooms[51] = room;

		/*
		room = std::make_shared<Room>(52);
		room->m_floors = { 0,5,0, 0,10,5, 0,0,0 };
		room->m_walls  = { 0,13,0, 0,13,0 };
		room->m_steps  = { 0,0,0, 0,0,0 };
		room->m_audio  = "nephtaline"
		room->m_theme  = "white"
		room->AddEvent(new Door(0,2,47,0,-1));
		room->AddEvent(new Door(2,0,49,-1,0));
		m_rooms[52] = room;

		room = std::make_shared<Room>(53);
		room->m_floors = { 0,0,0, 5,10,5, 0,0,0 };
		room->m_walls  = { 0,0,0, 3,13,3 };
		room->m_steps  = { 0,0,0, 0,7,0 };
		room->m_audio  = "nephtaline"
		room->m_theme  = "white"
		room->AddEvent(new Door(2,0,41,-1,0));
		room->AddEvent(new Door(0,-2,48,0,1));
		m_rooms[53] = room;

		room = std::make_shared<Room>(54);
		room->m_floors = { 3,4,3, 5,10,3, 3,3,4 };
		room->m_walls  = { 3,8,3, 3,3,3 };
		room->m_steps  = { 0,7,0, 0,7,0 };
		room->m_audio  = "nephtaline"
		room->m_theme  = "white"
		//    room->AddEvent(new Wizard(1,1, spell: null));
		//    > gateNestorine
		//    10 > 8|gateNestorine
		room->AddEvent(new Door(-2,0,47,1,0));
		room->AddEvent(new Door(0,-2,51,0,1));
		m_rooms[54] = room;
		*/


		room = std::make_shared<Room>(97);
		room->m_floors = {35, 35, 35, 35, 7, 35, 35, 35, 35};
		room->m_walls = {25, 15, 25, 0, 0, 0};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 94, 0, -1));
		m_rooms[97] = room;

		room = std::make_shared<Room>(55);
		room->m_floors = {0, 5, 0, 5, 10, 0, 0, 0, 0};
		room->m_walls = {0, 13, 0, 0, 0, 0};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 50, 0, -1));
		room->AddEvent(new Door(-2, 0, 56, 1, 0));
		m_rooms[55] = room;

		room = std::make_shared<Room>(56);
		room->m_floors = {0, 5, 0, 0, 10, 5, 0, 0, 0};
		room->m_walls = {0, 13, 0, 0, 13, 0};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Door(0, 2, 57, 0, -1));
		room->AddEvent(new Door(2, 0, 55, -1, 0));
		m_rooms[56] = room;

		room = std::make_shared<Room>(57);
		room->m_floors = {0, 5, 5, 0, 10, 5, 0, 5, 0};
		room->m_walls = {0, 0, 0, 3, 13, 3};
		room->m_steps = {0, 0, 0, 0, 7, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		Ramen* ramen = new Ramen(1, 1, "nephtaline");
		room->AddEvent(ramen->mat);
		room->AddEvent(ramen);
		room->AddEvent(new Door(2, 0, 58, -1, 0));
		room->AddEvent(new Door(0, -2, 56, 0, 1));
		m_rooms[57] = room;

		room = std::make_shared<Room>(58);
		room->m_floors = {31, 4, 31, 4, 5, 31, 31, 31, 31};
		room->m_walls = {18, 7, 18, 18, 18, 18};
		room->AddEvent(new Blocker(1, 1, 8));
		room->AddEvent(new Blocker(1, -1, 8));
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "nephtaline";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 0, "neomine"));
		room->AddEvent(new Gate("neomine", 0, 2, 59, 0, -1)); // 58 - 59
		room->AddEvent(new Door(-2, 0, 47, 1, 0));
		m_rooms[58] = room;

		room = std::make_shared<Room>(59);
		room->m_floors = {32, 29, 32, 29, 7, 29, 32, 5, 32};
		room->m_walls = {3, 2, 3, 3, 2, 3};
		room->m_steps = {0, 0, 0, 0, 4, 0};
		room->m_audio = "warp";
		room->m_theme = "white";
		room->AddEvent(new Teleport(0, 0, 1));
		room->AddEvent(new Gate("neomine", 0, -2, 58, 0, 1)); // 59 - 58
		m_rooms[59] = room;

		room = std::make_shared<Room>(121);
		room->m_floors = {97, 31, 97, 31, 8, 31, 98, 31, 98};
		room->m_walls = {25, 18, 25, 25, 18, 25};
		room->m_steps = {0, 0, 0, 0, 3, 0};
		room->m_audio = "pillar";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "nephtaline", 1));
		room->AddEvent(new Gate("nemedique", 0, -2, 50, 0, 1)); // 121 - 50
		m_rooms[121] = room;
	}

	void CreateNeomine()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(60);
		room->m_floors = {1, 4, 18, 1, 4, 1, 1, 23, 1};
		room->m_walls = {33, 31, 33, 30, 16, 14};
		room->m_steps = {0, 0, 0, 0, 4, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(0, 1, 13));
		room->AddEvent(new Door(2, 1, 62, -1, 1));
		room->AddEvent(new Plan(2, 0, "neomine"));
		room->AddEvent(new Gate("neomine", 0, -2, 3, 0, 1)); // 60 - 3
//		door.add_destination(function()
//		{
//			return oquonie.player.id == "nastazie";
//		}, 154, -1, -1);

		room->AddEvent(new Door(2, -1, 61, -1, -1));
		m_rooms[60] = room;

		room = std::make_shared<Room>(61);
		room->m_floors = {1, 1, 1, 38, 1, 38, 1, 1, 1};
		room->m_walls = {31, 30, 31, 33, 31, 33};
		room->m_steps = {0, 0, 3, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(-1, 1, 19));
		room->AddEvent(new Wizard(0, 1, "nestorine"), true);
		room->AddEvent(new Blocker(1, 1, 19));
		room->AddEvent(new Door(-2, -1, 60, 1, -1));
		m_rooms[61] = room;

		room = std::make_shared<Room>(62);
		room->m_floors = {4, 5, 6, 5, 6, 33, 6, 5, 5};
		room->m_walls = {32, 31, 32, 36, 4, 36};
		room->m_steps = {3, 0, 3, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "necomedre", true)); // requiresPillar
		room->AddEvent(new Blocker(0, -1, 23));
		room->AddEvent(new Gate("necomedre", 2, 0, 71, -1, 0)); // 62 - 71
		room->AddEvent(new Door(-2, 1, 60, 1, 1));
		room->AddEvent(new Door(-2, -1, 63, 1, -1));
		m_rooms[62] = room;

		room = std::make_shared<Room>(63);
		room->m_floors = {10, 10, 10, 10, 0, 10, 10, 10, 9};
		room->m_walls = {32, 33, 32, 14, 30, 14};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(0, 0, 9));
		Ramen* ramen = new Ramen(-1, 1, "neomine");
		room->AddEvent(ramen->mat, true);
		room->AddEvent(ramen, true);
		room->AddEvent(new Door(2, 1, 66, -1, 1));
		room->AddEvent(new Door(2, 0, 64, -1, 0));
		room->AddEvent(new Door(2, -1, 62, -1, -1));
		m_rooms[63] = room;

		room = std::make_shared<Room>(64);
		room->m_floors = {39, 1, 39, 36, 32, 36, 39, 36, 1};
		room->m_walls = {33, 33, 33, 31, 31, 15};
		room->m_steps = {3, 3, 3, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 21));
		room->AddEvent(new Wizard(1, 0, "nephtaline", true));
		room->AddEvent(new Door(-2, 1, 68, 1, 1));
		room->AddEvent(new Door(-2, 0, 63, 1, 0));
		room->AddEvent(new Door(-2, -1, 65, 1, -1));
		m_rooms[64] = room;

		room = std::make_shared<Room>(65);
		room->m_floors = {0, 33, 1, 1, 33, 1, 1, 33, 33};
		room->m_walls = {0, 3, 3, 3, 15, 14};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Wizard(0, 1, "nestorine"), true);
		room->AddEvent(new Blocker(1, 0, 1));
		room->AddEvent(new Door(2, -1, 64, -1, -1));
		m_rooms[65] = room;

		room = std::make_shared<Room>(66);
		room->m_floors = {39, 36, 39, 36, 28, 29, 39, 36, 39};
		room->m_walls = {31, 31, 31, 33, 32, 33};
		room->m_steps = {3, 3, 3, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 13));
		room->AddEvent(new Owl(1, 0));
		//    room->AddEvent(new Door(0,2,photoBooth));
		room->AddEvent(new Door(-2, 1, 63, 1, 1));
		room->AddEvent(new Door(-2, 0, 67, 1, 0));
		room->AddEvent(new Door(-2, -1, 68, 1, -1));
		m_rooms[66] = room;

		room = std::make_shared<Room>(67);
		room->m_floors = {1, 36, 1, 1, 39, 36, 1, 1, 1};
		room->m_walls = {25, 25, 25, 31, 30, 31};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Wizard(0, 1, "nestorine"), true);
		room->AddEvent(new Door(2, 0, 66, -1, 0));
		m_rooms[67] = room;

		room = std::make_shared<Room>(68);
		room->m_floors = {38, 100, 38, 6, 9, 10, 5, 4, 10};
		room->m_walls = {20, 38, 20, 30, 30, 30};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Ghost(-1, 1), true);
		room->AddEvent(new Door(2, 1, 64, -1, 1));
		room->AddEvent(new HiversairesGate(0, 2, 153, 0, -1));
		room->AddEvent(new Door(2, 0, 69, -1, 0));
		room->AddEvent(new Door(2, -1, 66, -1, -1));
		m_rooms[68] = room;

		room = std::make_shared<Room>(69);
		room->m_floors = {35, 35, 34, 35, 35, 12, 35, 35, 34};
		room->m_walls = {35, 34, 35, 34, 8, 34};
		room->m_steps = {0, 3, 0, 0, 0, 0};
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Speaker(1, 1));
		room->AddEvent(new Blocker(-1, 1, 11));
		room->AddEvent(new Gate("nestorine", 2, 0, 70, -1, 0)); // 69 - 70
		room->AddEvent(new Door(-2, 0, 68, 1, 0));
		m_rooms[69] = room;

		room = std::make_shared<Room>(70);
		room->m_floors = {39, 40, 39, 40, 7, 40, 39, 40, 39};
		room->m_walls = {29, 29, 29, 29, 29, 29};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "warp";
		room->m_theme = "black";
		room->AddEvent(new Teleport(0, 0, 1));
		room->AddEvent(new Door(-1, 2, 70, -1, -1));
		room->AddEvent(new Door(0, 2, 70, 0, -1));
		room->AddEvent(new Door(1, 2, 70, 1, -1));
		room->AddEvent(new Door(2, 1, 70, -1, 1));
		room->AddEvent(new Door(2, 0, 70, -1, 0));
		room->AddEvent(new Door(2, -1, 70, -1, 1));
		room->AddEvent(new Gate("nestorine", -2, 0, 69, 1, 0));
		m_rooms[70] = room;

		room = std::make_shared<Room>(71);
		room->m_floors = {1, 29, 1, 29, 8, 29, 1, 29, 1};
		room->m_walls = {18, 17, 18, 18, 17, 18};
		room->m_steps = {0, 3, 0, 0, 0, 0};
		room->m_audio = "pillar";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "neomine", 3));
		room->AddEvent(new Gate("necomedre", -2, 0, 62, 1, 0)); // 71 - 62
		m_rooms[71] = room;

		// 72-79
	}

	void CreateNestorine()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(80);
		room->m_floors = {35, 35, 7, 34, 8, 35, 35, 34, 35};
		room->m_walls = {35, 15, 35, 34, 15, 34};
		room->m_steps = {0, 7, 0, 0, 7, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, 1, 23));
		room->AddEvent(new Teleport(1, 1, 7, 0, 0));
		room->AddEvent(new Door(-2, 0, 81, 1, 0));
		room->AddEvent(new Door(0, -2, 82, 0, 1));
		m_rooms[80] = room;

		room = std::make_shared<Room>(81);
		room->m_floors = {34, 35, 35, 35, 35, 35, 34, 35, 34};
		room->m_walls = {34, 35, 34, 25, 15, 25};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Door(-2, 0, 83, 1, 0));
		room->AddEvent(new Door(0, -2, 84, 0, 1));
		m_rooms[81] = room;

		room = std::make_shared<Room>(82);
		room->m_floors = {34, 35, 34, 35, 35, 34, 34, 35, 34};
		room->m_walls = {25, 15, 25, 35, 34, 35};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, -1, 11));
		room->AddEvent(new Door(-2, 0, 84, 1, 0));
		room->AddEvent(new Door(0, -2, 85, 0, 1));
		m_rooms[82] = room;

		room = std::make_shared<Room>(83);
		room->m_floors = {34, 34, 34, 35, 35, 35, 35, 35, 0};
		room->m_walls = {35, 34, 35, 26, 15, 26};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Blocker(-1, -1, 23));
		room->AddEvent(new Door(-2, 0, 86, 1, 0));
		room->AddEvent(new Door(0, -2, 87, 0, 1));
		m_rooms[83] = room;

		room = std::make_shared<Room>(84);
		room->m_floors = {34, 35, 35, 35, 35, 35, 35, 35, 34};
		room->m_walls = {35, 15, 34, 34, 15, 35};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Owl(1, 1));
		room->AddEvent(new Blocker(-1, -1, 11));
		room->AddEvent(new Door(-2, 0, 87, 1, 0));
		room->AddEvent(new Door(0, -2, 88, 0, 1));
		m_rooms[84] = room;

		room = std::make_shared<Room>(85);
		room->m_floors = {34, 35, 35, 35, 35, 35, 0, 35, 34};
		room->m_walls = {26, 15, 26, 34, 16, 34};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Ghost(1, 0));
		room->AddEvent(new Plan(2, 0, "nestorine"));
		room->AddEvent(new Door(-2, 0, 88, 1, 0));
		room->AddEvent(new Door(0, -2, 89, 0, 1));
		m_rooms[85] = room;

		room = std::make_shared<Room>(86);
		room->m_floors = {34, 10, 34, 35, 35, 35, 34, 0, 34};
		room->m_walls = {34, 34, 34, 25, 15, 25};
		room->m_steps = {0, 1, 0, 0, 0, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Door(-2, 0, 80, 1, 0));
		m_rooms[86] = room;

		room = std::make_shared<Room>(87);
		room->m_floors = {35, 35, 35, 35, 35, 35, 35, 35, 0};
		room->m_walls = {25, 15, 25, 26, 15, 0};
		room->m_steps = {0, 0, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Speaker(1, 1));
		room->AddEvent(new Door(0, -2, 91, 0, 1));
		m_rooms[87] = room;

		room = std::make_shared<Room>(88);
		room->m_floors = {34, 35, 35, 35, 35, 35, 34, 35, 0};
		room->m_walls = {36, 15, 36, 36, 15, 0};
		room->m_steps = {0, 1, 0, 0, 0, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		Ramen* ramen = new Ramen(1, 1, "nestorine");
		room->AddEvent(ramen->mat);
		room->AddEvent(ramen);
		room->AddEvent(new Door(-2, 0, 91, 1, 0));
		m_rooms[88] = room;

		room = std::make_shared<Room>(89);
		room->m_floors = {34, 35, 0, 34, 35, 34, 34, 35, 34};
		room->m_walls = {26, 15, 0, 0, 34, 34};
		room->m_steps = {0, 0, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		//    room->AddEvent(new Door(2,0,photobooth));
		room->AddEvent(new Door(0, -2, 80, 0, 1));
		m_rooms[89] = room;

		room = std::make_shared<Room>(90);
		room->m_floors = {35, 34, 35, 34, 8, 34, 35, 34, 35};
		room->m_walls = {0, 0, 0, 0, 0, 0};
		room->m_steps = {0, 0, 0, 0, 6, 0};
		room->m_audio = "pillar";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "nestorine", 7));
		room->AddEvent(new Gate("nephtaline", 0, -2, 93, 0, 1)); // 90 - 93
		m_rooms[90] = room;

		room = std::make_shared<Room>(91);
		room->m_floors = {34, 35, 35, 35, 35, 35, 34, 35, 35};
		room->m_walls = {25, 15, 25, 25, 15, 25};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 1, "nephtaline", true)); // requiresPillar:
		room->AddEvent(new Blocker(1, -1, 23));
		room->AddEvent(new Door(-2, 0, 93, 1, 0));
		room->AddEvent(new Door(0, -2, 94, 0, 1));
		m_rooms[91] = room;

		room = std::make_shared<Room>(92);
		room->m_floors = {28, 38, 28, 36, 7, 36, 28, 38, 28};
		room->m_walls = {31, 37, 31, 31, 37, 31};
		room->m_steps = {0, 1, 0, 0, 0, 0};
		room->m_audio = "warp";
		room->m_theme = "white";
		room->AddEvent(new Teleport(0, 0, 1));
		room->AddEvent(new Gate("nemedique", -2, 0, 94, 1, 0));
		m_rooms[92] = room;

		room = std::make_shared<Room>(93);
		room->m_floors = {35, 34, 35, 35, 32, 35, 35, 35, 35};
		room->m_walls = {1, 5, 1, 25, 15, 25};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 1, "nemedique"));
		room->AddEvent(new Blocker(1, -1, 11));
		room->AddEvent(new Gate("nephtaline", 0, 2, 90, 0, -1)); // 93 - 90
		room->AddEvent(new Door(-2, 0, 80, 1, 0));
		room->AddEvent(new Door(0, -2, 95, 0, 1));
		m_rooms[93] = room;

		room = std::make_shared<Room>(94);
		room->m_floors = {5, 4, 3, 10, 9, 11, 4, 10, 3};
		room->m_walls = {25, 15, 25, 3, 9, 3};
		room->m_steps = {0, 3, 0, 0, 0, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 1, "nemedique"));
		room->AddEvent(new Gate("nemedique", 2, 0, 92, -1, 0)); // 94 - 92
		room->AddEvent(new Door(-2, 0, 95, 1, 0));
		m_rooms[94] = room;

		room = std::make_shared<Room>(95);
		room->m_floors = {34, 35, 35, 35, 35, 35, 35, 35, 0};
		room->m_walls = {25, 15, 25, 26, 15, 0};
		room->m_steps = {0, 1, 0, 0, 1, 0};
		room->m_audio = "nestorine";
		room->m_theme = "white";
		room->AddEvent(new Wizard(1, 1, "nemedique"), true);
		room->AddEvent(new Blocker(-1, -1, 23));
		room->AddEvent(new Door(-2, 0, 80, 1, 0));
		room->AddEvent(new Door(0, -2, 80, 0, 1));
		m_rooms[95] = room;

		room = std::make_shared<Room>(96);
		room->m_floors = {35, 35, 35, 34, 7, 35, 35, 35, 35};
		room->m_walls = {34, 35, 34, 19, 19, 19};
		room->m_steps = {0, 1, 0, 0, 0, 0};
		room->m_audio = "warp";
		room->m_theme = "white";
		room->AddEvent(new Teleport(0, 0, 80, 0, 0));
		room->AddEvent(new Gate("nestorine", -2, 0, 7, 1, 0)); // 96 - 7
		m_rooms[96] = room;

		// 97-99
	}

	void CreateNemedique()
	{
		RoomPtr room = nullptr;

		room = std::make_shared<Room>(100);
		room->m_floors = {10, 31, 10, 10, 4, 6, 10, 5, 10};
		room->m_walls = {18, 27, 18, 18, 12, 18};
		room->m_steps = {0, 0, 0, 0, 7, 0};
		room->m_audio = "nemedique";
		room->m_theme = "white";
		room->AddEvent(new Wizard(-1, 1, "nestorine"), true);
		room->AddEvent(new PillarGate(0, 2, 104, 0, -1));
		room->AddEvent(new Door(2, 0, 101, -1, 0));
		room->AddEvent(new Gate("nemedique", 0, -2, 9, 0, 1)); // 100 - 9
		m_rooms[100] = room;

		room = std::make_shared<Room>(101);
		room->m_floors = {10, 5, 10, 6, 6, 6, 10, 10, 10};
		room->m_walls = {18, 13, 18, 18, 4, 18};
		room->m_steps = {0, 7, 0, 0, 0, 0};
		room->m_audio = "nemedique";
		room->m_theme = "white";
		Ramen* ramen = new Ramen(1, 1, "nemedique");
		room->AddEvent(ramen->mat, true);
		room->AddEvent(ramen, true);
		room->AddEvent(new Door(0, 2, 102, 0, -1));
		room->AddEvent(new Gate("necomedre", 2, 0, 103, -1, 0)); // 101 - 103
		room->AddEvent(new Door(-2, 0, 100, 1, 0));
		m_rooms[101] = room;

		room = std::make_shared<Room>(102);
		room->m_floors = {10, 5, 10, 10, 6, 10, 10, 5, 10};
		room->m_walls = {18, 18, 18, 18, 18, 18};
		room->m_steps = {0, 0, 0, 0, 7, 0};
		room->m_audio = "nemedique";
		room->m_theme = "white";
		room->AddEvent(new Shark(0, 1), true);
		room->AddEvent(new Door(0, -2, 101, 0, 1));
		room->AddEvent(new Broken(2, 0, 2, 150, -1, 0));

		m_rooms[102] = room;

		room = std::make_shared<Room>(103);
		room->m_floors = {33, 2, 33, 2, 8, 2, 33, 2, 33};
		room->m_walls = {3, 31, 3, 3, 31, 3};
		room->m_steps = {0, 6, 0, 0, 0, 0};
		room->m_audio = "pillar";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "nemedique", 9));
		room->AddEvent(new Gate("necomedre", -2, 0, 101, 1, 0)); // 103 - 101
		m_rooms[103] = room;

		room = std::make_shared<Room>(104); // End Room
		room->m_floors = {41, 42, 43, 44, 45, 46, 47, 48, 49};
		room->m_walls = {39, 33, 39, 39, 33, 39};
		room->m_steps = {0, 0, 0, 0, 11, 0};
		room->m_audio = "nepturne";
		room->m_theme = "finale";
		room->AddEvent(new Red(0, 1, 106/* 9 */), true);
		m_rooms[104] = room;

		/*
		room = std::make_shared<Room>(105);
		room->m_floors = { 3,3,3, 3,7,3, 3,3,3 };
		room->m_walls  = { 0,0,0, 0,0,0 };
		room->m_steps  = { 0,6,0, 0,0,0 };
		room->m_audio  = "warp"
		room->m_theme  = "white"
		room->AddEvent(new Teleport(0,0,0,0));
		room->AddEvent(new Door(-2,0,101,1,0));
		m_rooms[105] = room;
		*/

		room = std::make_shared<Room>(106);
		room->m_floors = {40, 40, 40, 40, 8, 40, 40, 40, 40};
		room->m_walls = {29, 29, 29, 29, 29, 29};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nepturne";
		room->m_theme = "finale";
		room->AddEvent(new Door(-1, 2, 107, -1, -1));
		room->AddEvent(new Door(0, 2, 107, 0, -1));
		room->AddEvent(new Door(1, 2, 107, 1, -1));
		room->AddEvent(new Door(2, 1, 107, -1, 1));
		room->AddEvent(new Door(2, 0, 107, -1, 0));
		room->AddEvent(new Door(2, -1, 107, -1, -1));
		m_rooms[106] = room;

		room = std::make_shared<Room>(107);
		room->m_floors = {40, 40, 40, 40, 40, 40, 40, 40, 40};
		room->m_walls = {29, 29, 29, 29, 29, 29};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nepturne";
		room->m_theme = "finale";
		room->AddEvent(new Credit(0, 0, "rekka", {"guide", "outside", "friend"}, "credit.rekka"));
		room->AddEvent(new Door(-1, 2, 108, -1, -1));
		room->AddEvent(new Door(0, 2, 108, 0, -1));
		room->AddEvent(new Door(1, 2, 108, 1, -1));
		room->AddEvent(new Door(2, 1, 108, -1, 1));
		room->AddEvent(new Door(2, 0, 108, -1, 0));
		room->AddEvent(new Door(2, -1, 108, -1, -1));
		m_rooms[107] = room;

		room = std::make_shared<Room>(108);
		room->m_floors = {40, 40, 40, 40, 40, 40, 40, 40, 40};
		room->m_walls = {29, 29, 29, 29, 29, 29};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nepturne";
		room->m_theme = "finale";
		room->AddEvent(new Credit(0, 0, "devine",{"guide", "inside", "friend"}, "credit.devine"));
		room->AddEvent(new Door(-1, 2, 11, -1, -1));
		room->AddEvent(new Door(0, 2, 11, 0, -1));
		room->AddEvent(new Door(1, 2, 11, 1, -1));
		room->AddEvent(new Door(2, 1, 11, -1, 1));
		room->AddEvent(new Door(2, 0, 11, -1, 0));
		room->AddEvent(new Door(2, -1, 11, -1, -1));
		m_rooms[108] = room;

		room = std::make_shared<Room>(109);
		room->m_floors = {4, 5, 6, 5, 2, 6, 6, 5, 4};
		room->m_walls = {13, 13, 13, 13, 13, 13};
		room->m_steps = {0, 0, 0, 0, 6, 0};
		room->m_audio = "nepturne";
		room->m_theme = "finale";
		room->AddEvent(new Wizard(0, 0, "catfishbird"), true);
		room->AddEvent(new Door(0, -2, 100, 0, 1));
		m_rooms[109] = room;

		// 110-119
	}

	void CreateNastazie()
	{
		// 122-129

		RoomPtr room = nullptr;

		room = std::make_shared<Room>(130);
		room->m_floors = {36, 39, 36, 39, 8, 39, 36, 39, 36};
		room->m_walls = {30, 99, 100, 99, 30, 99};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "nastazie"));
		room->AddEvent(new Door(-1, 2, 131, -1, -1));
		room->AddEvent(new Gate("nastazie", 2, 0, 147, -1, 0)); // 130 - 147
		m_rooms[130] = room;

		room = std::make_shared<Room>(131);
		room->m_floors = {101, 10, 101, 10, 9, 10, 101, 10, 101};
		room->m_walls = {101, 30, 101, 101, 30, 101};
		room->m_steps = {0, 0, 0, 3, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 30));
		room->AddEvent(new Door(0, 2, 142, 0, -1));
		room->AddEvent(new Door(2, 0, 133, -1, 0));
		room->AddEvent(new Door(-1, -2, 130, -1, 1));
		// room->AddEvent(new Petunia(1,-1)); // Moved to Rm 154
		m_rooms[131] = room;

		room = std::make_shared<Room>(132);
		room->m_floors = {36, 39, 9, 39, 32, 10, 36, 39, 10};
		room->m_walls = {99, 99, 30, 99, 99, 19};
		room->m_steps = {3, 0, 0, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 0, "nemedique"));
		room->AddEvent(new Door(1, 2, 143, 1, -1));
		room->AddEvent(new Door(-2, 1, 140, 1, 1));
		m_rooms[132] = room;

		room = std::make_shared<Room>(133);
		room->m_floors = {36, 39, 10, 39, 32, 9, 36, 39, 10};
		room->m_walls = {99, 29, 99, 99, 19, 101};
		room->m_steps = {0, 3, 0, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 0, "neomine"));
		room->AddEvent(new Door(0, 2, 142, 0, -1));
		room->AddEvent(new Door(-2, 0, 139, 1, 0));
		m_rooms[133] = room;

		room = std::make_shared<Room>(134);
		room->m_floors = {36, 39, 10, 39, 32, 10, 36, 39, 9};
		room->m_walls = {12, 99, 99, 99, 99, 99};
		room->m_steps = {0, 0, 3, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 0, "nephtaline"));
		room->AddEvent(new Door(-1, 2, 141, -1, -1));
		room->AddEvent(new Door(-2, -1, 138, 1, -1));
		m_rooms[134] = room;

		room = std::make_shared<Room>(135);
		room->m_floors = {36, 39, 36, 39, 31, 39, 10, 10, 9};
		room->m_walls = {99, 99, 12, 99, 99, 12};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(0, 1, "nemedique"), true);
		room->AddEvent(new Door(1, 2, 143, 1, -1));
		room->AddEvent(new Door(2, -1, 134, -1, -1));
		m_rooms[135] = room;

		room = std::make_shared<Room>(136);
		room->m_floors = {36, 39, 36, 39, 19, 39, 10, 9, 10};
		room->m_walls = {99, 29, 99, 99, 29, 99};
		room->m_steps = {0, 0, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "nastazie"), true);
		room->AddEvent(new Door(0, 2, 142, 0, -1));
		room->AddEvent(new Door(2, 0, 133, -1, 0));
		room->AddEvent(new Door(0, -2, 131, 0, 1));
		m_rooms[136] = room;

		room = std::make_shared<Room>(137);
		room->m_floors = {36, 39, 36, 39, 31, 39, 9, 10, 10};
		room->m_walls = {30, 99, 99, 30, 99, 99};
		room->m_steps = {0, 0, 0, 0, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Ghost(1, -1));
		room->AddEvent(new Door(-1, 2, 141, -1, -1));
		room->AddEvent(new Door(2, 1, 132, -1, 1));
		m_rooms[137] = room;

		room = std::make_shared<Room>(138);
		room->m_floors = {10, 39, 36, 10, 8, 39, 9, 39, 36};
		room->m_walls = {25, 14, 25, 99, 99, 29};
		room->m_steps = {0, 0, 0, 8, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "nemedique"), true);
		room->AddEvent(new Door(0, 2, 142, 0, -1));
		room->AddEvent(new Door(2, -1, 134, -1, -1));
		room->AddEvent(new Door(-1, -2, 137, -1, 1));
		m_rooms[138] = room;

		room = std::make_shared<Room>(139);
		room->m_floors = {10, 39, 36, 9, 17, 39, 10, 39, 36};
		room->m_walls = {30, 99, 100, 99, 29, 99};
		room->m_steps = {0, 8, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "neomine"), true);
		room->AddEvent(new Door(-1, 2, 141, -1, -1));
		room->AddEvent(new Door(2, 0, 133, -1, 0));
		room->AddEvent(new Door(-2, 0, 131, 1, 0));
		room->AddEvent(new Door(0, -2, 136, 0, 1));
		m_rooms[139] = room;

		room = std::make_shared<Room>(140);
		room->m_floors = {9, 39, 36, 10, 8, 39, 10, 39, 36};
		room->m_walls = {12, 99, 99, 12, 99, 99};
		room->m_steps = {0, 0, 0, 0, 0, 8};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 0, "nephtaline"));
		room->AddEvent(new Door(-1, 2, 141, -1, -1));
		room->AddEvent(new Door(2, 1, 132, -1, 1));
		room->AddEvent(new Door(1, -2, 135, 1, 1));
		m_rooms[140] = room;

		room = std::make_shared<Room>(141);
		room->m_floors = {9, 10, 10, 39, 33, 39, 36, 39, 36};
		room->m_walls = {99, 5, 99, 99, 99, 99};
		room->m_steps = {8, 0, 0, 8, 0, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 30));
		room->AddEvent(new Gate("nephtaline", 0, 2, 144, 0, -1)); // 141 - 144
		room->AddEvent(new Door(-2, 1, 140, 1, 1));
		room->AddEvent(new Door(-1, -2, 137, -1, 1));
		m_rooms[141] = room;

		room = std::make_shared<Room>(142);
		room->m_floors = {10, 9, 10, 39, 33, 39, 36, 39, 36};
		room->m_walls = {19, 5, 99, 99, 99, 99};
		room->m_steps = {0, 8, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 30));
		room->AddEvent(new Gate("neomine", 0, 2, 145, 0, -1)); // 142 - 145
		room->AddEvent(new Broken(2, 0, 4, 151, -1, 0));
		room->AddEvent(new Door(-2, 0, 139, 1, 0));
		room->AddEvent(new Door(0, -2, 136, 0, 1));
		m_rooms[142] = room;

		room = std::make_shared<Room>(143);
		room->m_floors = {10, 10, 9, 39, 33, 39, 36, 39, 36};
		room->m_walls = {99, 5, 99, 99, 99, 99};
		room->m_steps = {0, 0, 3, 0, 0, 8};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Blocker(1, 1, 30));
		room->AddEvent(new Gate("nemedique", 0, 2, 146, 0, -1)); // 143 - 146
		room->AddEvent(new Door(-2, -1, 138, 1, -1));
		room->AddEvent(new Door(1, -2, 135, 1, 1));
		m_rooms[143] = room;

		room = std::make_shared<Room>(144);
		room->m_floors = {36, 39, 3, 39, 28, 39, 36, 39, 36};
		room->m_walls = {99, 99, 99, 99, 99, 99};
		room->m_steps = {0, 0, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "nastazie"), true);
		room->AddEvent(new Door(0, -2, 141, 0, 1));
		m_rooms[144] = room;

		room = std::make_shared<Room>(145);
		room->m_floors = {36, 39, 36, 39, 28, 39, 36, 39, 36};
		room->m_walls = {99, 99, 99, 99, 99, 99};
		room->m_steps = {0, 0, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "nephtaline"), true);
		room->AddEvent(new Door(0, -2, 142, 0, 1));
		m_rooms[145] = room;

		room = std::make_shared<Room>(146);
		room->m_floors = {36, 39, 36, 39, 28, 39, 36, 39, 36};
		room->m_walls = {99, 99, 99, 99, 99, 99};
		room->m_steps = {0, 0, 0, 0, 8, 0};
		room->m_audio = "nastazie";
		room->m_theme = "black";
		room->AddEvent(new Wizard(1, 1, "neomine"), true);
		room->AddEvent(new Door(0, -2, 143, 0, 1));
		m_rooms[146] = room;

		room = std::make_shared<Room>(147);
		room->m_floors = {101, 39, 101, 1, 1, 1, 101, 39, 101};
		room->m_walls = {99, 99, 99, 99, 100, 99};
		room->m_steps = {0, 8, 0, 0, 0, 0};
		room->m_audio = "glitch";
		room->m_theme = "pillars";
		room->AddEvent(new Pillar(0, 0, "nastazie", 4));
		room->AddEvent(new Gate("nastazie", -2, 0, 130, 1, 0));
		m_rooms[147] = room;
	}

	void CreateSecrets()
	{
		RoomPtr room = nullptr;

		// Cat Wizard 1
		room = std::make_shared<Room>(148);
		room->m_floors = {0, 0, 0, 1, 1, 1, 0, 0, 0};
		room->m_walls = {0, 0, 0, 0, 3, 0};
		room->m_steps = {0, 8, 0, 0, 0, 0};
		room->m_audio = "glitch";
		room->m_theme = "black";
		room->AddEvent(new Door(-2, 0, 3, 1, 0));
		room->AddEvent(new Wizard(1, 0, "catfishbird"));
		m_rooms[148] = room;

		// Cat Wizard 2
		room = std::make_shared<Room>(149);
		room->m_floors = {0, 10, 0, 0, 10, 0, 0, 10, 0};
		room->m_walls = {0, 16, 0, 0, 0, 0};
		room->m_steps = {0, 0, 0, 0, 8, 0};
		room->m_audio = "glitch";
		room->m_theme = "black";
		room->AddEvent(new Door(0, -2, 46, 0, 1));
		room->AddEvent(new Wizard(0, 1, "catfishbird"), true);
		m_rooms[149] = room;

		// Cat Wizard 3
		room = std::make_shared<Room>(150);
		room->m_floors = {0, 2, 0, 2, 2, 2, 0, 0, 0};
		room->m_walls = {0, 25, 0, 0, 0, 0};
		room->m_steps = {0, 8, 0, 0, 0, 0};
		room->m_audio = "glitch";
		room->m_theme = "black";
		room->AddEvent(new Door(-2, 0, 102, 1, 0));
		room->AddEvent(new Gate("catfishbird", 2, 0, 152, -1, 0)); // 150 - 152
		room->AddEvent(new Wizard(0, 1, "catfishbird"), true);
		m_rooms[150] = room;

		// Rekka & Devine
		room = std::make_shared<Room>(151);
		room->m_floors = {101, 39, 101, 1, 1, 1, 101, 39, 101};
		room->m_walls = {99, 99, 99, 99, 100, 99};
		room->m_steps = {0, 8, 0, 0, 0, 0};
		room->m_audio = "glitch";
		room->m_theme = "black";
		room->AddEvent(new Door(-2, 0, 142, 1, 0));
		room->AddEvent(new Credit(1, 0, "rekka", {"confusion1", "confusion3", "confusion2"}, true));
		room->AddEvent(new Credit(0, 1, "devine", {"confusion1", "confusion3", "confusion2"}));
		m_rooms[151] = room;

		// Cat Teleport
		room = std::make_shared<Room>(152);
		room->m_floors = {2, 2, 7, 2, 0, 0, 0, 0, 5 }; // 8
		room->m_walls = { 0, 0, 0, 0, 0, 13 };
		room->m_steps = { 0, 8, 0, 0, 0, 7 };
		room->m_audio = "glitch";
		room->m_theme = "black";
		room->AddEvent(new Teleport(1, 1, 9));
		room->AddEvent(new Gate("catfishbird", -2, 0, 150, 1, 0)); // 152 - 150
		m_rooms[152] = room;

		// Hiversaires
		room = std::make_shared<Room>(153);
		room->m_floors = { 109, 107, 105, 111, 0, 106, 112, 110, 108 };
		room->m_walls = { 102, 103, 102, 0, 0, 0 };
		room->m_steps = { 0, 0, 0, 0, 0, 0 };
		room->m_audio = "glitch";
		room->m_theme = "hiversaires";
		room->AddEvent(new Blocker(1, 0, 29));
		room->AddEvent(new Door(0, -2, 68, 0, 1));
		room->AddEvent(new Blocker(1, -1, 0));
		room->AddEvent(new Blocker(-1, -1, 0));
		room->AddEvent(new HiversairesSauvegarde(0, 0));
		m_rooms[153] = room;

		// Neomine Pig-Wizard Shuffle
		room = std::make_shared<Room>(154);
		room->m_floors = { 1, 1, 1, 38, 1, 38, 1, 1, 1 };
		room->m_walls = { 31, 30, 31, 33, 31, 33 };
		room->m_steps = { 0, 0, 3, 0, 0, 0 };
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Blocker(-1, 1, 19));
		room->AddEvent(new Blocker(1, 1, 19));
		room->AddEvent(new Wizard(1, 0, "nestorine"));
		room->AddEvent(new Door(0, 2, 155, 0, -1));
		room->AddEvent(new Door(-2, -1, 60, 1, -1));
		m_rooms[154] = room;

		// Petunia Lobby
		room = std::make_shared<Room>(155);
		room->m_floors = { 36, 36, 36, 36, 36, 36, 36, 36, 36 };
		room->m_walls = { 31, 31, 31, 31, 31, 31 };
		room->m_steps = { 0, 0, 0, 0, 3, 0 };
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Petunia(0, 1));
		room->AddEvent(new Door(0, -2, 154, 0, 1));
		room->AddEvent(new Door(2, 0, 156, -1, 0));
		m_rooms[155] = room;

		// TODO: Daniel"s room
		room = std::make_shared<Room>(156);
		room->m_floors = { 31, 29, 31, 29, 20, 29, 31, 29, 31 };
		room->m_walls = { 34, 37, 34, 35, 37, 35 };
		room->m_steps = { 0, 7, 0, 0, 0, 0 };
		room->m_audio = "neomine";
		room->m_theme = "black";
		room->AddEvent(new Credit(1, 0, "daniel", {"confusion1", "confusion3", "confusion2"}));
		room->AddEvent(new Door(-2, 0, 155, 1, 0));
		m_rooms[156] = room;
	}

	std::map<int, RoomPtr> m_rooms;
private:
};
