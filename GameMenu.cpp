#include "StdAfx.h"
#include "GameMenu.h"
#include "PhysWorld.h"
#include "NetWorld.h"

namespace Sarona
{
	enum {
		GUI_ID_BUTTON_LOCAL = 101,
		GUI_ID_BUTTON_HOST,
		GUI_ID_BUTTON_JOIN,
		GUI_ID_BUTTON_SETTINGS,
		GUI_ID_BUTTON_ABOUT,
		GUI_ID_BUTTON_QUIT
	};


	GameMenu::GameMenu(IrrlichtDevice * dev)
		:	m_device(dev)
		,	m_runlocal(true)
		,	m_runserver(true)
		,	m_finished(false)
	{
		dev->setEventReceiver(this);
	}


	GameMenu::~GameMenu()
	{
	}

	bool GameMenu::OnEvent(const SEvent& event)
	{
//		gui::IGUIEnvironment* env = m_device->getGUIEnvironment();

		if (event.EventType != EET_GUI_EVENT)
        {
			return false;
		}

		s32 id = event.GUIEvent.Caller->getID();

        switch(event.GUIEvent.EventType)
        {
		case gui::EGET_BUTTON_CLICKED:
            switch(id)
            {
            case GUI_ID_BUTTON_LOCAL:
				m_runserver = true;
				m_runlocal = true;
				m_finished = true;
                return true;
            case GUI_ID_BUTTON_HOST:
				m_runserver = true;
				m_runlocal = false;
				m_finished = true;
                return true;
            case GUI_ID_BUTTON_JOIN:
				m_runserver = false;
				m_runlocal = false;
				m_finished = true;
                return true;
            case GUI_ID_BUTTON_SETTINGS:
				return true;
            case GUI_ID_BUTTON_ABOUT:
                return true;
            case GUI_ID_BUTTON_QUIT:
                m_device->closeDevice();
                return true;
            default:
                return false;
            }
            break;
        default:
			break;
        }
		return false;
	}

	void GameMenu::Run(bool &runlocal, bool &runserver)
	{
		video::IVideoDriver* driver = m_device->getVideoDriver();
		gui::IGUIEnvironment* env = m_device->getGUIEnvironment();

		m_device->setWindowCaption(L"Sarona");
		m_device->getLogger()->setLogLevel(ELL_INFORMATION);
		m_device->setResizable(true);

		// Create buttons...

		const wchar_t* buttonlabels[6][2] = {
			{L"Local Game"	, L"Start a single-player game on this computer"},
			{L"Host Game"	, L"Host game other players can join"},
			{L"Join Game"	, L"Join a hosted game"},
			{L"Settings"	, L"Open settings dialog"},
			{L"About"		, L"Open about dialog"},
			{L"Quit"		, L"Quit the game"},
		};

		s32 ids[6] = {
			GUI_ID_BUTTON_LOCAL,
			GUI_ID_BUTTON_HOST,
			GUI_ID_BUTTON_JOIN,
			GUI_ID_BUTTON_SETTINGS,
			GUI_ID_BUTTON_ABOUT,
			GUI_ID_BUTTON_QUIT
		};

		for(int i = 0 ; i < 6; i++)
		{
			core::rect<s32> rect(200, i*50, 300, (i)*50+25);

			env->addButton(rect, 0, ids[i], buttonlabels[i][0], buttonlabels[i][1]);
		}

		// Run UI loop

		while(m_device->run() && driver)
		{
			if (m_device->isWindowActive())
			{
				driver->beginScene(true, true, video::SColor(0,128,160,255));

				env->drawAll();

				driver->endScene();
			}

			// Sleep 100 ms.
			boost::this_thread::sleep(
				boost::posix_time::milliseconds(100)
			);

			// Are we finished?
			if(m_finished)
			{
				runlocal = m_runlocal;
				runserver = m_runserver;
				break;
			}
		}

	}
}
