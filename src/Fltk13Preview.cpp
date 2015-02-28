#include "Fltk13Preview.h"

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_JPEG_Image.H> // for large preview img

Fltk13Preview::Fltk13Preview(int X, int Y, int W, int H) : Fl_Box(X,Y,W,H,0) {
	img = NULL;
}

void Fltk13Preview::setImg(std::string fileName) {
	Fl_JPEG_Image imgl(fileName.c_str());
	if (imgl.w()) {
		float scale = std::min(w() / (float) imgl.w(), h() / (float) imgl.h());
		if (img) delete img; img = NULL;
		img = imgl.copy((int) (scale * imgl.w()), (int) (scale * imgl.h()));

		if (img && (img->w() > 0)) {
			image(img);
			redraw();
		}
	}
}
