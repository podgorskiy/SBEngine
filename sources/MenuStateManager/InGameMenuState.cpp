#include "InGameMenuState.h"
#include "MenuStateManager.h"
#include "MenuFactory.h"
#include "UI/UI_Load.h"
#include "UI/UI.h"
#include "EventIds.h"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <fsal.h>
#include <Tile.h>
#include <Wall.h>
#include <Oquonie.h>
#include <World.h>
#include <Keyboard.h>
#include <Music.h>

Keyboard keyboard;


InGameMenuState::InGameMenuState(const fsal::File& yaml, MenuStateManager* msm): m_msm(msm), m_eventDispatcher(msm->GetEventDispatcher())
{
	auto res = msm->Load(yaml);
	m_root = res.first;
	m_animation = res.second;
	YAML::Node root_node = YAML::Load(std::string(yaml));
	auto name = root_node["menu_name"];
	m_name = name.as<std::string>("noname");
	m_time = 0.0f;
	m_frame = 0;
}

void InGameMenuState::OnPush(float time)
{
	m_msm->GetEventDispatcher()->AttachEventHandler<InGameMenuState>(Events::UserInput, &InGameMenuState::OnUserInput, this);
	m_msm->GetEventDispatcher()->AttachEventHandler<InGameMenuState>(Events::KeyDown, &InGameMenuState::OnUserInput, this);
	m_msm->GetEventDispatcher()->AttachEventHandler<InGameMenuState>(Events::KeyUp, &InGameMenuState::OnUserInput, this);
	Oquonie::GetInstance()->m_root = m_root;
	Oquonie::GetInstance()->Install();
	Oquonie::GetInstance()->Start();
	m_timer = 0.0f;
}

bool InGameMenuState::OnUserInput(uint64_t eventID, core::EventDispatcher::Box box)
{
	if (eventID == Events::UserInput)
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
	else if (eventID == Events::KeyDown)
	{
		keyboard.ListenOnKeyDown(box.Unbox<std::pair<int, char> >().second);
	}
	else if (eventID == Events::KeyUp)
	{
		keyboard.ListenOnKeyUp(box.Unbox<std::pair<int, char> >().second);
	}
}

void InGameMenuState::OnPop(float time)
{
	m_msm->GetEventDispatcher()->DetachEventHandler(this);
}

void InGameMenuState::Update(Render::View viewbox, float time, float deltaTime)
{
	m_timer += deltaTime;
	if (m_timer > 0.2)
	{
		Oquonie::GetInstance()->Animate();
		m_timer -= 0.2;
	}
	m_time += deltaTime;
	UI::DoLayout(m_root, viewbox, time, m_cursor_pos, m_cursor_down, m_cursor_action);
	m_cursor_action = false;
}

void InGameMenuState::Draw(Render::View viewbox, Render::Renderer2D* rd)
{
	if (m_frame < 2)
		m_time = 0;
	UI::Render(rd, m_root, viewbox, m_time);
	++m_frame;
}

std::string InGameMenuState::GetName()
{
	return m_name;
}


RegisterMenu(InGameMenuState);
