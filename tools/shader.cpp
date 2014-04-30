#include "shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

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
#ifndef CHECK_GL_STATE
#define CHECK_GL_STATE {int tmperr = glGetError(); if (tmperr != GL_NO_ERROR) {assert(0);}}
#endif // CHECK_GL_STATE

void printInfoLog(GLhandleARB obj) {
	int infologLength = 0;
	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

	if (infologLength > 0) {
		int charsWritten  = 0;
		char* infoLog = new char[infologLength];
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		delete[] infoLog;
	}
}

// data just stays in memory, delete[] it if you don't need it any more
const char* loadFile2Mem(char* fileName) {
	FILE* file = fopen(fileName, "rb");
	if (!file) {
		fprintf(stderr, "Unable to open file %s", fileName);
		exit(1);
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	unsigned long fileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	char* buffer= new char[fileLen+1];

	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);
	buffer[fileLen] = 0; // string terminator

	return buffer;
}

GLuint loadShader(char* vertexShaderFileName, char* fragmentShaderFileName) {
	GLuint shaderProgram;
	std::cout << "loading vertex shader: " << vertexShaderFileName << std::endl;
	GLhandleARB vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB); // glCreateShader
	const char* vertexShaderSource = loadFile2Mem(vertexShaderFileName);
	glShaderSourceARB(vertexShader, 1, &vertexShaderSource, NULL); // glShaderSource
	glCompileShaderARB(vertexShader); // glCompileShader
	printInfoLog(vertexShader);
	std::cout << "   ... done" << std::endl;

	std::cout << "loading fragment shader: " << fragmentShaderFileName << std::endl;
	GLhandleARB fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	const char* fragmentShaderSource = loadFile2Mem(fragmentShaderFileName);
	glShaderSourceARB(fragmentShader, 1, &fragmentShaderSource, NULL); // glShaderSource
	glCompileShaderARB(fragmentShader); // glCompileShader
	printInfoLog(fragmentShader);
	std::cout << "   ... done" << std::endl;

	std::cout << "linking shader program..." << std::endl;
	shaderProgram = glCreateProgramObjectARB(); // glCreateProgram
	glAttachObjectARB(shaderProgram, vertexShader); // glAttachShader
	glAttachObjectARB(shaderProgram, fragmentShader); // glAttachShader
	glLinkProgramARB(shaderProgram);
	printInfoLog(shaderProgram);
	std::cout << "   ... done" << std::endl;

	glLinkProgramARB(shaderProgram); // glLinkProgram

	return shaderProgram;
}

////////////////////////////////////////////////////////////////////////////////
// set uniform variables

// only print first few warnings
int shaderWarningsPrintedNum = 0;
const int SHADER_WARINGS_NUM = 10;
void printShaderWarning(char* varName) {
	if (shaderWarningsPrintedNum < SHADER_WARINGS_NUM) {
		std::cout << "Warning: shader variable >" << varName << "< not found." << std::endl;
		shaderWarningsPrintedNum++;
	}
}

void setUniform1i(GLuint shaderID, char* name, GLint v) {
	CHECK_GL_STATE
	GLint loc = glGetUniformLocationARB(shaderID, name);
	CHECK_GL_STATE
	if (loc >= 0) glUniform1iARB(loc, v);
	else printShaderWarning(name);
}

void setUniform1f(GLuint shaderID, char* name, GLfloat v) {
	CHECK_GL_STATE
	GLint loc = glGetUniformLocationARB(shaderID, name);
	CHECK_GL_STATE
	if (loc >= 0) glUniform1fARB(loc, v);
	else printShaderWarning(name);
}

void setUniform1fv(GLuint shaderID, char* name, GLfloat* v, int num/*=1*/) {
	CHECK_GL_STATE
	GLint loc = glGetUniformLocationARB(shaderID, name);
	CHECK_GL_STATE
	if (loc >= 0) glUniform1fvARB(loc, num, v);
	else printShaderWarning(name);
	CHECK_GL_STATE
}

void setUniform3fv(GLuint shaderID, char* name, GLfloat v[3]) {
	CHECK_GL_STATE
	GLint loc = glGetUniformLocationARB(shaderID, name);
	CHECK_GL_STATE
	if (loc >= 0) glUniform3fvARB(loc, 1, v);
	else printShaderWarning(name);
}

void setUniformMatrix4fv(GLuint shaderID, char* name, GLfloat v[16]) {
	CHECK_GL_STATE
	GLint loc = glGetUniformLocationARB(shaderID, name);
	CHECK_GL_STATE
	if (loc >= 0) glUniformMatrix4fvARB(loc, 1, false, v);
	else printShaderWarning(name);
}
