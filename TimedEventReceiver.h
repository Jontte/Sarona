#pragma once
#include "StdAfx.h"

namespace Sarona
{
	class TimedEventReceiver;
	class Event
	{
		friend class TimedEventReceiver;
		private:
			double m_time;
			bool m_cancel;
			virtual void operator()() = 0;
		public:
			Event() : m_time(0), m_cancel(false){}
			void cancel()	{ m_cancel = true; };
	};

	class TimedEventReceiver
	{
	private:
		vector<shared_ptr<Event>> m_events;
		double m_simtime;
	public:
		TimedEventReceiver() : m_simtime(0) {}
		~TimedEventReceiver(){};

		// Step simulation by dt, dispatch events.
		void TimeStep(double dt);

		// Add an event
		void Schedule(double seconds, shared_ptr<Event>);
	};

}
