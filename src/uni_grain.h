#pragma once
#include <Magick++.h>
#include <random>

void uni_grain( Magick::Image& input,
				  size_t Width, size_t Height,
				  std::string size );