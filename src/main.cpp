#include "main.h"
#include "read_PGM_as_string.hpp"

// Read the Blob and edit the output array
//void read_PGM_as_string( const char* filename, std::list<std::string>& lines, int& width, int& height )
//{
//	lines.clear();
//	std::ifstream file( filename );
//	std::string s;
//	while ( std::getline( file, s ) )
//		lines.push_back( s );
//	lines.pop_front();
//	lines.pop_front();
//	lines.pop_front();
//
//	std::ostringstream stringReplace;
//
//	stringReplace << width << "x" << height << ":";
//	std::string width_height = stringReplace.str();
//	lines.push_front( width_height );
//}

void defocussBlurr( int radius, std::vector<Magick::Coordinate> vertices, int diameter, int distAmnt_01, int distAmnt_02, int& debug )
{
	std::string xDisp;
	std::string yDisp;

	if ( radius > 9 ) {
		xDisp = "13x0";
		yDisp = "0x13";
	}
	else {
		xDisp = "5x0";
		yDisp = "0x5";
	}

	Magick::InitializeMagick;
	Magick::Image polyGon( Magick::Geometry( diameter, diameter ), "black" ), polyGon8x,
		polyBlobPGM, 
		displaceNoise(Magick::Geometry(diameter,diameter), "fractal");

	displaceNoise.resize( "1000%" );
	displaceNoise.addNoise( Magick::PoissonNoise );

	if ( radius > 12 ) {
		displaceNoise.gaussianBlur( 0, 11 );
	}
	else {
		displaceNoise.gaussianBlur( 0, 5 );
	}

	displaceNoise.colorSpace( Magick::GRAYColorspace );
	displaceNoise.autoLevel();
	displaceNoise.resize( "50%" );

	if ( debug == 1 ) {
		displaceNoise.write( "noise.png" );
	}

	Magick::Blob polyBlob, polyPGM;
	polyGon.magick( "PNG" );
	polyGon.fillColor( "white" );
	polyGon.draw( Magick::DrawablePolygon( vertices ) );
	polyGon.trim();

	std::string Eucl_01 = std::to_string( distAmnt_01 ) + ',' + std::to_string( distAmnt_02 ) + '!';

	polyGon.borderColor( "Black" );
	polyGon.border( Magick::Geometry( 9, 9, 9, 9 ) );

	// apply displacement to kernel for randomness
	polyGon.artifact( "compose:args", xDisp );
	polyGon.composite( displaceNoise, 0, 0, Magick::DisplaceCompositeOp );
	polyGon.artifact( "compose:args", yDisp );
	polyGon.composite( displaceNoise, 0, 0, Magick::DisplaceCompositeOp );

	polyGon.morphology( Magick::DistanceMorphology, Magick::EuclideanKernel, Eucl_01 );
	if ( radius > 9 ) {
		polyGon.blur( 0, 2.78 );
	}
	else {
		polyGon.blur( 0, 1.6 );
	}


	if ( radius < 3 ) {
		polyGon.resize( "50%" );
	}

	polyGon.write( &polyBlob ); // Write to memory

	if ( debug == 1 ) {
		polyGon.write( "poly.png" ); // Write to png
		polyGon8x = polyGon;
		polyGon8x.filterType( Magick::PointFilter );
		polyGon8x.resize( "800%" );
		polyGon8x.write( "poly_8x.png" );
	}

	polyBlobPGM.read( polyBlob );

	int polyWidth = polyBlobPGM.baseColumns();
	int polyHeight = polyBlobPGM.baseRows();

	std::string dimensions = std::to_string( polyWidth ) + "x" + std::to_string( polyHeight );

	polyBlobPGM.magick( "PGM" );
	polyBlobPGM.compressType( Magick::NoCompression );
	polyBlobPGM.depth( 8 );
	polyBlobPGM.write( "poly.txt" );


	std::list<std::string> lines;
	readDatThing::PGM_read::PGM_fix( "poly.txt", lines, polyWidth, polyHeight );
	std::ofstream text;
	text.open( "poly_new.txt" );
	for ( auto v : lines ) {
		text << v << "\n";
		//std::cout << v << std::endl;
	}
	text.close();
	std::filesystem::remove( "poly.txt" );
	std::filesystem::remove( "poly.pgm" );
}

//void polyVertices( int radius, int num, double rad1, double rad2, double offset, double diameter, int& debug )
//{
//	int distAmnt_01 = rad1 / 7;
//	int distAmnt_02 = rad1 / 13;
//	//std::cout << distAmnt << std::endl;
//
//	double pi = 2 * acos( 0.0 );
//	int number_01 = num;
//	double radius_01 = rad1;
//	double radius_02 = rad2;
//	double offset_01 = offset;
//	double xx;
//	double yy;
//
//	std::vector<Magick::Coordinate> vertice_locations;
//	for ( int i = 0; i < number_01; i++ ) {
//		double angle = i * static_cast<double>( 360 ) / number_01 + offset;
//		xx = radius_02 * cos( angle * pi / 180 ) + radius_01 - 0.5;
//		yy = radius_02 * sin( angle * pi / 180 ) + radius_01 - 0.5;
//		vertice_locations.emplace_back( xx, yy );
//	}
//	defocussBlurr( radius, vertice_locations, diameter, distAmnt_01, distAmnt_02, debug );
//}


void convolve_first( Magick::Image Defocussed, Magick::Blob defocusBlob, std::string& output, auto& Width, auto& Height, std::string& size )
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

int main(int argc, char** argv)
{
	cxxopts::Options options( argv[0] );
	options.add_options()
		( "i, input", "input image", cxxopts::value<std::string>() )
		( "o, output", "output image", cxxopts::value<std::string>() )
		( "r, radius", "defocus radius", cxxopts::value<double>() )
		( "s, sides", "defocus kernel sides", cxxopts::value<int>() )
		( "d, debug", "save kernels?", cxxopts::value<int>() )
		( "h, help", "print help" )
		;
	auto result = options.parse( argc, argv );

	if ( result.count( "help" ) )
	{
		std::cout << options.help() << std::endl;
		exit( 0 );
	}

	std::string input;
	std::string output;
	double radius;
	int sides;
	int debug;

	if ( result.count( "input" ) )
		input = result["input"].as<std::string>();

	if ( result.count( "output" ) )
		output = result["output"].as<std::string>();

	if ( result.count( "radius" ) )
		radius = result["radius"].as<double>();

	if ( result.count( "sides" ) )
		sides = result["sides"].as<int>();

	if ( result.count( "debug" ) )
		debug = result["debug"].as<int>();

	Magick::EnableOpenCL;
	Magick::InitializeMagick(*argv);
	Magick::Image inImg, Defocussed;

	inImg.read( input );
	inImg.depth( 16 );
	inImg.colorSpace( Magick::RGBColorspace );

	// DEFOCUS BLUR
	//double radii = radius;
	double radii;
	if ( radius < 3 ) {
		radii = 3;
	}
	else {
		radii = radius;
	}
	double diameter = radii * 2;
	int radiVert = sides;

	if ( debug == 1 ) {
		std::cout << "Radii: " << radii << "\n"
			<< "Diameter: " << diameter << "\n"
			<< "Sides: " << radiVert << std::endl;;
	}

	// create defocus polygon shape
	Magick::Blob polyBlob, polyPGM, defocusBlob;
	polyVertices( polyBlob, polyPGM, radius, radiVert, radii, radii, -90.0, diameter, debug );


	int width = inImg.baseColumns();
	int height = inImg.baseRows();
	std::string size = std::to_string( width ) + "x" + std::to_string( height );

	Defocussed = inImg;

	// pad image edges
	auto Width = Defocussed.columns();
	auto Height = Defocussed.rows();

	std::string stretchImg;
	stretchImg += std::to_string( Width + 128 );
	stretchImg += "x";
	stretchImg += std::to_string( Height + 128 );
	stretchImg += "-";
	stretchImg += std::to_string( 64 );
	stretchImg += "-";
	stretchImg += std::to_string( 64 );

	auto argNum = 1;
	double* argList = new double[argNum];
	argList[0] = 0.0;
	bool bestFit = Magick::MagickFalse;
	Defocussed.artifact( "distort:viewport", stretchImg );
	Defocussed.virtualPixelMethod( Magick::MirrorVirtualPixelMethod );
	Defocussed.distort( Magick::ScaleRotateTranslateDistortion, argNum, argList, bestFit );
	Defocussed.repage();
	// end pad image edges

	// process both blur kernels
	convolve_first( Defocussed, defocusBlob, output, Width, Height, size );

}