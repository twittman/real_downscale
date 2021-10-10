#include "convolve.h"

void convolve( Magick::Image& Defocussed_002,
					 Magick::Blob& defocusBlob, 
					 std::string output, 
					 size_t Width, size_t Height, 
					 std::string size, std::string polyStr, int debug )
{
	try {
		std::ifstream textOpen;
		textOpen.open( polyStr );
		std::string str( ( std::istreambuf_iterator<char>( textOpen ) ),
						 std::istreambuf_iterator<char>() );
		std::string kernel_motion = str;
		textOpen.close();
		if ( debug == 1 ) {
			std::cout << "\nKernel: " << kernel_motion << "\n";
		}

		std::filesystem::remove( polyStr );
		std::filesystem::remove( "poly.txt" );



		// convolve with defocus blur kernel
		Defocussed_002.read( defocusBlob );
		makeitgrain( Defocussed_002, defocusBlob, Width, Height, size );
		Defocussed_002.artifact( "convolve:scale", "\!" );
		Defocussed_002.morphology( Magick::ConvolveMorphology, kernel_motion );
		Defocussed_002.depth( 8 );
		Defocussed_002.colorSpace( Magick::sRGBColorspace );
		//Defocussed_002.crop( Magick::Geometry( Width, Height, 64, 64 ) );
		//Defocussed_002.filterType( Magick::PointFilter );
		//Defocussed_002.resize( "25%" );
		Defocussed_002.magick( "PNG" );
		Defocussed_002.write( &defocusBlob );
		// end convolve with defocus blur kernel

		// check for txt files and remove all
		std::vector<std::string> filesForRemove = { "poly.txt",
													"poly.pgm",
													"poly_new.txt", 
													"mBpoly.txt", 
													"mBpoly.pgm",
													"mBpoly_new.txt" };
		for ( auto&& files : filesForRemove ) {
			if ( std::filesystem::exists( files ))
			{
				std::filesystem::remove( files );
			} 
		}

	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, first convolution: " << error_.what() << std::endl;
	}
}