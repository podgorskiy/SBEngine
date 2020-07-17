#pragma once
#include "utils/string_hash.h"
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>

namespace core
{
	class EventDispatcher
	{
	public:
		~EventDispatcher();

		struct IType
		{
			typedef int TypeID;
		protected:
			static int m_idGenerator;
		};

		class Box
		{
		public:
			template<typename T>
			void Pack(const T& obj);

			template<typename T>
			const T& Unbox() const;

			template<typename T>
			bool Is() const;

		private:
			template<typename T>
			struct DType : public IType
			{
				static IType::TypeID GetTypeID()
				{
					return GetTypeIDRef();
				}

			private:
				static IType::TypeID& GetTypeIDRef()
				{
					static IType::TypeID TypeId = m_idGenerator++;
					return TypeId;
				}
			};

			IType::TypeID m_type;
			void* m_data;
		};

		template<typename Handler>
		void AttachEventHandler(uint64_t eventID, bool (Handler::*callback)(uint64_t eventID, Box box), Handler* object,
		                        char priority = 50);

		void AttachEventHandler(uint64_t eventID, std::function<bool(uint64_t eventID, Box box)>, char priority = 0);

		template<typename Handler>
		void DetachEventHandler(Handler* object);

		template<typename Handler>
		void DetachEventHandler(Handler* object, uint64_t eventID);

		template<typename Handler>
		void Pause(Handler* object, bool enable);

		void Dispatch(uint64_t eventID);

		template<typename T>
		void Dispatch(uint64_t eventID, const T& obj);

	private:
		struct IEventAttachment
		{
			IEventAttachment(uint64_t eventID, char priority): m_eventID(eventID), m_priority(priority), m_enabled(true)
			{};

			virtual ~IEventAttachment() = default;

			virtual bool Dispatch(uint64_t eventID, Box box) = 0;

			uint64_t m_eventID;
			char m_priority;
			bool m_enabled;
		};

		template<typename Handler>
		struct EventAttachment : public IEventAttachment
		{
			EventAttachment(bool (Handler::*callback)(uint64_t eventID, Box box), Handler* handler, uint64_t eventID,
			                char priority):
					m_callback(callback), m_handler(handler), IEventAttachment(eventID, priority)
			{};

			EventAttachment(std::function<bool(uint64_t eventID, Box box)>&& func, uint64_t eventID, char priority):
					IEventAttachment(eventID, priority), m_callback(nullptr), m_func(func), m_handler(nullptr)
			{};

			bool Dispatch(uint64_t eventID, Box box) override;

			bool (Handler::*m_callback)(uint64_t eventID, Box box) = nullptr;

			std::function<bool(uint64_t eventID, Box box)> m_func;

			Handler* m_handler;
		};

		typedef std::vector<IEventAttachment*> Input;

		void Dispatch(uint64_t eventID, Box obj);

		void Attach(uint64_t eventID, IEventAttachment* attachment);

		std::map<uint64_t, Input> m_mapping;
		std::vector<IEventAttachment*> m_queue;
	};

	template<typename T>
	inline void EventDispatcher::Dispatch(uint64_t eventID, const T& obj)
	{
		Box box;
		box.Pack(obj);
		Dispatch(eventID, box);
	}

	template<typename T>
	inline void EventDispatcher::Box::Pack(const T& obj)
	{
		m_data = (void*) &obj;
		m_type = DType<T>::GetTypeID();
	}

	template<typename T>
	inline const T& EventDispatcher::Box::Unbox() const
	{
		assert(Is<T>());
		return *reinterpret_cast<const T*>(m_data);
	}

	template<typename T>
	inline bool EventDispatcher::Box::Is() const
	{
		return m_type == DType<T>::GetTypeID();
	}

	template<typename Handler>
	inline void
	EventDispatcher::AttachEventHandler(uint64_t eventID, bool (Handler::*callback)(uint64_t eventID, Box box),
	                                    Handler* object, char priority)
	{
		auto* attachment = new EventAttachment<Handler>(callback, object, eventID, priority);
		Attach(eventID, attachment);
	}

	inline void
	EventDispatcher::AttachEventHandler(uint64_t eventID, std::function<bool(uint64_t eventID, Box box)> lambda,
	                                    char priority)
	{
		auto* attachment = new EventAttachment<std::function<bool(
				uint64_t eventID, Box box)> >(std::move(lambda), eventID, priority);
		Attach(eventID, attachment);
	}

	inline void EventDispatcher::Dispatch(uint64_t eventID)
	{
		Dispatch(eventID, 0);
	}

	template<typename Handler>
	inline void EventDispatcher::DetachEventHandler(Handler* object)
	{
		for (auto& it : m_mapping)
		{
			Input& input = it.second;
			auto deleteIt = std::remove_if(input.begin(), input.end(), [object](IEventAttachment* x)
			{
				auto* attachment = static_cast<EventAttachment<Handler>*>(x);
				if (attachment->m_handler == object)
				{
					delete x;
				}
				return remove;
			});
			input.erase(deleteIt, input.end());
		}
	}

	template<typename Handler>
	inline void EventDispatcher::DetachEventHandler(Handler* object, uint64_t eventID)
	{
		for (auto& it : m_mapping)
		{
			const Input& input = it.second;
			auto deleteIt = std::remove_if(input.begin(), input.end(), [object, eventID](IEventAttachment* x)
			{
				auto* attachment = static_cast<EventAttachment<Handler>*>(x);
				if (attachment->m_handler == object && attachment->m_eventID == eventID)
				{
					delete x;
				}
				return remove;
			});
			input.erase(deleteIt, input.end());
		}
	}

	template<typename Handler>
	inline bool EventDispatcher::EventAttachment<Handler>::Dispatch(uint64_t eventID, Box box)
	{
		if (m_enabled)
		{
			if (m_handler == nullptr)
			{
				return m_func(eventID, box);
			}
			else
			{
				return ((*m_handler).*m_callback)(eventID, box);
			}
		}
		return false;
	}

	template<typename Handler>
	inline void EventDispatcher::Pause(Handler* object, bool enable)
	{
		for (auto& it_inputs : m_mapping)
		{
			Input& input = it_inputs.second;
			for (auto& it : input)
			{
				auto* attachment = static_cast<EventAttachment<Handler>*>(*it);
				if (attachment->m_subscriber == object)
				{
					attachment->m_active = enable;
				}
			}
		}
	}

	inline EventDispatcher& GetEventDispatcher()
	{
		static EventDispatcher ed;
		return ed;
	}
}
