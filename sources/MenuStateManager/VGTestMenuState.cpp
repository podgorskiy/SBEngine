#include "VGTestMenuState.h"
#include "MenuFactory.h"
#include "utils/assertion.h"
#include <spdlog/spdlog.h>
#include <dear-imgui/imgui.h>
#include <yaml-cpp/yaml.h>
#include "EventIds.h"
#include "EventDispatcher/EventDispatcher.h"



VGTestMenuState::VGTestMenuState(const fsal::File& yaml, MenuStateManager* msm): BaseMenuState(yaml, msm)
{
}

VGTestMenuState::~VGTestMenuState()
{
}

void VGTestMenuState::Update(Render::View viewbox, float time, float deltaTime)
{
	BaseMenuState::Update(viewbox, time, deltaTime);
}

void VGTestMenuState::Draw(Render::View viewbox, Render::Renderer2D* rd)
{
	BaseMenuState::Draw(viewbox, rd);
}

RegisterMenu(VGTestMenuState);
