#include "convolve.h"

void fuzzyBloom(	Magick::Image Defocussed_002,
					Magick::Image SharpForBloom,
					Magick::Blob& defocusBlob,
					std::string polyStr,
					std::stringstream& buffered, int& memory, int debug, double& Radii)
{
	std::string kernel_motion;
	if (memory == 1) {
		kernel_motion = buffered.str();
	}
	else {
		std::ifstream textOpen;
		textOpen.open(polyStr);
		std::string str((std::istreambuf_iterator<char>(textOpen)),
			std::istreambuf_iterator<char>());
		kernel_motion = str;
		textOpen.close();
	}

	Magick::Image bloom;
	bloom = SharpForBloom;
	bloom.alpha( false );
	bloom.alpha( true );

	// convert threshold in to single pixels for convolution?? //
	bloom.threshold(QuantumRange / 1.32);
	bloom.morphology(Magick::HitAndMissMorphology, "peaks", 2);
	bloom.morphology(Magick::DilateMorphology, "Disk");
	bloom.gaussianBlur(0, 0.64);
	//bloom.write("output/peaks1.png");

	bloom.artifact("convolve:scale", "\!");
	bloom.morphology(Magick::ConvolveMorphology, kernel_motion);
	bloom.artifact("convolve:scale", "1");

	bloom.sharpen(0, 1.22);
	
	//bloom.write("output/peaks2.png");


	//double raVal = 0;
	double doValMin = 32;
	double dnValMin = 0.23;

	double nVal = (dnValMin / Radii) * doValMin;

	//for (int i = 0; i < 20; i++) {
	//	int incr = raVal++;
	//	double nVal2 = (dnValMin / static_cast<double>(incr)) * doValMin;
	//	std::cout << std::to_string(incr) << ": " << std::to_string(nVal2) << "\n";
	//}

	std::cout << "Level: " << std::to_string(nVal) << "\n";

	bloom.level(0, QuantumRange*nVal, 1.0);
	//bloom.write("output/peaks3.png");

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
					 std::string size, std::string polyStr, 
					 std::stringstream& buffered, int& memory, int debug )
{
	try {
		Magick::EnableOpenCL();
		std::string kernel_motion;
		if ( memory == 1 ) {
			kernel_motion = buffered.str();
		}
		else {
			std::ifstream textOpen;
			textOpen.open( polyStr );
			std::string str( ( std::istreambuf_iterator<char>( textOpen ) ),
							 std::istreambuf_iterator<char>() );
			kernel_motion = str;
			textOpen.close();
		}

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
		Magick::ErrorType::exception;
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