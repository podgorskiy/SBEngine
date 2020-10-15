#include "BaseMenuState.h"
#include "MenuStateManager.h"
#include "MenuFactory.h"
#include "UI/UI_Load.h"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <fsal.h>


BaseMenuState::BaseMenuState(const fsal::File& yaml, MenuStateManager* msm): m_msm(msm), m_eventDispatcher(msm->GetEventDispatcher())
{
	m_root = msm->Load(yaml);
	YAML::Node root_node = YAML::Load(std::string(yaml));
	auto name = root_node["menu_name"];
	m_name = name.as<std::string>("noname");
	m_time = 0.0f;
}

void BaseMenuState::Update(Render::View viewbox, float time, float deltaTime)
{
	m_time += deltaTime;
	UI::DoLayout(m_root, viewbox, time);
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
