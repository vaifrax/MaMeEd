// include GLee.h before openglean

#ifndef SHADER_HEADER_INCLUDED
#define SHADER_HEADER_INCLUDED

#include "GLee.h"
//#include <GL/openglean.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

GLuint loadShader(char* vertexShaderFileName, char* fragmentShaderFileName);

#endif // SHADER_HEADER_INCLUDED
