#ifndef FL_IMG_TOOLS_HEADER_INCLUDED
#define FL_IMG_TOOLS_HEADER_INCLUDED

#include <FL/Fl_Image.H>

class FlImgTools {
  public:
	static Fl_Image* rotate(Fl_Image* img, int rotateClockwDeg);
};

#endif // FL_IMG_TOOLS_HEADER_INCLUDED
