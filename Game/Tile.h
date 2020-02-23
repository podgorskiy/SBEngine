#pragma once
#include <string>
#include <memory>
#include <stdlib.h>


class Tile;
typedef std::shared_ptr<Tile> TilePtr;


class Tile
{
public:
	Tile(): m_class_name("unknown"), m_x(0), m_y(0), m_id(-1)
	{
		// TODO create element 'tile'
	}

	explicit Tile(std::string type): m_class_name(std::move(type)), m_x(0), m_y(0), m_id(-1)
	{
	}

	void Update(int depth_offset = 0)
	{
		int depth = Depth(depth_offset);
	    auto p = PositionAt(m_x, m_y);
		// TODO element.set_position(p);
		// TODO element.set_depth(depth);
	}

	void Animate()
	{
		int origin = PositionAt(m_x, m_y).first;
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float offset = (origin * (1 + (r / 20.0f)));
		// TODO element.animate() opacity 0 -> 1, top offset -> origin, speed
    }

	int m_x;
	int m_y;
protected:
	static std::pair<int, int> PositionAt(int x, int y)
	{
	    int top = 0;
        int left = 0;
	
	    if (x == 1 && y == 1) { top = 40; left = 50; }
	    if (x == 1 && y == 0) { top = 45; left = 60; }
	    if (x == 1 && y == -1) { top = 50; left = 70; }
	
	    if (x == 0 && y == 1) { top = 45; left = 40; }
	    if (x == 0 && y == 0) { top = 50; left = 50; }
	    if (x == 0 && y == -1) { top = 55; left = 60; }
	
	    if (x == -1 && y == 1) { top = 50; left = 30; }
	    if (x == -1 && y == 0) { top = 55; left = 40; }
	    if (x == -1 && y == -1) { top = 60; left = 50; }
	
	    // Wall
	    if (x == -1 && y == 2) { top = 48; left = 20; }
	    if (x == 0 && y == 2) { top = 43; left = 30; }
	    if (x == 1 && y == 2) { top = 38; left = 40; }
	    if (x == 2 && y == 1) { top = 38; left = 60; }
	    if (x == 2 && y == 0) { top = 43; left = 70; }
	    if (x == 2 && y == -1) { top = 48; left = 80; }
	
	    // Step
	    if (x == -2 && y == 1) { top = 55; left = 20; }
	    if (x == -2 && y == 0) { top = 60; left = 30; }
	    if (x == -2 && y == -1) { top = 65; left = 40; }
	    if (x == -1 && y == -2) { top = 65; left = 60; }
	    if (x == 0 && y == -2) { top = 60; left = 70; }
	    if (x == 1 && y == -2) { top = 55; left = 80; }

        return std::make_pair(top, left);
	}

	int Depth(int offset = 0)
	{
        int zIndex = offset;

	    if (m_x == 1 && m_y == 1) { zIndex += 1; }
	    if (m_x == 1 && m_y == 0) { zIndex += 2; }
	    if (m_x == 1 && m_y == -1) { zIndex += 3; }
	    if (m_x == 0 && m_y == 1) { zIndex += 2; }
	    if (m_x == 0 && m_y == 0) { zIndex += 3; }
	    if (m_x == 0 && m_y == -1) { zIndex += 4; }
	    if (m_x == -1 && m_y == 1) { zIndex += 3; }
	    if (m_x == -1 && m_y == 0) { zIndex += 4; }
	    if (m_x == -1 && m_y == -1) { zIndex += 5; }
	    // Wall
	    if (m_x == -1 && m_y == 2) { zIndex += 3; }
	    if (m_x == 0 && m_y == 2) { zIndex += 2; }
	    if (m_x == 1 && m_y == 2) { zIndex += 1; }
	    if (m_x == 2 && m_y == 1) { zIndex += 1; }
	    if (m_x == 2 && m_y == 0) { zIndex += 2; }
	    if (m_x == 2 && m_y == -1) { zIndex += 3; }
	    // Step
	    if (m_x == -2 && m_y == 1) { zIndex += 1; }
	    if (m_x == -2 && m_y == 0) { zIndex += 2; }
	    if (m_x == -2 && m_y == -1) { zIndex += 3; }
	    if (m_x == -1 && m_y == -2) { zIndex += 3; }
	    if (m_x == 0 && m_y == -2) { zIndex += 2; }
	    if (m_x == 1 && m_y == -2) { zIndex += 1; }
	    return zIndex;
	}

	int m_id;
	std::string m_class_name;
};
