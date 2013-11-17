// helper class for most basic operations

#ifndef MLOPENGLTEX_HEADER_INCLUDED
#define MLOPENGLTEX_HEADER_INCLUDED

#include "GLee.h"
//#include <GL/openglean.h>
#include <FL/gl.h>
#include "MLImage.h"

class MLOpenGLTex {
  public:
	MLOpenGLTex(MLImage* img, int numberOfSourceMipmaps = 0, bool isCubemap = false);
	~MLOpenGLTex();

	void bind();

	int width, height;
	GLuint id;
	bool isCubemap;
};

#endif // MLOPENGLTEX_HEADER_INCLUDED
