#pragma once

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

	public:
		GameMenu(IrrlichtDevice * dev);
		~GameMenu(void);

		void Run();

		bool OnEvent(const SEvent&);
	};
}
