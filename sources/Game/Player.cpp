#include "Player.h"
#include "Oquonie.h"
#include "Stage.h"
#include <glm/glm.hpp>


Player::Player(): Event("player"), m_orientation("front")
{
	m_animator->Add(Animation("idle.front", {1, 1, 1, 1, 1, 2, 3, 2}));
	m_animator->Add(Animation("idle.back", {1}));
	m_animator->Add(Animation("walk.front", {1, 2}));
	m_animator->Add(Animation("walk.back", {1, 2}));
	m_animator->Add(Animation("warp", {1}));
}

void Player::SetId(const std::string& new_id)
{
	if (m_id != new_id)
	{
		m_id = new_id;
		// TODO: m_animator.preload();
	}
}

void Player::TryMove(int x, int y)
{
//	if (Oquonie::GetInstance()->m_dialog->content)
//	{
//		Oquonie::GetInstance()->m_dialog->Hide();
//		return;
//	}
//	if (Oquonie::GetInstance()->m_overlay->content)
//	{
//		Oquonie::GetInstance()->m_overlay->Hide()
//		return;
//	}

	glm::ivec2 destination(m_x + x, m_y + y);

	auto target_tiles = Oquonie::GetInstance()->m_stage->TilesAt(destination.x, destination.y);
	auto target_floor = Oquonie::GetInstance()->m_stage->FloorAt(destination.x, destination.y);

	bool elicits_collision_bump = target_tiles.empty();

	std::vector<EventPtr> colliders;

	for (auto tile: target_tiles)
	{
		elicits_collision_bump = elicits_collision_bump || tile->ElicitsCollisionBump();
		if (tile->IsCollider())
		{
			colliders.push_back(tile);
		}
	}

	if (elicits_collision_bump)
	{
		if (x == 0 && y == -1)
		{
			//$(this.element).attr('orientation', 'front').attr('direction', 'right');
		}
		if (x == -1 && y == 0)
		{
			//$(this.element).attr('orientation', 'front').attr('direction', 'left');
		}
		if (x == 0 && y == 1)
		{
			//$(this.element).attr('orientation', 'back').attr('direction', 'left');
		}
		if (x == 1 && y == 0)
		{
			//$(this.element).attr('orientation', 'back').attr('direction', 'right');
		}
	}

	bool mid_walk = m_animator->m_state.find("walk") != std::string::npos;

	if (colliders.size() > 0)
	{
		if (elicits_collision_bump)
		{
			// BumpAgainst(x, y)

			for (auto collider: colliders)
			{
				printf("Blocked by: %s\n", collider->GetHostHame().c_str());

				if (collider->ElicitsCollisionBump())
				{
					// collider->ReceiveBump();
				}
				collider->OnCollision();
			}
		}
	}
	else if (destination[0] > 1 || destination[0] < -1 || destination[1] > 1 || destination[1] < -1)
	{
		printf("Blocked by: Edge\n");
		if (!mid_walk)
		{
			// BumpUp(x, y)
			// oquonie.music.play_effect('bump.2')
		}
	}
	else if (target_floor == 0)
	{
		printf("Blocked by: Floor(%d)\n", target_floor);
		if (!mid_walk)
		{
			// BumpUp(x, y)
			// oquonie.music.play_effect('bump.3')
		}
	}
	else
	{
		MoveBy(x, y);
		printf("Moved to: Floor(%d, %d)\n", m_x, m_y);
		// oquonie.music.play_effect('walk')
	}

	for (auto tile: target_tiles)
	{
		tile->OnStep();
	}

	Update(20);
}
