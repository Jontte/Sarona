#include "StdAfx.h"
#include "TimedEventReceiver.h"


namespace Sarona
{
	void TimedEventReceiver::Schedule(double timeout, const shared_ptr<Event>& e)
	{
		e->m_time = m_simtime + timeout;
		m_events.push_back(e);
	};
	void TimedEventReceiver::TimeStep(double dt)
	{
		// Step forward in time.. Time travel!
		m_simtime += dt;

		/*
		// group outdated events together
		vector<shared_ptr<Event>>::iterator iter = std::remove_if(m_events.begin(), m_events.end(), [&](shared_ptr<Event> e){
			return m_simtime >= e->m_time;
		});

		// execute them
		std::for_each(iter, m_events.end(), [](shared_ptr<Event> e){
			// unless cancelled
			if(!e->m_cancel)
				(*e)();
		});

		// remove them
		if(iter != m_events.end())
			m_events.erase(iter, m_events.end());
		*/

		for(int i = 0 ; i < (int)m_events.size(); i++)
		{
			shared_ptr<Event> e = m_events[i];

			if(m_simtime >= e->m_time)
			{
				// Run it unless already cancelled
				if(!e->m_cancel)
					(*e)();

				// Drop it

				m_events.erase(m_events.begin() + i);
				i--;
			}
		}
	}
}
