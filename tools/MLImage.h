#ifndef MLIMAGE_HEADER_INCLUDED
#define MLIMAGE_HEADER_INCLUDED

struct MLImage {
	int width, height;
	int bytesPerPixel;
	int colorComponents; // number of channels, e.g. 4 for RGBA
	unsigned int openGLFormat; // for OpenGL texturing

	unsigned char* data; // raw data, allocate with new, deallocate with delete[]
	int dataSize; // should include complete storage space, including mipmaps, several cube faces etc.
};


// avoid gl and glee headers by defining new values here
#define _GL_COLOR_INDEX                    0x1900
#define _GL_STENCIL_INDEX                  0x1901
#define _GL_DEPTH_COMPONENT                0x1902
#define _GL_RED                            0x1903
#define _GL_GREEN                          0x1904
#define _GL_BLUE                           0x1905
#define _GL_ALPHA                          0x1906
#define _GL_RGB                            0x1907
#define _GL_RGBA                           0x1908
#define _GL_LUMINANCE                      0x1909
#define _GL_LUMINANCE_ALPHA                0x190A

#define _GL_BGR_EXT                        0x80E0
#define _GL_BGRA_EXT                       0x80E1

#define _GL_COMPRESSED_ALPHA               0x84E9
#define _GL_COMPRESSED_LUMINANCE           0x84EA
#define _GL_COMPRESSED_LUMINANCE_ALPHA     0x84EB
#define _GL_COMPRESSED_INTENSITY           0x84EC
#define _GL_COMPRESSED_RGB                 0x84ED
#define _GL_COMPRESSED_RGBA                0x84EE

#define _GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define _GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define _GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define _GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3

#endif // MLIMAGE_HEADER_INCLUDED
