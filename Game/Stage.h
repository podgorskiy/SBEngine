#pragma once
#include "Room.h"
#include <vector>
#include <memory>


class Stage;
typedef std::shared_ptr<Stage> StagePtr;


class Stage
{
public:
	Stage();

	void Install();


	void EnterRoom(int room_id, int x = 0, int y = 0);

	std::vector<EventPtr> TilesAt(int x, int y);

	int FloorAt(int x, int y);

	int WallAt(int x, int y);

	void Look();

	void Animate(int x, int y);

	void Center(int x, int y);

	RoomPtr m_room;
private:
};
