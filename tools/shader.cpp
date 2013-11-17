#include "shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

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
