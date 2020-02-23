#pragma once
#include "Tile.h"

class Floor: public Tile
{
public:
	Floor(int pos, int id): Tile("floor")
	{
		std::pair<int, int> t[] = {{-1, 1}, {0, 1}, {1, 1}, {-1, 0}, {0, 0}, {1, 0}, {-1, -1}, {0, -1}, {1, -1}};

		m_x = t[pos].first;
		m_y = t[pos].second;
		m_id = id;
        // TODO this.element.setAttribute('pos', this.x + ',' + this.y)
        Update(5);
        if (id != 0)
        {
        	// TODO oquonie.artbook.set_art(this.element, 'media/graphics/floor/' + this.id + '.png');
        }
	}
};
