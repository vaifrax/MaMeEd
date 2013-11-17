// helper class for most basic operations

#include "GLee.h"
#include "MLOpenGLTex.h"

#include <assert.h>

//GL_INVALID_ENUM                   0x0500
//GL_INVALID_VALUE                  0x0501
//GL_INVALID_OPERATION              0x0502
//GL_STACK_OVERFLOW                 0x0503
//GL_STACK_UNDERFLOW                0x0504
//GL_OUT_OF_MEMORY                  0x0505
//GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
//#define CHECK_GL_STATE {int tmperr = glGetError(); if (tmperr != GL_NO_ERROR) {std::cout << "GL error: " << tmperr; std::cout.unsetf(std::ios::dec); std::cout.setf(std::ios::hex); std::cout << " (" << tmperr << " hex)" << std::endl; assert(0); exit(1);}}

// short version, stripped cout output
#define CHECK_GL_STATE {int tmperr = glGetError(); if (tmperr != GL_NO_ERROR) {assert(0);}}


// source format is typically GL_RGB or GL_RGBA
// target format is typically GL_COMPRESSED_RGBA or GL_COMPRESSED_RGB

/*
	useful subsequent calls:

	// generate and use mipmaps
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, format, textureWidth, textureHeight, format, GL_UNSIGNED_BYTE, img->data());

	// clamping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
*/

MLOpenGLTex::MLOpenGLTex(MLImage* img, int numberOfSourceMipmaps/* = 0*/, bool isCubemap/* = false*/) {
	this->isCubemap = isCubemap;
	this->width = img->width;
	this->height = img->height;

	glGenTextures(1, &id);
	CHECK_GL_STATE

	bind();

	if (isCubemap) {
		GLint minFilter = (numberOfSourceMipmaps > 0) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		//glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // test?
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (numberOfSourceMipmaps) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	int blockSize = (img->openGLFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

	bool compressed = (img->openGLFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)
					|| (img->openGLFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
					|| (img->openGLFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);

	GLenum target = GL_TEXTURE_2D;

	GLenum internalFormat = GL_RGB; // target format on graphics board
	switch (img->colorComponents) {
		case 1:
			internalFormat = GL_LUMINANCE;
			break;
		case 4:
			internalFormat = GL_RGBA;
			break;
	}


	int offset = 0;

	int numberOfImages = (isCubemap) ? 6 : 1;

	for (int c=0; c<numberOfImages; c++) {
		if (isCubemap) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + c;
		int width = img->width;
		int height = img->height;

		for (int i = 0; i <= numberOfSourceMipmaps && (width || height); ++i) {
			if (width == 0) width = 1;
			if (height == 0) height = 1;

			int size;

			if (compressed) {
				size = ((width+3)/4)*((height+3)/4)*blockSize;
				glCompressedTexImage2DARB(target, i, img->openGLFormat, width, height, 0, size, img->data + offset);
				CHECK_GL_STATE
			} else {
				size = width*height * img->bytesPerPixel; // TODO: test!!! / verify / correct
				glTexImage2D(target, i, internalFormat, width, height, 0, img->openGLFormat, GL_UNSIGNED_BYTE, img->data + offset);
				CHECK_GL_STATE
			}

			offset += size;
			width >>= 1;
			height >>= 1;
		}
	}
}

MLOpenGLTex::~MLOpenGLTex() {
	glDeleteTextures(1, &id);
}

void MLOpenGLTex::bind() {
	if (isCubemap) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	} else {
		glBindTexture(GL_TEXTURE_2D, id);
	}

	CHECK_GL_STATE
};
