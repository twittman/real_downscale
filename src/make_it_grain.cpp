#include "make_it_grain.h"

void makeitgrain( Magick::Image& Defocussed_002,
				  Magick::Blob& defocusBlob,
				  size_t Width, size_t Height,
				  std::string size )
{
	// add grain before convolve
	std::random_device rd;
	std::mt19937 generator( rd() );

	std::uniform_real_distribution<double> noiseQ( 0.0, 0.11 );
	double noiseZ = noiseQ( rd );
	std::uniform_real_distribution<double> blurQ( 0.2, 0.8 );
	double blurZ = blurQ( rd );

	Magick::Image grainLayer( Magick::Geometry( size ), "fractal" ), singlePixel( "1x1", "#f2f2f2" );

	std::uniform_int_distribution<int> randomRow( 0 + 20, static_cast<int>( Width ) - 20 );
	std::uniform_int_distribution<int> randomCol( 0 + 20, static_cast<int>( Height ) - 20 );

	std::uniform_int_distribution<int> randomPixelNum( 64, 192 );
	int randomPixel = randomPixelNum( rd );

	grainLayer.alpha( false );
	grainLayer.alpha( true );
	grainLayer.addNoise( Magick::LaplacianNoise );
	grainLayer.addNoise( Magick::PoissonNoise );
	grainLayer.colorSpace( Magick::GRAYColorspace );
	grainLayer.gaussianBlur( 0, blurZ );
	grainLayer.evaluate( Magick::AlphaChannel, Magick::MultiplyEvaluateOperator, noiseZ );
	Defocussed_002.composite( grainLayer, 0, 0, Magick::OverlayCompositeOp );
	// end of add grain before convolve

	for ( int i = 0; i < randomPixel; i++ ) {
		int pixRow = randomRow( rd );
		int pixCol = randomCol( rd );
		Defocussed_002.composite( singlePixel, pixRow, pixCol, Magick::OverCompositeOp );
	}

}