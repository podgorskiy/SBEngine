#include "UserInput.h"
#include "EventIds.h"
#include <fsal.h>
#include <yaml-cpp/yaml.h>


void UserInput::LoadProfile()
{
    using fs = fsal::FileSystem;
	using loc = fsal::Location;

	YAML::Node config = YAML::Load(std::string(fs().Open("input.yaml")));

}

void UserInput::OnKey(int key, char asci, int action, int mods)
{

}

void UserInput::OnSetChar(unsigned int c)
{

}

void UserInput::OnMouseButton(int button, int action, int mods)
{
	if (button == 0)
	{
		if (action == 1)
		{
			m_eventDispatcher->Dispatch(Events::UserInput,
			                            Events::UserInputEvent({Events::UserInputEvent::CursorDown, m_cursor}));
		}
		else if (action == 0)
		{
			m_eventDispatcher->Dispatch(Events::UserInput,
			                            Events::UserInputEvent({Events::UserInputEvent::CursorRelease, m_cursor}));
		}
	}
}


void UserInput::OnMouseMove(glm::vec2 pos)
{
	m_cursor = pos;
		m_eventDispatcher->Dispatch(Events::UserInput, Events::UserInputEvent({Events::UserInputEvent::CursorMove, m_cursor}));
}

void UserInput::OnScroll(glm::vec2 offset)
{

}
