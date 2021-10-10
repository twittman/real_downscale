#include "uni_grain.h"

void uni_grain( Magick::Image& input,
				  size_t Width, size_t Height,
				  std::string size )
{

	std::random_device rd;
	std::mt19937 generator( rd() );

	std::uniform_real_distribution<double> noiseQ( 0.0, 0.13 );
	double noiseZ = noiseQ( rd );
	std::uniform_real_distribution<double> blurQ( 0.1, 0.8 );
	double blurZ = blurQ( rd );

	std::uniform_int_distribution<int> ranPix( 0, 9 );
	int randPixels = ranPix( rd );

	Magick::Image grainLayer( Magick::Geometry( size ), "fractal" ), singlePixel( "1x1", "#f2f2f2" );

	std::uniform_int_distribution<int> randomRow( 0 + 20, static_cast<int>( Width ) - 20 );
	std::uniform_int_distribution<int> randomCol( 0 + 20, static_cast<int>( Height ) - 20 );

	std::uniform_int_distribution<int> randomPixelNum( 0, 8 );
	int randomPixel = randomPixelNum( rd );

	grainLayer.alpha( false );
	grainLayer.alpha( true );
	grainLayer.addNoise( Magick::LaplacianNoise );
	grainLayer.addNoise( Magick::PoissonNoise );
	grainLayer.colorSpace( Magick::GRAYColorspace );
	grainLayer.gaussianBlur( 0, blurZ );
	grainLayer.evaluate( Magick::AlphaChannel, Magick::MultiplyEvaluateOperator, noiseZ );
	input.composite( grainLayer, 0, 0, Magick::OverlayCompositeOp );


	if ( randPixels <= 1 ) {
		for ( int i = 0; i < randomPixel; i++ ) {
			int pixRow = randomRow( rd );
			int pixCol = randomCol( rd );
			input.composite( singlePixel, pixRow, pixCol, Magick::OverCompositeOp );
		}
	};

}