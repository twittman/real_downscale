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


void convolve( Magick::Image& Defocussed,
			   Magick::Blob& defocusBlob,
			   std::string output,
			   size_t Width, size_t Height,
			   std::string size, std::string polyStr, int debug );