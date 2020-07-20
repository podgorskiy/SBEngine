#pragma once
#include "IMenuState.h"
#include "UI/View.h"
#include "UI/UI_Backend.h"
#include <memory>
#include <vector>


typedef std::vector<std::pair<IMenuStatePtr, std::string> > StateStack;
typedef std::vector<IMenuStatePtr> StateQueue;

class MenuStateManager // : public EventReceiver
{
public:
	MenuStateManager();
	~MenuStateManager();

	void Pop();

	void Update(UI::View viewbox, float time, float deltaTime);

	void Draw(UI::View viewbox, UI::Renderer* rd, float time, float deltaTime);

	void Push(std::string filename);

	void Reload();

	// void EventReceiver_OnEvent( EventBase* event );

	IMenuStatePtr GetFrontState(int i=0) const;

private:
	void PushState(IMenuStatePtr state);

	StateStack m_stack;
	StateQueue m_deletionQueue;

	int m_frameCount;
	float m_dtime;
	float m_fps;
};

typedef std::shared_ptr<MenuStateManager> MenuStateManagerPtr;
