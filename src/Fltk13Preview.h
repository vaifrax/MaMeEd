#ifndef F13PREVIEW_HEADER_INCLUDED
#define F13PREVIEW_HEADER_INCLUDED

#include <string>

#include "FL/Fl_Box.H"

class Fltk13Preview : public Fl_Box {
  public:
	Fltk13Preview(int X, int Y, int W, int H);

	// show 1st image, cache other two
//TODO: load other images in extra thread
	void setImg(std::string fileName, std::string prevFileName, std::string nextFileName);

  protected:
	Fl_Image* img;

	struct ImgCacheStruct {
		std::string fileName;
		Fl_Image* img;
	} cache[3];

	Fl_Image* loadImg(std::string fileName);
};


#endif F13PREVIEW_HEADER_INCLUDED
