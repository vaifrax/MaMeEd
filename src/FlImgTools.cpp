#include <algorithm> // for std::swap

#include "FlImgTools.h"


/*static*/ Fl_Image* FlImgTools::rotate(Fl_Image* img, int rotateClockwDeg) {
	int rWidth = img->w();
	int rHeight = img->h();

	unsigned char* data = new unsigned char[rWidth * rHeight * 3];

	unsigned char* dp = data;
	const char *buf = img->data()[0];

	if (rotateClockwDeg == 90) {
		std::swap(rWidth, rHeight);
		for (int x=0; x<img->w(); x++) {
			for (int y=img->h()-1; y>=0; y--) {
				*dp++ = *(buf + 3*(x+y*img->w()));
				*dp++ = *(buf + 3*(x+y*img->w())+1);
				*dp++ = *(buf + 3*(x+y*img->w())+2);
			}
		}
	}

	if (rotateClockwDeg == 270) {
		std::swap(rWidth, rHeight);
		for (int x=img->w()-1; x>=0; x--) {
			for (int y=0; y<img->h(); y++) {
				*dp++ = *(buf + 3*(x+y*img->w()));
				*dp++ = *(buf + 3*(x+y*img->w())+1);
				*dp++ = *(buf + 3*(x+y*img->w())+2);
			}
		}
	}

	if (rotateClockwDeg == 180) {
		for (int y=rHeight-1; y>=0; y--) {
			for (int x=rWidth-1; x>=0; x--) {
				*dp++ = *(buf + 3*(x+y*img->w()));
				*dp++ = *(buf + 3*(x+y*img->w())+1);
				*dp++ = *(buf + 3*(x+y*img->w())+2);
			}
		}
	}

	return new Fl_RGB_Image(data, rWidth, rHeight, 3);
}
