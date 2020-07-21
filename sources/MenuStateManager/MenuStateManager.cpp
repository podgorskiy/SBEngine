#include "IMenuState.h"
#include "MenuStateManager.h"
#include "MenuFactory.h"
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include "utils/assertion.h"



MenuStateManager::MenuStateManager(): m_frameCount(0),m_dtime(0), m_fps(0)
{
	spdlog::info("Starting MenuStateManager");
}


MenuStateManager::~MenuStateManager()
{
	spdlog::info("Destroying MenuStateManager");
}

void MenuStateManager::Update(Render::View viewbox, float time, float deltaTime)
{
	auto state = GetFrontState();
	if (state)
		state->Update(viewbox, time, deltaTime);
	m_dtime += deltaTime;
	m_frameCount++;
	if (m_dtime > 500)
	{
		m_fps = (1000 * m_frameCount)/m_dtime;
		m_frameCount = 0;
		m_dtime = 0;
	}

	m_deletionQueue.clear();
}

void MenuStateManager::Draw(Render::View viewbox, Render::Renderer2D* rd, float time, float deltaTime)
{
	auto state = GetFrontState();
	if (state)
		state->Draw(viewbox, rd);
}

void MenuStateManager::Push(std::string filename)
{
    using fs = fsal::FileSystem;
    auto f = fs().Open(filename);
	ASSERT(f, "Could not find menu file %s", filename.c_str());
	spdlog::info("Loading: {}", f.GetPath().string().c_str());

	YAML::Node root_node = YAML::Load(std::string(f));

	auto class_ = root_node["class"];
	ASSERT(class_.IsDefined(), "No class defines");

	auto ctr = MenuRegisterer::GetConstructor(class_.as<std::string>().c_str());
	auto state = ctr(f, this);

	spdlog::info("Pushing state: {}", state->GetName());
	state->OnPush();
	m_stack.push_back(std::make_pair(std::move(state), filename));
}


void MenuStateManager::Reload()
{
	if (!m_stack.empty())
	{
		auto filename = m_stack.back().second;
		Pop();
		Push(filename);
	}
}


void MenuStateManager::Pop()
{
	IMenuStatePtr state = m_stack.back().first;
	spdlog::info("Poping state: {}", state->GetName());
	state->OnPop();
	m_stack.pop_back();
	m_deletionQueue.push_back(std::move(state));
}

IMenuStatePtr MenuStateManager::GetFrontState(int i) const
{
	if (m_stack.size() > i)
	{
		return m_stack[m_stack.size() - 1 - i].first;
	}
	return nullptr;
}

//
//void MenuStateManager::EventReceiver_OnEvent( EventBase* event )
//{
//	m_stack.back()->OnEvent( event );
//}
