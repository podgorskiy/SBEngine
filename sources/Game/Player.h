#pragma once
#include "Event.h"
#include "Animator.h"


class Player;
typedef std::shared_ptr<Player> PlayerPtr;


class Player : public Event
{
public:
	Player();

	void SetId(const std::string& new_id);

	void TryMove(int x, int y);

	std::string m_id;
	std::string m_orientation;
};
