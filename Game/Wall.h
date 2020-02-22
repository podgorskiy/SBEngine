#pragma once
#include "Tile.h"

class Step: public Tile
{
public:
	Step(int pos, int id): Tile(std::string("step ") + (pos < 3 ? "left" : "right"))
	{
		std::pair<int, int> t[] = {
				{-1, 2},
				{0,  2},
				{1,  2},
				{2,  1},
				{2,  0},
				{2,  -1}
		};
		m_x = t[pos].first;
		m_y = t[pos].second;
		m_id = id;
        // TODO this.element.setAttribute('pos', this.x + ',' + this.y)
        Update();
        if (id != 0)
        {
        	// TODO oquonie.artbook.set_art(this.element, 'media/graphics/wall/' + this.id + '.png');
        }
	}
};
