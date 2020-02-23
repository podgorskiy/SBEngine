#pragma once
#include <vector>
#include <memory>
#include <Tile.h>
#include <Floor.h>
#include <Wall.h>
#include <Step.h>
#include <Event.h>


class Room;
typedef std::shared_ptr<Room> RoomPtr;


class Room
{
public:
	Room(int id): m_id(id)
	{

	}

	void AddEvent(Event* event, bool is_mirrored = false)
	{
		if (is_mirrored)
		{
			event->Mirror();
		}
		event->m_location = m_id;

		m_events.push_back(EventPtr(event));
	}


	void Refresh()
	{
		// Clear

	    // Floor
	    for (int i = 0; i < m_floors.size(); i++)
	    {
	      auto tile = new Floor(i, m_floors[i]);
	      //tile.element.setAttribute("id", "floor_" + i);
	      //m_element.appendChild(tile.element);
	      // tile->Animate();
	    }

	    // Wall
	    for (int i = 0; i < m_walls.size(); i++)
	    {
	      auto tile = new Wall(i, m_walls[i]);
	      //tile.element.setAttribute("id", "wall_" + i);
	      //m_element.appendChild(tile.element);
	      // tile->Animate();
	    }

	    // Steps
	    for (int i = 0; i < m_steps.size(); i++)
	    {
	      auto tile = new Step(i, m_steps[i]);
	      //m_element.appendChild(tile.element);
	      // tile->Animate();
	    }

	    // Events
	    for (int i = 0; i < m_events.size(); i++)
	    {
	      auto event = m_events[i];
	      event->m_animator->Animate();
	      // m_element.appendChild(event.element);
	      // tile->Animate();
        }

        // TODO m_element.appendChild(oquonie.player.element);
	}

	void Show()
	{
		// Floor
		for (int i = 0; i < m_floors.size(); i++)
		{
			auto tile = new Floor(i, m_floors[i]);
			//tile.element.setAttribute("id", "floor_" + i)
			//m_element.appendChild(tile.element)
			tile->Animate();
		}

		// Wall
		for (int i = 0; i < m_walls.size(); i++)
		{
			auto tile = new Wall(i, m_walls[i]);
			//tile.element.setAttribute("id", "wall_" + i)
			//m_element.appendChild(tile.element)
			tile->Animate();
		}

		// Steps
		for (int i = 0; i < m_steps.size(); i++)
		{
			auto tile = new Step(i, m_steps[i]);
			// m_element.appendChild(tile.element)
			tile->Animate();
		}

		// Events
		for (int i = 0; i < m_events.size(); i++)
		{
			auto event = m_events[i];
			event->m_animator->Animate();
			// m_element.appendChild(event.element)
			event->Animate();
		}

		// m_element.appendChild(oquonie.player.element)
	}

	std::vector<int> m_floors;
	std::vector<int> m_walls;
	std::vector<int> m_steps;
	std::string m_audio;
	std::string m_theme;
	bool m_is_known;
	std::vector<EventPtr> m_events;
private:
	int m_id;
};
