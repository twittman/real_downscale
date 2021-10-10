#include "jPegger.h"

std::string samplesYo()
{
	const std::string samplesList[6] = { "4:1:0",
									"4:1:1",
									"4:2:0",
									"4:2:2",
									"4:4:0",
									"4:4:4" };
	return samplesList[rand() % 6];
}

void jPegger( Magick::Image& input, int& minJ, int& maxJ )
{
	std::random_device ranDev;
	std::mt19937 rng1( rand() );
	// Random JPEG quality
	std::uniform_int_distribution<int> seq2( minJ, maxJ );
	int qJpeg = seq2( rng1 );
	input.defineValue( "JPEG", "sampling-factor", samplesYo() );
	input.quality( qJpeg );
	input.magick( "JPEG" );

}