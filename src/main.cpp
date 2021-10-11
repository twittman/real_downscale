#include "main.h"

void runProcess( std::string& inFile, std::string& outFile, double& radius, int& sides, int debug, int& length, int& vertice, int& scale )
{
	try {
		Magick::EnableOpenCL();
		Magick::InitializeMagick;
		Magick::Image inImg, Defocussed, Defocussed_002, DefocussedOutput;

		inImg.read( inFile );
		inImg.depth( 16 );
		inImg.colorSpace( Magick::RGBColorspace );

		// DEFOCUS BLUR
		//double radii = radius;
		double radii;
		double radiusF;
		if ( radius == 3 ) {
			radii = 6;
			radiusF = 6;
		}
		else if ( radius <= 2 ) {
			radii = 6;
			radiusF = 6;
		}
		else {
			radii = radius;
			radiusF = radius;
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
		try {
			
			polyVertices_De( polyBlob, polyPGM, static_cast<int>( radius ), radiVert, radii, radii, -90.0, diameter, debug );
			if ( length >= 3 ) {
				motion_blur_kernel( length, vertice, debug );
			};
		} catch ( Magick::Exception& error_ ) {
			std::cerr << "Caught exception Polygon generation: " << error_.what() << std::endl;
		}
	

		int width = static_cast<int>( inImg.baseColumns() );
		int height = static_cast<int>( inImg.baseRows() );
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

		// process blur kernel(s)
		convolve( Defocussed_002, defocusBlob, outFile, Width, Height, size, "poly_new.txt", debug );
		if ( length >= 3 ) {
			convolve( Defocussed_002, defocusBlob, outFile, Width, Height, size, "mBpoly_new.txt", debug );
		};
		// check for txt files and remove all
		std::vector<std::string> filesForRemove = { "poly.txt",
													"poly.pgm",
													"poly_new.txt",
													"mBpoly.txt",
													"mBpoly.pgm",
													"mBpoly_new.txt" };
		for ( auto&& files : filesForRemove ) {
			if ( std::filesystem::exists( files ) )
			{
				std::filesystem::remove( files );
			}
		}


		std::string outputScale;
		switch ( scale ) {
			case 1:
				outputScale = "100%"; break;
			case 2:
				outputScale = "50%"; break;
			case 3:
				outputScale = "33.3%"; break;
			case 4:
				outputScale = "25%"; break;
			default:
				outputScale = "25%"; break;
		}

		DefocussedOutput.read( defocusBlob );
		DefocussedOutput.crop( Magick::Geometry( Width, Height, 64, 64 ) );
		DefocussedOutput.filterType( Magick::PointFilter );
		DefocussedOutput.resize( outputScale );


		// add grain before final save
		std::string size_img = std::to_string( Width ) + "x" + std::to_string( Height );
		uni_grain( DefocussedOutput, Width, Height, size_img );

		std::random_device rd;
		std::mt19937 generator( rd() );
		std::uniform_int_distribution<int> jP1( 0, 8 );
		int jP2 = jP1( rd );

		if ( jP2 <= 2 ) {
			// add jpeg compression
			int min = 66;
			int max = 95;
			jPegger( DefocussedOutput, min, max );
		}

		DefocussedOutput.write( outFile );
	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception on Run Process: " << error_.what() << std::endl;
	}
}

std::string getOutputName( std::string& fileName, std::string& fileDir )
{
	std::string outName;
	outName += fileDir;
	outName += "\\";
	outName += fileName;

	return outName;
}

void runOnDir( std::string& input,
			   std::filesystem::path& inputDir,
			   std::string& output,
			   std::filesystem::path& outputDir, 
			   double& radius, int& sides, int& debug,
			   int& length, int& vertice, int& scale )
{
	for ( const auto& entry : std::filesystem::directory_iterator( input ) ) {
		if ( is_regular_file( entry.path() ) ) {

			std::string ExtensionType = entry.path().extension().string();

			std::string inFileEXT = entry.path().string();
			const __int64 index1 = inFileEXT.find_last_of( "/\\" );
			std::string fileNoPathEXT = inFileEXT.substr( index1 + 1 );

			std::string inFileNoEXT = entry.path().stem().string();
			const __int64 index2 = inFileNoEXT.find_last_of( "/\\" );
			std::string fileNoPathNoEXT = inFileNoEXT.substr( index2 + 1 );

			std::string outFileDir = outputDir.string();
			std::string outFile = outFileDir + '\\' + fileNoPathNoEXT + ".png";

			runProcess( inFileEXT, outFile, radius, sides, debug, length, vertice, scale );

		}
	}
}

int main(int argc, char** argv)
{
	cxxopts::Options options( argv[0] );
	options.add_options()
		( "i, input", "input image", cxxopts::value<std::string>() )
		( "o, output", "output image", cxxopts::value<std::string>() )
		( "k, scale", "output image scale", cxxopts::value<int>() )
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
	int scale;
	double radius;
	int sides;
	int length;
	int vertice;
	int debug;

	if ( result.count( "input" ) )
		input = result["input"].as<std::string>();

	if ( result.count( "output" ) )
		output = result["output"].as<std::string>();

	if ( result.count( "scale" ) )
		scale = result["scale"].as<int>();
	
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

	std::filesystem::path inputDir( input );
	std::filesystem::path outputDir( output );

	if ( std::filesystem::create_directory( outputDir ) ) {
		std::cout << output << " Directory created" << "\n";
	}

	// first check if text files exist from previous run //
	std::vector<std::string> filesForRemove = { "poly.txt",
												"poly.pgm",
												"poly_new.txt",
												"mBpoly.txt",
												"mBpoly.pgm",
												"mBpoly_new.txt" };
	for ( auto&& files : filesForRemove ) {
		if ( std::filesystem::exists( files ) )
		{
			std::filesystem::remove( files );
		}
	}

	runOnDir( input, inputDir, output, outputDir, 
			  radius, sides, debug, length, vertice, scale );

}