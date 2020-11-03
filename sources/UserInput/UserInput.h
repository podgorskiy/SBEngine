#pragma once

#include "EventDispatcher/EventDispatcher.h"
#include "InputEventReciever.h"
#include <glm/glm.hpp>

class UserInput : public InputEventReciever
{
public:
	void LoadProfile();

	void OnKey(int key, char asci, int action, int mods) override;

	void OnSetChar(unsigned int c) override;

	void OnMouseButton(int button, int action, int mods) override;

	void OnMouseMove(glm::vec2 pos) override;

	void OnScroll(glm::vec2 offset) override;

	void InjectEventDispatcher(core::EventDispatcher* eventDispatcher)
	{ m_eventDispatcher = eventDispatcher; }

private:
	core::EventDispatcher* m_eventDispatcher;
	glm::vec2 m_cursor;
};
