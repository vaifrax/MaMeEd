#ifndef F13PREVIEW_HEADER_INCLUDED
#define F13PREVIEW_HEADER_INCLUDED

#include <string>

#include "FL/Fl_Box.H"

class Fltk13Preview : public Fl_Box {
  public:
	Fltk13Preview(int X, int Y, int W, int H);

	// show 1st image, cache other two
//TODO: load other images in extra thread
	void setImg(std::string fileName, int imgExifStorageOrientation,
				std::string prevFileName, int prevImgExifStorageOrientation,
				std::string nextFileName, int nextImgExifStorageOrientation);
  protected:
	Fl_Image* img;

	struct ImgCacheStruct {
		std::string fileName;
		Fl_Image* img;
	} cache[3];

	Fl_Image* loadImg(std::string fileName, int exifStorageOrientation);


public:
void printCache(ImgCacheStruct* cache);

};


#endif F13PREVIEW_HEADER_INCLUDED
