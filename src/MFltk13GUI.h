#ifndef MFLTK13GUI_HEADER_INCLUDED
#define MFLTK13GUI_HEADER_INCLUDED

#include "MGUI.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

class MFltk13GUI : public MGUI, public Fl_Window {
  public:
	MFltk13GUI(MCore* mCore);
	~MFltk13GUI();

	void openDir(std::string);
	int showWindow();
};

#endif // MFLTK13GUI_HEADER_INCLUDED
