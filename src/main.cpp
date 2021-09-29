#include "main.h"

int main(int argc, char** argv)
{
	cxxopts::Options options( argv[0] );
	options.add_options()
		( "i, input", "input image", cxxopts::value<std::string>() )
		( "o, output", "output image", cxxopts::value<std::string>() )
		( "r, radius", "defocus radius", cxxopts::value<double>() )
		( "s, sides", "defocus kernel sides", cxxopts::value<int>() )
		( "l, length", "motion blur length", cxxopts::value<int>() )
		( "v, vertice", "motion blur corners", cxxopts::value<int>() )
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
	int length;
	int vertice;
	int debug;

	if ( result.count( "input" ) )
		input = result["input"].as<std::string>();

	if ( result.count( "output" ) )
		output = result["output"].as<std::string>();

	if ( result.count( "radius" ) )
		radius = result["radius"].as<double>();

	if ( result.count( "sides" ) )
		sides = result["sides"].as<int>();

	if ( result.count( "length" ) )
		length = result["length"].as<int>();

	if ( result.count( "vertice" ) )
		vertice = result["vertice"].as<int>();

	if ( result.count( "debug" ) )
		debug = result["debug"].as<int>();

	Magick::EnableOpenCL();
	Magick::InitializeMagick(*argv);
	Magick::Image inImg, Defocussed, Defocussed_002, DefocussedOutput;

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
	polyVertices_De( polyBlob, polyPGM, static_cast<int>(radius), radiVert, radii, radii, -90.0, diameter, debug );
	motion_blur_kernel( length, vertice, debug );


	int width = static_cast<int>(inImg.baseColumns());
	int height = static_cast<int>(inImg.baseRows());
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
	Defocussed.magick( "PNG" );
	Defocussed.write( &defocusBlob );
	// end pad image edges

	// process first blur kernel
	convolve( Defocussed_002, defocusBlob, output, Width, Height, size, "poly_new.txt", debug);
	convolve( Defocussed_002, defocusBlob, output, Width, Height, size, "mBpoly_new.txt", debug);



	DefocussedOutput.read( defocusBlob );
	DefocussedOutput.crop( Magick::Geometry( Width, Height, 64, 64 ) );
	DefocussedOutput.filterType( Magick::PointFilter );
	DefocussedOutput.resize( "25%" );
	DefocussedOutput.write( output );
}