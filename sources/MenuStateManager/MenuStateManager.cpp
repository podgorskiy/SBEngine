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

void MenuStateManager::InjectEventDispatcher(core::EventDispatcher* eventDispatcher)
{
	m_eventDispatcher = eventDispatcher;
}

MenuStateManager::~MenuStateManager()
{
	spdlog::info("Destroying MenuStateManager");
}


std::pair<UI::BlockPtr, UI::Animation> MenuStateManager::Load(const fsal::File& yaml)
{
	return UI::Load(yaml, m_tf_map, m_shader_map);
}


void MenuStateManager::Update(Render::View viewbox, float time, float deltaTime)
{
	for (auto& com: m_commands)
	{
		switch (com.first)
		{
			case CPop:
			{
				IMenuStatePtr state = m_stack.back().first;
				spdlog::info("Poping state: {}", state->GetName());
				state->OnPop(time);
				m_stack.pop_back();
				break;
			}
			case CPush:
			{
				auto filename = com.second;
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
				state->OnPush(time);
				m_stack.push_back(std::make_pair(std::move(state), filename));
				break;
			}
			default:
				assert(false);
		}
	}
	m_commands.clear();

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
}

void MenuStateManager::Draw(Render::View viewbox, Render::Renderer2D* rd, float time, float deltaTime)
{
	auto state = GetFrontState();
	if (state)
		state->Draw(viewbox, rd);
}

void MenuStateManager::Push(std::string filename)
{
	m_commands.push_back(std::make_pair(CPush, std::move(filename)));
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
	m_commands.push_back(std::make_pair(CPop, std::string()));
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
