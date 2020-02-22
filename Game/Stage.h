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

	RoomPtr m_room;
private:
};
