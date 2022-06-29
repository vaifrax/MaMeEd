
/* by ML */

#ifndef HEIF_IMAGE_H
#define HEIF_IMAGE_H
#  include <FL/Fl_Image.H>

   /**
	
	*/
class FL_EXPORT Heif_Image : public Fl_RGB_Image {

public:
	// minDim: minimal size of max dimension, enables (faster) decoding of lower resolution, -1 disables
	Heif_Image(const char* filename);
	Heif_Image(const char* name, const unsigned char* data);

protected:


};

#endif
