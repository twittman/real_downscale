#include "convolve.h"

void fuzzyBloom( Magick::Image Defocussed_002,
				 Magick::Blob& defocusBlob,
				 double& diameter )
{
	std::random_device rd;
	std::mt19937 generator( rd() );
	std::uniform_int_distribution r01( 0, 32 );
	int intensityHighChance = r01( generator );

	double intensity;
	if ( intensityHighChance == 6 || intensityHighChance == 12 ) {
		intensity = diameter / 18;
	}
	else {
		intensity = diameter / 30;
	}

	Magick::Image bloom;
	bloom = Defocussed_002;
	bloom.threshold( QuantumRange / 2.40 );
	bloom.blur( 0, diameter );
	bloom.alpha( false );
	bloom.alpha( true );
	bloom.evaluate( Magick::AlphaChannel, Magick::MultiplyEvaluateOperator, intensity );
	Defocussed_002.composite( bloom, 0, 0, Magick::ScreenCompositeOp );
	Defocussed_002.depth( 8 );
	Defocussed_002.colorSpace( Magick::sRGBColorspace );
	Defocussed_002.magick( "PNG" );
	Defocussed_002.write( &defocusBlob );
}
void convolve( Magick::Image& Defocussed_002,
					 Magick::Blob& defocusBlob, 
					 std::string output, 
					 size_t Width, size_t Height, 
					 std::string size, std::string polyStr, int debug )
{
	try {
		Magick::EnableOpenCL();
		std::ifstream textOpen;
		textOpen.open( polyStr );
		std::string str( ( std::istreambuf_iterator<char>( textOpen ) ),
						 std::istreambuf_iterator<char>() );
		std::string kernel_motion = str;
		textOpen.close();
		if ( debug == 1 ) {
			std::cout << "\nKernel: " << kernel_motion << "\n";
		}

		// convolve with defocus blur kernel
		Defocussed_002.read( defocusBlob );
		makeitgrain( Defocussed_002, defocusBlob, Width, Height, size );
		Defocussed_002.artifact( "convolve:scale", "\!" );
		Defocussed_002.morphology( Magick::ConvolveMorphology, kernel_motion );
		Defocussed_002.depth( 8 );
		Defocussed_002.colorSpace( Magick::sRGBColorspace );
		Defocussed_002.magick( "PNG" );
		Defocussed_002.write( &defocusBlob );
		// end convolve with defocus blur kernel

	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, first convolution: " << error_.what() << std::endl;
	}
}
void gaussianBlur( Magick::Image& Defocussed_002,
				   Magick::Blob& defocusBlob,
				   std::string output,
				   size_t Width, size_t Height,
				   std::string size, double& gaussianRange, int debug )
{
	try {

		Defocussed_002.read( defocusBlob );
		makeitgrain( Defocussed_002, defocusBlob, Width, Height, size );
		Defocussed_002.gaussianBlur( 0, gaussianRange );
		Defocussed_002.depth( 8 );
		Defocussed_002.colorSpace( Magick::sRGBColorspace );
		Defocussed_002.magick( "PNG" );
		Defocussed_002.write( &defocusBlob );

	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, first convolution: " << error_.what() << std::endl;
	}
}