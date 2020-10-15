#pragma once
#include "InputEventReciever.h"
#include "IMenuState.h"
#include "2DEngine/View.h"
#include "2DEngine/Renderer2D.h"
#include "UI/UI_Load.h"
#include "EventDispatcher/EventDispatcher.h"
#include <memory>
#include <vector>


typedef std::vector<std::pair<IMenuStatePtr, std::string> > StateStack;

class MenuStateManager
{
	enum Command
	{
		CNone,
		CPop,
		CPush
	};

	typedef std::vector<std::pair<Command, std::string> > Commands;

public:
	MenuStateManager();
	~MenuStateManager();

	void Pop();

	void Update(Render::View viewbox, float time, float deltaTime);

	void Draw(Render::View viewbox, Render::Renderer2D* rd, float time, float deltaTime);

	void Push(std::string filename);

	void Reload();

	void InjectMaps(const std::map<std::string, int>& tf_map, const std::map<std::string, int>& shader_map)
	{
		m_tf_map = tf_map;
		m_shader_map = shader_map;
	}

	void InjectEventDispatcher(core::EventDispatcher* eventDispatcher) { m_eventDispatcher = eventDispatcher; }

	core::EventDispatcher* GetEventDispatcher() const { return m_eventDispatcher; }

	UI::BlockPtr Load(const fsal::File& yaml);

	// void EventReceiver_OnEvent( EventBase* event );

	IMenuStatePtr GetFrontState(int i=0) const;

private:
	StateStack m_stack;
	Commands m_commands;

	int m_frameCount;
	float m_dtime;
	float m_fps;

	std::map<std::string, int> m_tf_map;
	std::map<std::string, int> m_shader_map;
	core::EventDispatcher* m_eventDispatcher;
};

typedef std::shared_ptr<MenuStateManager> MenuStateManagerPtr;
