#!/bin/sh

gcc utils.cpp mesh.cpp shader.cpp main.cpp -g -std=c++11 -lstdc++ -lm -lGL -lGLU -lGLEW -lglfw -lpng -o $1
