#include "Actors.h"
#include "Audio/Audio.h"
#include "Block.h"
#include "EventDispatcher/EventDispatcher.h"
#include "EventIds.h"


namespace UI
{
	using namespace UI::lit;

	void ButtonActor::AfterCST(Block* block, Block* parent, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click)
	{
		glm::aabb2 box = block->GetBox();
		bool inside = glm::is_inside(box, mouse_pos);

		bool transition_to_hower = inside && (!m_hovered_state || m_down_state) && !mouse_down;
		bool transition_to_down = inside && !m_down_state && mouse_down;
		bool transition_to_idle = !inside && (m_hovered_state || m_down_state);

		if (inside && mouse_left_click)
		{
			Audio::AudioContext().PlayFile("audio/ui/381995-Complex_Button_3.mp3");
			core::GetEventDispatcher().Dispatch(Events::UI_UserAction, m_action_str);
		}

		if (transition_to_down)
		{
			Audio::AudioContext().PlayFile("audio/ui/navigate_8.mp3");
			for (auto c: c_down) block->PushTargetTransitionConstraints(c);
			m_down_state = true;
		}
		if (transition_to_hower)
		{
			Audio::AudioContext().PlayFile("audio/ui/App Menu Button 6.mp3");
			for (auto c: c_hower) block->PushTargetTransitionConstraints(c);
			m_down_state = false;
			m_hovered_state = true;
		}
		if (transition_to_idle)
		{
			for (auto c: c_idle) block->PushTargetTransitionConstraints(c);
			m_hovered_state = false;
			m_down_state = false;
		}
	}
}
