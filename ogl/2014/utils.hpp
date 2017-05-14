#ifndef UTILS_HPP
#define UTILS_HPP

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::string;
using std::cerr;
using std::endl;

string OpenTextFile(const string filename);
string OpenBMPFile(const string filename, unsigned int& width, unsigned int& height);
string OpenPNGFile(const string filename, unsigned int& width, unsigned int& height);

void serrx(string message);
void errx(string message);

#endif
