#include "Fltk13Preview.h"

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
//#include <FL/Fl_JPEG_Image.H> // for large preview img
#include "Fl_JPEG_Image-Fast.h"
#include "Heif_Image.h"
#include "FlImgTools.h"

#include <iostream> //for cout only

Fltk13Preview::Fltk13Preview(int X, int Y, int W, int H) : Fl_Box(X,Y,W,H,0) {
	img = NULL;
	for (int i=0; i<3; i++) {
		cache[i].fileName = std::string();
		cache[i].img = NULL;
	}

	// set a style that fills background, so that when setting a new image with a different size everything looks ok
	box(FL_FLAT_BOX);
	color(50);
}

void Fltk13Preview::printCache(ImgCacheStruct* cache) {
	std::cout << "cache [" << cache[0].fileName << "_" << cache[1].fileName << "_" << cache[2].fileName << "]" << std::endl;
}

void Fltk13Preview::setImg(std::string fileName, int imgExifStorageOrientation,
				std::string prevFileName, int prevImgExifStorageOrientation,
				std::string nextFileName, int nextImgExifStorageOrientation) {

	Fl_Image* prevImg = NULL;
	Fl_Image* nextImg = NULL;
	img = NULL;
printCache(cache);

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
		if (!used) {
			try {
				std::cout << "deleting cache " << i << std::endl;
				if (cache[i].img) delete cache[i].img;
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
	Fl::check(); // this is necessary to execute drawing now before loading the other images below

	// load missing images
	if (!prevImg) prevImg = loadImg(prevFileName, nextImgExifStorageOrientation);
	if (!prevImg || !prevImg->data()) prevFileName = std::string();
	if (!nextImg) nextImg = loadImg(nextFileName, nextImgExifStorageOrientation);
	if (!nextImg || !nextImg->data()) nextFileName = std::string();

	cache[0].fileName = prevFileName;
	cache[0].img = prevImg;
	cache[1].fileName = fileName;
	cache[1].img = img;
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

	Fl_Image* img = NULL;
	//Fl_JPEG_ImageFast imgl(fileName.c_str(), 800);
	Fl_Image* imgl = NULL;

	bool applyRotation = false;
	if ((strcmp(ext, ".JPG") == 0) || (strcmp(ext, "JPEG") == 0)) {
		// load jpeg image
		imgl = new Fl_JPEG_ImageFast(fileName.c_str(), 800);
		applyRotation = true; // image is still in sensor orientation and may need rotation
	} else if (strcmp(ext, "HEIC") == 0) {
		// load heif image
		imgl = new Heif_Image(fileName.c_str());
		applyRotation = false; // libheif already rotates images on its own
	} else {
		return nullptr;
	}
	if (imgl == nullptr) return nullptr;

	std::cout << " loading " << fileName;
	int imglWr = imgl->w();
	int imglHr = imgl->h();

	if ((imglWr == 0) || (imglHr == 0)) {
		std::cerr << "  Error" << std::endl;
		return nullptr;
	}

	int rotateClockwDeg = 0;
	if (applyRotation) {
		// check rotation
		switch (exifStorageOrientation) {
			case 1:
				// do nothing
				break;
			case 3:
				rotateClockwDeg = 180;
				break;
			case 6:
				rotateClockwDeg = 90;
				std::swap(imglWr, imglHr);
				break;
			case 8:
				rotateClockwDeg = 270;
				std::swap(imglWr, imglHr);
				break;
			default:
				// there are mirrored versions, too, they are not yet implemented here
				std::cerr << "exifStorageOrientation " << exifStorageOrientation << " not supported" << std::endl;
				break;
		}
	}

	std::cout << "  w x h " << imglWr << " " << imglHr << std::endl;

	// resize first (faster than rotating first)
	float scale = std::min(w() / (float) imglWr, h() / (float) imglHr);
	//if (img) delete img; img = NULL;
	img = imgl->copy((int) (scale * imgl->w()), (int) (scale * imgl->h()));

	// rotate if necessary
	if ((applyRotation) && (rotateClockwDeg != 0)) {
		Fl_Image* tmpImg = FlImgTools::rotate(img, rotateClockwDeg);
		delete img;
		img = tmpImg;
	}

	delete imgl;
	return img;
}
