#pragma once
#include "BaseMenuState.h"
#include "MenuStateManager.h"
#include "MenuFactory.h"
#include "UI/UI_Load.h"
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <fsal.h>


BaseMenuState::BaseMenuState(const fsal::File& yaml, MenuStateManager* msm): m_msm(msm)
{
	using fs = fsal::FileSystem;
	using loc = fsal::Location;
	m_root = UI::Load(yaml);

	YAML::Node root_node = YAML::Load(std::string(yaml));
	auto name = root_node["menu_name"];
	m_name = name.as<std::string>("noname");
}

void BaseMenuState::Update(UI::View viewbox, float time, float deltaTime)
{
	UI::DoLayout(m_root, viewbox);
}

void BaseMenuState::Draw(UI::View viewbox, UI::Renderer* rd)
{
	UI::Render(rd, m_root, viewbox);
}

std::string BaseMenuState::GetName()
{
	return m_name;
}


RegisterMenu(BaseMenuState);

