// include GLee.h before openglean

/* example code ****************

GLuint myShader;

// init
myShader = loadShader("v.txt", "f.txt");

// using shader
glUseProgram(myShader);
...
glUseProgram(0);

*******************************/


#ifndef SHADER_HEADER_INCLUDED
#define SHADER_HEADER_INCLUDED

#include "../tools/GLee.h"
#include <GL/GL.h>

GLuint loadShader(char* vertexShaderFileName, char* fragmentShaderFileName);

// for convenience: set uniform variables
void setUniform1i(GLuint shaderID, char* name, GLint v);
void setUniform1f(GLuint shaderID, char* name, GLfloat v);
void setUniform1fv(GLuint shaderID, char* name, GLfloat* v, int num=1);
void setUniform3fv(GLuint shaderID, char* name, GLfloat v[3]);
void setUniformMatrix4fv(GLuint shaderID, char* name, GLfloat v[16]);

#endif // SHADER_HEADER_INCLUDED
