#pragma once
#include <Magick++.h>
#include <random>

void makeitgrain( Magick::Image& Defocussed_002,
				  Magick::Blob& defocusBlob,
				  size_t Width, size_t Height,
				  std::string size );