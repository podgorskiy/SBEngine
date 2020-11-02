#include "BaseMenuState.h"
#include "MenuStateManager.h"
#include "MenuFactory.h"
#include "UI/UI_Load.h"
#include "UI/UI.h"
#include "EventIds.h"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <fsal.h>


BaseMenuState::BaseMenuState(const fsal::File& yaml, MenuStateManager* msm): m_msm(msm), m_eventDispatcher(msm->GetEventDispatcher())
{
	auto res = msm->Load(yaml);
	m_root = res.first;
	m_animation = res.second;
	YAML::Node root_node = YAML::Load(std::string(yaml));
	auto name = root_node["menu_name"];
	m_name = name.as<std::string>("noname");
	m_time = 0.0f;
}

void BaseMenuState::OnPush(float time)
{
	auto it = m_animation.find("on_push");
	if (it != m_animation.end())
	{
		for (auto& b:it->second)
		{
			for (auto& c: b.second)
			{
				b.first->PushTargetTransitionConstraints(c);
			}
		}
	}

	m_msm->GetEventDispatcher()->AttachEventHandler<BaseMenuState>(Events::UserInput, &BaseMenuState::OnUserInput, this);
}

bool BaseMenuState::OnUserInput(uint64_t eventID, core::EventDispatcher::Box box)
{
	auto type = box.Unbox<Events::UserInputEvent>().type;
	m_cursor_pos = box.Unbox<Events::UserInputEvent>().position;
	if (type == Events::UserInputEvent::CursorMove)
	{
	}
	else if (type == Events::UserInputEvent::CursorDown)
	{
		m_cursor_down = true;
	}
	else if (type == Events::UserInputEvent::CursorRelease)
	{
		m_cursor_action = true;
		m_cursor_down = false;
	}
}

void BaseMenuState::OnPop(float time)
{
	m_msm->GetEventDispatcher()->DetachEventHandler(this);
}

void BaseMenuState::Update(Render::View viewbox, float time, float deltaTime)
{
	m_time += deltaTime;
	UI::DoLayout(m_root, viewbox, time);
	UI::Action(m_root, viewbox, time, m_cursor_pos, m_cursor_down, m_cursor_action);


	m_cursor_action = false;
}

void BaseMenuState::Draw(Render::View viewbox, Render::Renderer2D* rd)
{
	UI::Render(rd, m_root, viewbox, m_time);
}

std::string BaseMenuState::GetName()
{
	return m_name;
}


RegisterMenu(BaseMenuState);
