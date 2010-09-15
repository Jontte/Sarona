#pragma once
#ifndef GAMEMENU_H_
#define GAMEMENU_H_
namespace Sarona
{

// A class that manages the frontend user interface:
// - Main menu
// - Server selector
// - Level selector

	class GameMenu : public IEventReceiver
	{
	private:
		IrrlichtDevice * m_device;

		// Whether the game should be run locally (without even binding to ports)
		bool				m_runlocal;

		// Whether we should host the server too
		bool				m_runserver;

		// This will allow us to break out from the irrlicht loop
		bool				m_finished;

	public:
		GameMenu(IrrlichtDevice * dev);
		~GameMenu(void);

		void Run(bool& run_local, bool& run_server);

		bool OnEvent(const SEvent&);
	};
}
#endif
