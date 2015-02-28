#ifndef F13PREVIEW_HEADER_INCLUDED
#define F13PREVIEW_HEADER_INCLUDED

#include <string>

#include "FL/Fl_Box.H"

class Fltk13Preview : public Fl_Box {
  public:
	Fltk13Preview(int X, int Y, int W, int H);

	// TODO: cache 3 images and load in extra thread
	void setImg(std::string fileName);

  protected:
	Fl_Image* img;
};


#endif F13PREVIEW_HEADER_INCLUDED
