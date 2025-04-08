#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H
#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


float* goldAmbient();
float* goldDiffuse();
float* goldSpecular();
float  goldShininess();

float* silverAmbient();
float* silverDiffuse();
float* silverSpecular();
float  silverShininess();

float* bronzeAmbient();
float* bronzeDiffuse();
float* bronzeSpecular();
float  bronzeShininess();


void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool CheckOpenGLError();
std::string readShaderSource(const char* filePath);
GLuint createShaderProgram();
GLuint loadTexture(const char* texImagePath);

#endif 