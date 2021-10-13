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
	std::random_device rd;
	std::mt19937 generator( rd() );
	std::uniform_int_distribution r01( minJ, maxJ );
	int rNum = r01( generator );

	int qJpeg = rNum;
	input.defineValue( "JPEG", "sampling-factor", samplesYo() );
	input.quality( qJpeg );
	input.magick( "JPEG" );

}

void jPeggMulti( Magick::Image& input, int& minJ, int& maxJ )
{
	Magick::Blob jpegBlob;
	input.magick( "JPEG" );
	//input.write( &jpegBlob );
	// generate number of times to re-compress
	std::random_device rd;
	std::mt19937 generator( rd() );
	std::uniform_int_distribution r01( 4, 64 );
	int rNum = r01( generator );
	int compressionTimes = rNum;
	for ( int i = 1; i < compressionTimes; i++ ) {
		// generate separate X and Y roll values

		std::uniform_int_distribution<int> specialSequence01( -21, 21 );
		int xRollAmt = specialSequence01( rd );
		std::uniform_int_distribution<int> specialSequence02( -21, 21 );
		int yRollAmt = specialSequence02( rd );

		int xRoll;
		int xRollNeg;
		// X translate
		if ( xRollAmt > 0 ) {
			xRoll = xRollAmt;
			xRollNeg = -abs( xRollAmt );
		}
		else if ( xRollAmt < 0 ) {
			xRoll = xRollAmt;
			xRollNeg = abs( xRollAmt );
		}
		else if ( xRollAmt == 0 ) {
			xRoll = xRollAmt;
			xRollNeg = xRollAmt;
		}
		int yRoll;
		int yRollNeg;
		// Y translate
		if ( yRollAmt > 0 ) {
			yRoll = yRollAmt;
			yRollNeg = -abs( yRollAmt );
		}
		else if ( yRollAmt < 0 ) {
			yRoll = yRollAmt;
			yRollNeg = abs( yRollAmt );
		}
		else if ( yRollAmt == 0 ) {
			yRoll = yRollAmt;
			yRollNeg = yRollAmt;
		}

		// roll forwards
		input.filterType( Magick::PointFilter );
		input.roll( xRoll, yRoll );
		// compression
		std::uniform_int_distribution r01( minJ, maxJ );
		int rNum = r01( generator );
		int qJpeg = rNum;
		input.defineValue( "JPEG", "sampling-factor", samplesYo() );
		input.quality( qJpeg );
		input.magick( "JPEG" );
		// roll backwards
		input.filterType( Magick::PointFilter );
		input.roll( xRollNeg, yRollNeg );
		//std::cout << "Jpeg: " << qJpeg << "\n";
		//std::cout << "X Translation: " << xRoll << "\n";

	}
}