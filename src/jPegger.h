#pragma once
#include <Magick++.h>
#include <random>
#include <algorithm>
#include <iostream>
#include "uni_grain.h"

void jPegger( Magick::Image& input, int& minJ, int& maxJ );
void jPeggMulti( Magick::Image& input, int& minJ, int& maxJ );