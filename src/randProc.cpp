#include "randProc.h"

void random_contrast( Magick::Image& input, double& radius, double& strength )
{
	input.localContrast( radius, strength );
}