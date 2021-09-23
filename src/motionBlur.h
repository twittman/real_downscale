#pragma once
#include "main.h"


void mBlurr( std::vector<Magick::Coordinate> vertices, int& maxlength, int& debug );
void motion_blur_kernel( int& maxlength, int& vertices, int& debug );
