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
	auto Width = input.columns();
	auto Height = input.rows();

	std::string crop = std::to_string( Width ) + "x" + std::to_string( Height );

	Magick::Blob jpegBlob;
	input.magick( "JPEG" );
	//input.write( &jpegBlob );
	// generate number of times to re-compress
	std::random_device rd;
	std::mt19937 generator( rd() );
	std::uniform_int_distribution r01( 8, 64 );
	int rNum = r01( generator );
	int compressionTimes = rNum;
	const int rolVal = 39;
	const int rotVal = 2;
	int times = 1;
	//std::cout << "\nChosen value: " << rNum << std::endl;
	for ( int i = 1; i < compressionTimes; i++ ) {
		// generate separate X and Y roll values

		std::uniform_int_distribution<int> specialSequence01( -rolVal, rolVal );
		int xRollAmt = specialSequence01( rd );
		std::uniform_int_distribution<int> specialSequence02( -rolVal, rolVal );
		int yRollAmt = specialSequence02( rd );

		std::uniform_int_distribution<int> specialSequence03( -rotVal, rotVal );
		int xRotAmt = specialSequence03( rd );
		std::uniform_int_distribution<int> specialSequence04( -rotVal, rotVal );
		int yRotAmt = specialSequence04( rd );

		int xRoll;
		int xRollNeg;
		int xRot;
		int xRotNeg;
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
		// X rotate
		if ( xRotAmt > 0 ) {
			xRot = xRotAmt;
			xRotNeg = -abs( xRotAmt );
		}
		else if ( xRotAmt < 0 ) {
			xRot = xRotAmt;
			xRotNeg = abs( xRotAmt );
		}
		else if ( xRotAmt == 0 ) {
			xRot = xRotAmt;
			xRotNeg = xRotAmt;
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
		//std::cout << "Rotating image: " << ++times << " times\t\r";

		// weird code for distortion
		// 
		// roll forwards
		// rotation
		//auto argNumForward = 2;
		//double* argListForward = new double[argNumForward];
		//argListForward[0] = 1.0; // scale
		//argListForward[1] = xRoll; // rotation
		//bool bestFit = Magick::MagickTrue;
		////input.virtualPixelMethod( Magick::TileVirtualPixelMethod );
		//input.distort( Magick::ScaleRotateTranslateDistortion, argNumForward, argListForward, bestFit );


		input.roll( xRoll, yRoll );
		input.filterType( Magick::PointFilter );
		//input.rotate( xRot );

		//std::string size_img = std::to_string( Width ) + "x" + std::to_string( Height );
		//uni_grain( input, Width, Height, size_img );

		// compression
		std::uniform_int_distribution r01( minJ, maxJ );
		int rNum = r01( generator );
		int qJpeg = rNum;
		input.defineValue( "JPEG", "sampling-factor", samplesYo() );
		input.quality( qJpeg );
		input.magick( "JPEG" );
		input.roll( xRollNeg, yRollNeg );

		// roll backwards
		//auto argNumBackward = 2;
		//double* argListBackward = new double[argNumBackward];
		//argListBackward[0] = 1.0; // scale
		//argListBackward[1] = xRollNeg; // rotation
		//bool bestFitBackward = Magick::MagickTrue;
		////input.virtualPixelMethod( Magick::MirrorVirtualPixelMethod );
		//input.filterType( Magick::PointFilter );
		//input.distort( Magick::ScaleRotateTranslateDistortion, argNumBackward, argListBackward, bestFitBackward );
		//input.rotate( xRotNeg );
		//input.trim();
		//std::cout << "Jpeg: " << qJpeg << "\n";
		//std::cout << "X Translation: " << xRoll << "\n";
	}
	//std::cout << std::endl;
}