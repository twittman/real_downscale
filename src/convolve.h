#pragma once
#include <Magick++.h>
#include <random>
#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <string>
#include <iostream>
#include <filesystem>
#include "make_it_grain.h"

void fuzzyBloom( Magick::Image Defocussed_002,
				 Magick::Image SharpForBloom,
				 Magick::Blob& defocusBlob,
				 std::string polyStr,
				 std::stringstream& buffered, int& memory, int debug, double& Radii);

void convolve( Magick::Image& Defocussed,
			   Magick::Blob& defocusBlob,
			   std::string output,
			   size_t Width, size_t Height,
			   std::string size, std::string polyStr, 
			   std::stringstream& buffered, int& memory, int debug );

void gaussianBlur( Magick::Image& Defocussed_002,
				   Magick::Blob& defocusBlob,
				   std::string output,
				   size_t Width, size_t Height,
				   std::string size, double& gaussianRange, int debug );