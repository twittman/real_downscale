#include "convolve.h"

void convolve( Magick::Image Defocussed, 
					 Magick::Blob defocusBlob, 
					 std::string& output, 
					 auto& Width, auto& Height, 
					 std::string& size )
{
	try {
		std::ifstream textOpen;
		textOpen.open( "poly_new.txt" );
		std::string str( ( std::istreambuf_iterator<char>( textOpen ) ),
						 std::istreambuf_iterator<char>() );
		std::string kernel_motion = str;
		textOpen.close();
		std::filesystem::remove( "poly_new.txt" );
		std::filesystem::remove( "poly.txt" );


		// add grain before convolve
		std::random_device rd;
		std::mt19937 generator( rd() );

		std::uniform_real_distribution<double> noiseQ( 0.0, 0.11 );
		double noiseZ = noiseQ( rd );
		std::uniform_real_distribution<double> blurQ( 0.2, 0.8 );
		double blurZ = blurQ( rd );

		Magick::Image grainLayer( Magick::Geometry( size ), "fractal" ), singlePixel( "1x1", "#f2f2f2" );

		std::uniform_int_distribution<int> randomRow( 0 + 20, Width - 20 );
		std::uniform_int_distribution<int> randomCol( 0 + 20, Height - 20 );

		std::uniform_int_distribution<int> randomPixelNum( 64, 192 );
		int randomPixel = randomPixelNum( rd );

		grainLayer.alpha( false );
		grainLayer.alpha( true );
		grainLayer.addNoise( Magick::LaplacianNoise );
		grainLayer.addNoise( Magick::PoissonNoise );
		grainLayer.colorSpace( Magick::GRAYColorspace );
		grainLayer.gaussianBlur( 0, blurZ );
		grainLayer.evaluate( Magick::AlphaChannel, Magick::MultiplyEvaluateOperator, noiseZ );
		Defocussed.composite( grainLayer, 0, 0, Magick::OverlayCompositeOp );
		// end of add grain before convolve

		for ( int i = 0; i < randomPixel; i++ ) {
			int pixRow = randomRow( rd );
			int pixCol = randomCol( rd );
			Defocussed.composite( singlePixel, pixRow, pixCol, Magick::OverCompositeOp );
		}



		// convolve with defocus blur kernel
		Defocussed.artifact( "convolve:scale", "\!" );
		Defocussed.morphology( Magick::ConvolveMorphology, kernel_motion );
		Defocussed.depth( 8 );
		Defocussed.colorSpace( Magick::sRGBColorspace );
		Defocussed.crop( Magick::Geometry( Width, Height, 64, 64 ) );
		Defocussed.filterType( Magick::PointFilter );
		Defocussed.resize( "25%" );
		Defocussed.magick( "PNG" );
		Defocussed.write( output );
		// end convolve with defocus blur kernel
	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, first convolution: " << error_.what() << std::endl;
	}
}