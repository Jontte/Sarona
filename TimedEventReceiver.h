#pragma once
#include "StdAfx.h"

namespace Sarona
{
	class TimedEventReceiver
	{
	public:
		class Event
		{
			friend class TimedEventReceiver;
			private:
				double m_time;
				bool m_cancel;
				virtual void operator()() = 0;
			public:
				Event() : m_time(0), m_cancel(false){}
				virtual ~Event(){};
				void cancel()	{ m_cancel = true; };
		};
	private:
		vector<shared_ptr<Event>> m_events;
		double m_simtime;
	public:
		TimedEventReceiver() : m_simtime(0) {}
		~TimedEventReceiver(){};

		// Step simulation by dt, dispatch events.
		void TimeStep(double dt);

		// Add an event
		void Schedule(double seconds, const shared_ptr<Event>& );
	};

}
