#include "StdAfx.h"
#include "Util.h"

// Reads the whole file to a string
bool readFile(
	IrrlichtDevice* irr,
	const std::string& path,
	std::string& result)
{
	intrusive_ptr<io::IReadFile> file(irr->getFileSystem()->createAndOpenFile(path.c_str()), false);

	if(!file.get())
		return false;
	result.resize(file->getSize());

	int bytesleft = file->getSize();
	int bytesread = 0;
	while(bytesleft > 0)
	{
		int bs = file->read(&result[bytesread], bytesleft);

		bytesread += bs;
		bytesleft -= bs;
	}
	return true;
}

void btTransformToIrrlichtMatrix(const btTransform& worldTrans, irr::core::matrix4 &matr)
{
    worldTrans.getOpenGLMatrix(matr.pointer());
}


void btTransformFromIrrlichtMatrix(const irr::core::matrix4& irrmat, btTransform &transform)
{
    transform.setIdentity();

    transform.setFromOpenGLMatrix(irrmat.pointer());
}

// Make Irrlicht's reference counted objects work together with intrusive_ptr
void intrusive_ptr_add_ref(IReferenceCounted* t)
{
	if(t)
		t->grab();
}
void intrusive_ptr_release(IReferenceCounted* t)
{
	if(t)
		t->drop();
}

string keycode2string(irr::u8 code)
{
#define C(CODE,STRING) case CODE: return STRING;

	switch(code)
	{

		C(KEY_LBUTTON, "lmb");
		C(KEY_RBUTTON, "rmb");
//		C(KEY_CANCEL, // Control-break processing
		C(KEY_MBUTTON, "rmb"); // Middle mouse button (three-button mouse)
//		C(KEY_XBUTTON1         = 0x05,  // Windows 2000/XP: X1 mouse button
//		C(KEY_XBUTTON2         = 0x06,  // Windows 2000/XP: X2 mouse button
		C(KEY_BACK, "backspace"); // BACKSPACE key
		C(KEY_TAB, "tab");//Tab key
//		C(KEY_CLEAR            = 0x0C,  // CLEAR key
		C(KEY_RETURN, "enter"); // ENTER key
		C(KEY_SHIFT, "shift"); // SHIFT key
		C(KEY_CONTROL, "ctrl"); // CTRL key
		C(KEY_MENU, "alt"); // ALT key
		C(KEY_PAUSE, "pause"); // PAUSE key
		C(KEY_CAPITAL, "capslock"); // CAPS LOCK key
//		C(KEY_KANA             = 0x15,  // IME Kana mode
//		C(KEY_HANGUEL          = 0x15,  // IME Hanguel mode (maintained for compatibility use KEY_HANGUL)
//		C(KEY_HANGUL           = 0x15,  // IME Hangul mode
//		C(KEY_JUNJA            = 0x17,  // IME Junja mode
//		C(KEY_FINAL            = 0x18,  // IME final mode
//		C(KEY_HANJA            = 0x19,  // IME Hanja mode
//		C(KEY_KANJI            = 0x19,  // IME Kanji mode
		C(KEY_ESCAPE, "esc"); // ESC key
//		C(KEY_CONVERT          = 0x1C,  // IME convert
//		C(KEY_NONCONVERT       = 0x1D,  // IME nonconvert
//		C(KEY_ACCEPT           = 0x1E,  // IME accept
//		C(KEY_MODECHANGE       = 0x1F,  // IME mode change request
		C(KEY_SPACE, "space"); // SPACEBAR
		C(KEY_PRIOR, "pageup"); // PAGE UP key
		C(KEY_NEXT, "pagedown"); // PAGE DOWN key
		C(KEY_END, "end"); // END key
		C(KEY_HOME, "home"); // HOME key
		C(KEY_LEFT, "left"); // LEFT ARROW key
		C(KEY_UP,   "up" ); // UP ARROW key
		C(KEY_RIGHT, "right"); // RIGHT ARROW key
		C(KEY_DOWN, "down"); // DOWN ARROW key
//		C(KEY_SELECT           = 0x29,  // SELECT key
//		C(KEY_PRINT            = 0x2A,  // PRINT key
//		C(KEY_EXECUT           = 0x2B,  // EXECUTE key
//		C(KEY_SNAPSHOT         = 0x2C,  // PRINT SCREEN key
		C(KEY_INSERT, "insert"); // INS key
		C(KEY_DELETE, "delete"); // DEL key
//		C(KEY_HELP             = 0x2F,  // HELP key
		C(KEY_KEY_0, "0");// 0 key
		C(KEY_KEY_1, "1"); // 1 key
		C(KEY_KEY_2, "2"); // 2 key
		C(KEY_KEY_3, "3"); // 3 key
		C(KEY_KEY_4, "4"); // 4 key
		C(KEY_KEY_5, "5"); // 5 key
		C(KEY_KEY_6, "6"); // 6 key
		C(KEY_KEY_7, "7"); // 7 key
		C(KEY_KEY_8, "8"); // 8 key
		C(KEY_KEY_9, "9"); // 9 key
		C(KEY_KEY_A, "A"); // A key
		C(KEY_KEY_B, "B"); // B key
		C(KEY_KEY_C, "C"); // C key
		C(KEY_KEY_D, "D"); // D key
		C(KEY_KEY_E, "E"); // E key
		C(KEY_KEY_F, "F"); // F key
		C(KEY_KEY_G, "G"); // G key
		C(KEY_KEY_H, "H"); // H key
		C(KEY_KEY_I, "I"); // I key
		C(KEY_KEY_J, "J"); // J key
		C(KEY_KEY_K, "K"); // K key
		C(KEY_KEY_L, "L"); // L key
		C(KEY_KEY_M, "M"); // M key
		C(KEY_KEY_N, "N"); // N key
		C(KEY_KEY_O, "O"); // O key
		C(KEY_KEY_P, "P"); // P key
		C(KEY_KEY_Q, "Q"); // Q key
		C(KEY_KEY_R, "R"); // R key
		C(KEY_KEY_S, "S"); // S key
		C(KEY_KEY_T, "T"); // T key
		C(KEY_KEY_U, "U"); // U key
		C(KEY_KEY_V, "V"); // V key
		C(KEY_KEY_W, "W"); // W key
		C(KEY_KEY_X, "X"); // X key
		C(KEY_KEY_Y, "Y"); // Y key
		C(KEY_KEY_Z, "Z"); // Z key
		C(KEY_LWIN, "leftwin"); // Left Windows key (Microsoft® Natural® keyboard)
		C(KEY_RWIN, "rightwin"); // Right Windows key (Natural keyboard)
//		C(KEY_APPS             = 0x5D,  // Applications key (Natural keyboard)
//		C(KEY_SLEEP            = 0x5F,  // Computer Sleep key
		C(KEY_NUMPAD0, "num1"); // Numeric keypad 0 key
		C(KEY_NUMPAD1, "num2"); // Numeric keypad 1 key
		C(KEY_NUMPAD2, "num3"); // Numeric keypad 2 key
		C(KEY_NUMPAD3, "num4"); // Numeric keypad 3 key
		C(KEY_NUMPAD4, "num5");  // Numeric keypad 4 key
		C(KEY_NUMPAD5, "num6"); // Numeric keypad 5 key
		C(KEY_NUMPAD6, "num7");  // Numeric keypad 6 key
		C(KEY_NUMPAD7, "num7"); // Numeric keypad 7 key
		C(KEY_NUMPAD8, "num8"); // Numeric keypad 8 key
		C(KEY_NUMPAD9, "num9"); // Numeric keypad 9 key
		C(KEY_MULTIPLY, "multiply"); // Multiply key
		C(KEY_ADD, "add"); // Add key
		C(KEY_SEPARATOR, "separator"); // Separator key
		C(KEY_SUBTRACT, "subtract"); // Subtract key
		C(KEY_DECIMAL, "decimal"); // Decimal key
		C(KEY_DIVIDE, "divide" ); // Divide key
		C(KEY_F1, "f1");  // F1 key
		C(KEY_F2, "f2");  // F2 key
		C(KEY_F3, "f3");  // F3 key
		C(KEY_F4, "f4");  // F4 key
		C(KEY_F5, "f5");  // F5 key
		C(KEY_F6, "f6");  // F6 key
		C(KEY_F7, "f7");  // F7 key
		C(KEY_F8, "f8");  // F8 key
		C(KEY_F9, "f9");  // F9 key
		C(KEY_F10, "f10");  // F10 key
		C(KEY_F11, "f11");  // F11 key
		C(KEY_F12, "f12");  // F12 key
		C(KEY_F13, "f13");  // F13 key
		C(KEY_F14, "f14");  // F14 key
		C(KEY_F15, "f15");  // F15 key
		C(KEY_F16, "f16");  // F16 key
		C(KEY_F17, "f17");  // F17 key
		C(KEY_F18, "f18");  // F18 key
		C(KEY_F19, "f19");  // F19 key
		C(KEY_F20, "f20");  // F20 key
		C(KEY_F21, "f21");  // F21 key
		C(KEY_F22, "f22");  // F22 key
		C(KEY_F23, "f23");  // F23 key
		C(KEY_F24, "f24");  // F24 key
//		C(KEY_NUMLOCK          = 0x90,  // NUM LOCK key
//		C(KEY_SCROLL           = 0x91,  // SCROLL LOCK key
		C(KEY_LSHIFT, "lshift"); // Left SHIFT key
		C(KEY_RSHIFT, "rshift"); // Right SHIFT key
		C(KEY_LCONTROL, "lctrl"); // Left CONTROL key
		C(KEY_RCONTROL, "rctrl"); // Right CONTROL key
//		C(KEY_LMENU            = 0xA4,  // Left MENU key
//		C(KEY_RMENU            = 0xA5,  // Right MENU key
		C(KEY_PLUS, "plus"); // Plus Key   (+)
		C(KEY_COMMA, "comma"); // Comma Key  (,)
		C(KEY_MINUS, "minus"); // Minus Key  (-)
		C(KEY_PERIOD, "period"); // Period Key (.)
//		C(KEY_ATTN             = 0xF6,  // Attn key
//		C(KEY_CRSEL            = 0xF7,  // CrSel key
//		C(KEY_EXSEL            = 0xF8,  // ExSel key
//		C(KEY_EREOF            = 0xF9,  // Erase EOF key
//		C(KEY_PLAY             = 0xFA,  // Play key
//		C(KEY_ZOOM             = 0xFB,  // Zoom key
//		C(KEY_PA1              = 0xFD,  // PA1 key
//		C(KEY_OEM_CLEAR        = 0xFE,   // Clear key
	}
	return "";
#undef C
}


// Loads a 2d
class HeightMapLoader : public scene::IMeshLoader
{
private:
	IrrlichtDevice* m_device;

	float get_height(video::IImage* img, int x, int y)
	{
		return img->getPixel(x,  y  ).getLightness()/255;
	}
public:
	HeightMapLoader(IrrlichtDevice * dev) : m_device(dev){}

	virtual scene::IAnimatedMesh * createMesh (io::IReadFile *file)
	{
		video::IVideoDriver * driver = m_device->getVideoDriver();
		intrusive_ptr<video::IImage> img =
			intrusive_ptr<video::IImage>(driver->createImageFromFile(file), false);
		if(!img)
		{
			return false;
		}

		core::dimension2d<u32> size = img->getDimension();


		intrusive_ptr<scene::SMesh> mesh(new scene::SMesh, false);
		intrusive_ptr<scene::IAnimatedMesh> animmesh(new scene::SAnimatedMesh(get_pointer(mesh)), false);

		const u32 meshbuffer_size = driver -> getMaximalPrimitiveCount();

		int rows_per_buffer = meshbuffer_size / ((size.Width-1) * 2);

		int trianglecount = 2 * (size.Width-1) * (size.Height-1);
		int buffercount = trianglecount/meshbuffer_size+1;

		for(int i = 0; i < buffercount; i++)
		{
			int starty = rows_per_buffer*i;
			int endy = rows_per_buffer*(i+1);

			if(endy > size.Height)endy = size.Height;

			// create new buffer...
			scene::SMeshBuffer * buffer = new scene::SMeshBuffer;
			mesh->addMeshBuffer(buffer);
			buffer->drop(); // now owned by mesh

			buffer->Vertices.set_used((1 + endy - starty) * size.Width);
			int idx = 0;
			for(int y = starty; y < endy; y++)
			{
				for(int x = 0; x < size.Width; x++)
				{
					 // center the model around -0.5 -> 0.5 on every axis
					float pz = get_height(get_pointer(img), x, y);
					float px = float(x)/(size.Width-1);
					float py = float(y)/(size.Height-1);

					video::S3DVertex& v = buffer->Vertices[idx++];
					v.Pos.set(-0.5+px, -0.5+py, -0.5+pz);

					v.Normal.set(0,0,1);

					if(x < size.Width-1)
						v.Normal.X -= size.Width/(get_height(get_pointer(img), x+1, y)-get_height(get_pointer(img), x, y));
					if(x > 0)
						v.Normal.X += size.Width/(get_height(get_pointer(img), x-1, y)-get_height(get_pointer(img), x, y));

					if(y < size.Height-1)
						v.Normal.Y -= size.Height/(get_height(get_pointer(img), x, y+1)-get_height(get_pointer(img), x, y));
					if(y > 0)
						v.Normal.Y += size.Height/(get_height(get_pointer(img), x, y-1)-get_height(get_pointer(img), x, y));

					v.Normal.normalize();

					v.Color.set(255,255,255,255);
					v.TCoords.set(px, py);
				}
			}

			idx = 0;
			buffer->Indices.set_used(6 * (size.Width-1) * (endy-starty));
			for(int y = starty; y < endy; y++)
			{
				for(int x = 0; x < size.Width-1; x++)
				{
					int n = (y-starty) * size.Width + x;
					buffer->Indices[idx++] = n;
					buffer->Indices[idx++] = n + size.Height + 1;
					buffer->Indices[idx++] = n + size.Height;

					buffer->Indices[idx++] = n + size.Height + 1;
					buffer->Indices[idx++] = n;
					buffer->Indices[idx++] = n + 1;
				}
			}

			buffer -> recalculateBoundingBox();
		}
		// make animmesh outlive this scope
		animmesh->grab();
		return get_pointer(animmesh);
	}
	virtual bool isALoadableFileExtension (const io::path &filename) const
	{
		return filename.subString(filename.size()-3, filename.size()) == "png";
	}
};


void addHeightMapLoader(IrrlichtDevice* device)
{
	HeightMapLoader * loader = new HeightMapLoader(device);
	device->getSceneManager()->addExternalMeshLoader(loader);
}

SimpleTimer::SimpleTimer()
{
	restart();
}
void SimpleTimer::restart()
{
	m_start = boost::posix_time::microsec_clock::local_time();
}
double SimpleTimer::elapsed(const boost::posix_time::ptime& now)
{
	boost::posix_time::time_period period(m_start, now);
	return double(period.length().total_microseconds())/1000000;
}





