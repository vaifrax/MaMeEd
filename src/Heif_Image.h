
/* by ML */

#ifndef HEIF_IMAGE_H
#define HEIF_IMAGE_H

#include <string>
#  include <FL/Fl_Image.H>

   /**
	
	*/
class FL_EXPORT Heif_Image : public Fl_RGB_Image {

public:
	// minDim: minimal size of max dimension, enables (faster) decoding of lower resolution, -1 disables
	Heif_Image(const char* filename);
	Heif_Image(const char* name, const unsigned char* data);

    static std::string iso_8859_1_to_utf8(std::string &str);

protected:


};

#endif
