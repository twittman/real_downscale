#pragma once
#include "main.h"


void defocussBlurr( Magick::Blob polyBlob, Magick::Blob polyPGM,
					int radius,
					std::vector<Magick::Coordinate> vertices,
					int diameter, int distAmnt_01, int distAmnt_02,
					int& debug );

void polyVertices( Magick::Blob polyBlob, Magick::Blob polyPGM,
				   int radius, int num,
				   double rad1, double rad2, double offset,
				   double diameter,
				   int debug );