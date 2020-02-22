#include "Stage.h"
#include "Oquonie.h"


Stage::Stage(): m_room(nullptr)
{
	//this.element = document.createElement('stage')
	//this.parallax_over = document.createElement('parallax')
	//this.parallax_under = document.createElement('parallax')
}

void Stage::Install()
{
//	    oquonie.element.appendChild(this.element)
//	    this.element.appendChild(this.parallax_over)
//	    this.element.appendChild(this.parallax_under)
//	    this.parallax_over.setAttribute('class', 'over')
//	    this.parallax_under.setAttribute('class', 'under')
}


void Stage::EnterRoom(int room_id, int x, int y)
{
	printf("Entering Room: %d\n", room_id);

	if (Oquonie::GetInstance()->m_world->m_rooms.find(room_id) == Oquonie::GetInstance()->m_world->m_rooms.end())
	{
		printf("Missing room:(%d)\n", room_id);
		return;
	}

}
