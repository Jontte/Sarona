#include "StdAfx.h"
#include "TimedEventReceiver.h"


namespace Sarona
{
	void TimedEventReceiver::Schedule(double timeout, shared_ptr<Event> e)
	{
		e->m_time = m_simtime + timeout;
		m_events.push_back(e);
	};
	void TimedEventReceiver::TimeStep(double dt)
	{
		vector<shared_ptr<Event>>::iterator iter = m_events.begin();
		while(iter != m_events.end())
		{
			if((m_simtime + dt) >= (*iter)->m_time)
			{
				// Run & Drop it
				if((*iter)->m_cancel) // Don't run if already cancelled
					(**iter)();

				iter = m_events.erase(iter);
			}
			else
			{
				iter++;
			}
		}

		// Step forward in time.. Time travel!
		m_simtime += dt;
	}
}
