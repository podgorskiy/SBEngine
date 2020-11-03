#pragma once
#include "Enums.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>


namespace UI
{
	class Block;

	class IActor
	{
	public:
		virtual void BeforeCST(Block* block, Block* parent, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click) {};
		virtual void AfterCST(Block* block, Block* parent, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click) {};

		virtual ~IActor() = default;
	};

	class ButtonActor: public IActor
	{
	public:
		ButtonActor(std::string action_str, std::vector<Constraint> c_idle, std::vector<Constraint> c_hower, std::vector<Constraint> c_down):
			m_action_str(std::move(action_str)), c_idle(std::move(c_idle)), c_hower(std::move(c_hower)), c_down(std::move(c_down)) {}

		//void BeforeCST(Block* block, Block* parent, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click) override;
		void AfterCST(Block* block, Block* parent, float time, glm::vec2 mouse_pos, bool mouse_down, bool mouse_left_click) override;
	private:
		bool m_hovered_state = false;
		bool m_down_state = false;
		std::string m_action_str;
		std::vector<Constraint> c_idle;
		std::vector<Constraint> c_hower;
		std::vector<Constraint> c_down;
	};
}
