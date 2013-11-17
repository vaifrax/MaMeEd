// basic read support for DDS files
//
// by Marcel Lancelle

#ifndef DDSFILE_HEADER_INCLUDED
#define DDSFILE_HEADER_INCLUDED

#include <iostream>
#include "MLImage.h"

///////////////////////////////////////////////////////////////////////////////

//	// sample code to read an image
//
//	MLImage* img = loadDDS(fileName);
//	if (img) {
//		std::cout << "read image successfully" << std::endl;
//
//		...
//
//		delete[] img->data;
//		img->data = NULL;
//	} else {
//		std::cerr << "Error reading raw data from PGM file " << fileName
//				<< std::endl;
//	}

// read image (data points to compressed data, may include mipmaps and cube
// faces)
// The necessary memory will be allocated with new, call delete[] when you
// don't need it any more!
MLImage* loadDDS(std::string fileName, int* numberOfMipmaps=NULL, bool* isCubemap=NULL);

///////////////////////////////////////////////////////////////////////////////

// save image
//bool saveDDS(std::string fileName, MLImage* img);

///////////////////////////////////////////////////////////////////////////////

#endif // DDSFILE_HEADER_INCLUDED
