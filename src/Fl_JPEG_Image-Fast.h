//
// JPEG image header file for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

/* \file
   Fl_JPEG_Image class . ML modified to use jpeg-turbo */

#ifndef Fl_JPEG_Image_Fast_H
#define Fl_JPEG_Image_Fast_H
#  include <FL/Fl_Image.H>

   /**
	The Fl_JPEG_Image class supports loading, caching,
	and drawing of Joint Photographic Experts Group (JPEG) File
	Interchange Format (JFIF) images. The class supports grayscale
	and color (RGB) JPEG image files.
	*/
class FL_EXPORT Fl_JPEG_ImageFast : public Fl_RGB_Image {

public:
	// minDim: minimal size of max dimension, enables (faster) decoding of lower resolution, -1 disables
	Fl_JPEG_ImageFast(const char* filename, int minDim = -1);
	Fl_JPEG_ImageFast(const char* name, const unsigned char* data, int minDim = -1);

protected:

	//void load_jpg_(const char* filename, const char* sharename, const unsigned char* data);

};

#endif
