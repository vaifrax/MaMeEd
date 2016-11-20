#include "Fltk13Preview.h"

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
//#include <FL/Fl_JPEG_Image.H> // for large preview img
#include "Fl_JPEG_Image-Fast.h"
#include "FlImgTools.h"

#include <iostream> //for cout only

Fltk13Preview::Fltk13Preview(int X, int Y, int W, int H) : Fl_Box(X,Y,W,H,0) {
	img = NULL;
	for (int i=0; i<3; i++) {
		cache[i].fileName = std::string();
		cache[i].img = NULL;
	}
}

void Fltk13Preview::setImg(std::string fileName, int imgExifStorageOrientation,
				std::string prevFileName, int prevImgExifStorageOrientation,
				std::string nextFileName, int nextImgExifStorageOrientation) {

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
			try {
				delete cache[i].img;
			} catch (...) {
				std::cout << "DELETION FAILED of cached file " << cache[i].fileName << std::endl;
			}
			cache[i].img = NULL;
			cache[i].fileName = std::string();
		}
	}

	//std::cout << "START LOADING" << std::endl;

	// load missing images
	if (!img) img = loadImg(fileName, imgExifStorageOrientation);
	// refresh preview
	if (img && (img->w() > 0)) {
		image(img);
		redraw();
	}
	Fl::check();

	// load missing images
	if (!prevImg) prevImg = loadImg(prevFileName, nextImgExifStorageOrientation);
	if (!nextImg) nextImg = loadImg(nextFileName, nextImgExifStorageOrientation);

	cache[0].fileName = fileName;
	cache[0].img = img;
	cache[1].fileName = prevFileName;
	cache[1].img = prevImg;
	cache[2].fileName = nextFileName;
	cache[2].img = nextImg;

	//std::cout << "END LOADING" << std::endl;
}

// TODO: do this in a different thread!
Fl_Image* Fltk13Preview::loadImg(std::string fileName, int exifStorageOrientation) {
	if (fileName.empty()) return NULL;

	// check extension
	if (fileName.length() < 4) return NULL;
	char ext[5] = {0,0,0,0,0};
	for (int i=0; i<4; i++) ext[i] = toupper(fileName.at(fileName.length()-4+i));
	if ((strcmp(ext, ".JPG") != 0) && (strcmp(ext, "JPEG") != 0)) return NULL; // not correct extension

	// load image
	std::cout << " loading " << fileName;
	Fl_Image* img = NULL;
	Fl_JPEG_ImageFast imgl(fileName.c_str(), 800);
	int imglWr = imgl.w();
	int imglHr = imgl.h();

	// check rotation
	int rotateClockwDeg = 0;
	switch (exifStorageOrientation) {
		case 3:
			rotateClockwDeg = 180;
			break;
		case 5:
			rotateClockwDeg = 270;
			std::swap(imglWr, imglHr);
			break;
		case 6:
			rotateClockwDeg = 90;
			std::swap(imglWr, imglHr);
			break;
	}

	// resize first
	std::cout << "  w x h " << imglWr << " " << imglHr << std::endl;
	if (imgl.w()) {
		float scale = std::min(w() / (float) imglWr, h() / (float) imglHr);
		if (img) delete img; img = NULL;
		img = imgl.copy((int) (scale * imgl.w()), (int) (scale * imgl.h()));
	}

	// rotate if necessary
	// there are mirrored versions, too, they are not yet implemented here
	if (rotateClockwDeg != 0) {
		Fl_Image* tmpImg = FlImgTools::rotate(img, rotateClockwDeg);
		delete img;
		img = tmpImg;
	}

	return img;
}
