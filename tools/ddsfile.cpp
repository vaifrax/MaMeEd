// basic read support for DDS files
//
// by Marcel Lancelle

//#include <stdint.h>
#include "ddsfile.h"
#include <fstream>
#include <string.h>

// type defines
#ifdef _MSC_VER
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int32 uint64_t;
#else
#include <stdint.h>
#endif

const unsigned int FOURCC_DXT1 = uint32_t('D') + (uint32_t('X') << 8) + (uint32_t('T') << 16) + (uint32_t('1') << 24);
const unsigned int FOURCC_DXT3 = uint32_t('D') + (uint32_t('X') << 8) + (uint32_t('T') << 16) + (uint32_t('3') << 24);
const unsigned int FOURCC_DXT5 = uint32_t('D') + (uint32_t('X') << 8) + (uint32_t('T') << 16) + (uint32_t('5') << 24);


// read image (data points to compressed data, may include mipmaps and cube
// faces)
// The necessary memory will be allocated with new, call delete[] when you
// don't need it any more!

struct DDSHeader {
	struct pixel_format {
		uint32_t Size;
		uint32_t Flags;
		uint32_t FourCC;
		uint32_t BitsPerPixel;
		uint32_t RMask;
		uint32_t GMask;
		uint32_t BMask;
		uint32_t AMask;
	};

	struct capabilities {
		uint32_t Caps1;
		uint32_t Caps2;
		uint32_t Reserved[2];
	};

	uint8_t Signature[4];
	uint32_t Size;
	uint32_t Flags;
	uint32_t Height;
	uint32_t Width;
	uint32_t PitchOrLinearSize;
	uint32_t Depth;
	uint32_t MipmapCount;
	uint32_t Reserved[11];
	pixel_format Format;
	capabilities Caps;
	uint32_t Reserved2;
};

#define DDSCAPS2_CUBEMAP		0x00000200

MLImage* loadDDS(std::string fileName, int* numberOfMipmaps/*=NULL*/, bool* isCubemap/*=NULL*/) {

	// code adopted from NVidia 'ARB_texture_compression.pdf'
	// and from Tanguy Fautré

	std::ifstream is;

	is.open(fileName.c_str(), std::ios::in | std::ios::binary);
	if (!is.is_open()) return NULL; // file couldn't be opened

	DDSHeader header;
	if (!is.read((char*) &header, sizeof(header))) {
		is.close();
		return NULL;
	}

	// verify file type
	if (strncmp((const char*) header.Signature, "DDS ", 4) != 0) {
		is.close();
		return NULL;
	}

	MLImage* img = new MLImage();

	img->width = header.Width;
	img->height = header.Height;
	img->colorComponents = header.Format.BitsPerPixel / 8; // TODO: compressed formats? 16 bit formats???
	img->bytesPerPixel = header.Format.BitsPerPixel / 8;

	if (numberOfMipmaps) *numberOfMipmaps = header.MipmapCount;
	int blockSize = 16;

	switch (header.Format.FourCC) {
		case 0:
			img->openGLFormat = _GL_RGB;
			if (header.Format.BMask == 0XFF) img->openGLFormat = _GL_BGR_EXT;
			if (img->colorComponents == 4) {
				img->openGLFormat = _GL_RGBA;
				if (header.Format.BMask = 0xFF) img->openGLFormat = _GL_BGRA_EXT;
			}
			if (img->colorComponents == 1) img->openGLFormat = _GL_LUMINANCE;
			break;
		case FOURCC_DXT1:
			img->openGLFormat = _GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case FOURCC_DXT3:
			img->openGLFormat = _GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			img->openGLFormat = _GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			delete[] img->data;
			delete img;
			return NULL;
	}

	// how big is it going to be including all mipmaps?
	img->dataSize = (header.MipmapCount > 1) ? header.PitchOrLinearSize * 2 : header.PitchOrLinearSize;
	if (header.Format.FourCC == 0) {
		img->dataSize *= header.Height; // uncompressed format: only one scan line -> multiply with height
	}
	if (img->dataSize == 0) { // some files have that - compressed files always???
		int width = img->width; 
		int height = img->height;

		for (int i = 0; i <= header.MipmapCount && (width || height); ++i) { // mipmap loop
			if (width == 0) width = 1;
			if (height == 0) height = 1;
			int imgSize = ((width+3)/4)*((height+3)/4)*blockSize;
			if (!header.Format.FourCC) imgSize = width*height*img->bytesPerPixel; // not compressed
			img->dataSize += imgSize;
			width >>= 1;
			height >>= 1;
		}
	}

	bool _isCubemap = (bool) (header.Caps.Caps2 & DDSCAPS2_CUBEMAP);
	if (isCubemap) *isCubemap = _isCubemap;
	if (_isCubemap) img->dataSize *= 6; // TODO: only support cubemaps with 6 faces; throw error otherwise

	img->data = new unsigned char[img->dataSize];

	// read image data
	if (!is.read((char*) img->data, img->dataSize)) {
		delete[] img->data;
		delete img;
		is.close();
		return NULL;
	}

	// close the file
	is.close();

	return img;
}
