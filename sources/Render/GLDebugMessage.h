#pragma once
#include <inttypes.h>


namespace Render
{
	bool ProcessGLErrorEvents(bool reportErrors = false);

	class DebugDevice
	{
	public:
		void lock();
		void unlock();

		bool enabled_high;
		bool enabled_medium;
		bool enabled_low;
		bool enabled_notify;
	};

	template<bool high=true, bool medium=true, bool low=true, bool notify=false>
	class debug_guard
	{
	public:
	    debug_guard() : m_device(DebugDevice({high, medium, low, notify}))
	    {
	    	m_device.lock();
	    }
	    explicit debug_guard(DebugDevice& m) : m_device(m)
	    {
	    	m_device.lock();
	    }
	    ~debug_guard()
	    {
	    	m_device.unlock();
	    }

	    debug_guard(const debug_guard&) = delete;
	    debug_guard& operator=(const debug_guard&) = delete;

	private:
		DebugDevice  m_device;
	};
}
