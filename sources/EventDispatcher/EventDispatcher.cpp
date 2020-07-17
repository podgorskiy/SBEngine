#include "EventDispatcher.h"
#include <algorithm>

using namespace core;

EventDispatcher::~EventDispatcher()
{
	for (auto& m : m_mapping)
	{
		for (auto& it : m.second)
		{
			delete it;
		}
	}
}

void EventDispatcher::Attach(uint64_t eventID, IEventAttachment* attachment)
{
	Input& input = m_mapping[eventID];
	input.push_back(attachment);
	std::sort(input.begin(), input.end(), [](IEventAttachment* a, IEventAttachment* b) { return a->m_priority > b->m_priority; });
}

void EventDispatcher::Dispatch(uint64_t eventID, Box box)
{
	auto it = m_mapping.find(eventID);

	if (it != m_mapping.end())
	{
		auto& input = it->second;
		for (auto& attachment : input)
		{
			m_queue.push_back(attachment);
		}

		for (auto& it_q : m_queue)
		{
			if (it_q->Dispatch(eventID, box))
			{
				return;
			}
		}
		m_queue.clear();
	}
}

int EventDispatcher::IType::m_idGenerator = 0;
