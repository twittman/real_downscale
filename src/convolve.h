#pragma once
#include "main.h"
void convolve( Magick::Image Defocussed,
			   Magick::Blob defocusBlob,
			   std::string& output,
			   auto& Width, auto& Height,
			   std::string& size );