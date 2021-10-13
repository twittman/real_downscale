#pragma once
#include <Magick++.h>
#include <random>
#include <iostream>

void jPegger( Magick::Image& input, int& minJ, int& maxJ );
void jPeggMulti( Magick::Image& input, int& minJ, int& maxJ );