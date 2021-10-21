#include <Magick++.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <random>
#include <stdio.h>
#include <fstream>
#include <streambuf>

void read_PGM_as_string_mB( const char* filename, std::list<std::string>& lines, int& width, int& height );
void mBlurr( std::vector<Magick::Coordinate> vertices, int& maxlength, int& debug );
void motion_blur_kernel( int& maxlength, int& vertices, std::string& fileNoPathNoEXT, int& debug );