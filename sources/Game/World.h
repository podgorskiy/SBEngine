#pragma once
#include "Room.h"
#include "Entities.h"
#include <map>


class World;
typedef std::shared_ptr<World> WorldPtr;


class World
{
public:
	void Install();

	void CreateLobby();

	void CreateNecomedre();

	void CreateNephtaline();

	void CreateNeomine();

	void CreateNestorine();

	void CreateNemedique();

	void CreateNastazie();

	void CreateSecrets();

	std::map<int, RoomPtr> m_rooms;
private:
};
