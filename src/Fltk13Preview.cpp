#include "Fltk13Preview.h"

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_JPEG_Image.H> // for large preview img

#include <iostream> //for cout only

Fltk13Preview::Fltk13Preview(int X, int Y, int W, int H) : Fl_Box(X,Y,W,H,0) {
	img = NULL;
	for (int i=0; i<3; i++) {
		cache[i].fileName = std::string();
		cache[i].img = NULL;
	}
}

void Fltk13Preview::setImg(std::string fileName, std::string prevFileName, std::string nextFileName) {
	Fl_Image* prevImg = NULL;
	Fl_Image* nextImg = NULL;
	img = NULL;

	// check cache images if they are still being used
	for (int i=0; i<3; i++) {
		bool used = false;

		if (cache[i].fileName.length()) {
			if (cache[i].fileName == fileName) {
				img = cache[i].img;
				used = true;
			}
			if (cache[i].fileName == prevFileName) {
				prevImg = cache[i].img;
				used = true;
			}
			if (cache[i].fileName == nextFileName) {
				nextImg = cache[i].img;
				used = true;
			}
		}

		// if not used: delete
		if (!used && cache[i].img) {
			delete cache[i].img;
			cache[i].img = NULL;
			cache[i].fileName = std::string();
		}
	}
std::cout << "START" << std::endl;
	// load missing images
	if (!img) img = loadImg(fileName);
	if (!prevImg) prevImg = loadImg(prevFileName);
	if (!nextImg) nextImg = loadImg(nextFileName);

	cache[0].fileName = fileName;
	cache[0].img = img;
	cache[1].fileName = prevFileName;
	cache[1].img = prevImg;
	cache[2].fileName = nextFileName;
	cache[2].img = nextImg;
std::cout << "END LOADING" << std::endl;

	// refresh preview
	if (img && (img->w() > 0)) {
		image(img);
		redraw();
	}
std::cout << "END REDRAW" << std::endl;
}

// TODO: do this in a different thread!
Fl_Image* Fltk13Preview::loadImg(std::string fileName) {
	if (fileName.empty()) return NULL;
std::cout << "loading " << fileName << std::endl;

	Fl_Image* img = NULL;
	Fl_JPEG_Image imgl(fileName.c_str());
std::cout << "  w x h " << imgl.w() << " " << imgl.h() << std::endl;
	if (imgl.w()) {
		float scale = std::min(w() / (float) imgl.w(), h() / (float) imgl.h());
		if (img) delete img; img = NULL;
		img = imgl.copy((int) (scale * imgl.w()), (int) (scale * imgl.h()));
	}
	return img;
}
