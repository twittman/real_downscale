#include "main.h"

void runProcess( std::string& inFile, std::string& outFile, 
				 std::string& fileNoPathNoEXT,
				 double& radius, int& sides, int debug, 
				 int& length, int& vertice, int& scale,
				 double& grain, double& jpeg, double& gaussian,
				 int& memory)
{
	try {
		//Magick::EnableOpenCL();
		Magick::InitializeMagick("");
		Magick::Image inImg, Defocussed, Defocussed_002, DefocussedOutput;

		inImg.read( inFile );
		inImg.depth( 16 );
		inImg.colorSpace( Magick::RGBColorspace );

		// DEFOCUS BLUR
		//double radii = radius;
		double radii;
		double radiusF;
		if ( radius < 3 ) {
			radii = 8;
			radiusF = 8;
		}
		else if ( radius == 2 ) {
			radii = 6;
			radiusF = 6;
		}
		else {
			radii = radius;
			radiusF = radius;
		}
		double diameter = radii * 2;
		int radiVert = sides;
		double bloomDia = radius * 2;

		auto mapDaRange = twitls::count::mapRange( radius, 0.0, 3.0, 10, 50 );
		std::string scaleVal = std::to_string( mapDaRange ) + "%";
		if ( debug == 1 ) {
			std::cout << "Scale Value: " << scaleVal << "\n";
		}

		std::string polyDeFocusName = fileNoPathNoEXT + "_Poly";
		std::string polyMotionName = fileNoPathNoEXT + "_Motion";


		// Getting smol chance of Gaussian Blur instead of deFocus blur //
		// 0.05 = 5% of images will be Gaussian blurred
		auto chanceOfGaussianRandomNumer = twitls::randgen::randomNumber( 0, 100 );
		double chanceOfGaussianPercent = 100 * gaussian;
		// Getting randomised range of floats for Gaussian Blur
		double gaussianRange = twitls::randgen::randomDouble( double( radius / 2 ), double( radius * 1.5 ) );

		// process blur kernel(s)
		std::string polyStr;
		std::string polyStrM;
		std::stringstream buffered;
		std::stringstream bufferedM;
		if ( memory == 1 ) {
			polyStr = "";
			polyStrM = "";
		}
		else {
			polyStr = "poly_new.txt";
			polyStrM = "mBpoly_new.txt";
		}

		if ( debug == 1 ) {
			if ( chanceOfGaussianRandomNumer <= static_cast<int>( chanceOfGaussianPercent ) ) {
				std::cout << "\nGaussian: " << gaussianRange << "\n" << std::endl;;
			}
			else {
				std::cout << "Radii: " << radii << "\n"
					<< "Diameter: " << diameter << "\n"
					<< "Sides: " << radiVert << std::endl;
			}
		}
		// create defocus polygon shape
		Magick::Blob polyBlob, polyPGM, defocusBlob;
		try {
			if ( chanceOfGaussianRandomNumer <= static_cast<int>( chanceOfGaussianPercent ) ) {

			}
			else {
				polyVertices_De( polyBlob, polyPGM, 
								 static_cast<int>( radius ), radiVert, 
								 radii, radii, -90.0, 
								 diameter, 
								 scaleVal, 
								 fileNoPathNoEXT, 
								 buffered, debug, memory );
			}
			if ( length >= 3 ) {
				motion_blur_kernel( length, vertice, fileNoPathNoEXT ,debug );
			};
		} catch ( Magick::Exception& error_ ) {
			std::cerr << "Caught exception Polygon generation: " << error_.what() << std::endl;
		}
	

		int width = static_cast<int>( inImg.baseColumns() );
		int height = static_cast<int>( inImg.baseRows() );
		std::string size = std::to_string( width ) + "x" + std::to_string( height );

		Defocussed = inImg;

		// pad image edges
		auto Width = Defocussed.baseColumns();
		auto Height = Defocussed.baseRows();

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

		//// process blur kernel(s)
		//std::string polyStr;
		//std::string polyStrM;
		//std::stringstream buffered;
		//std::stringstream bufferedM;
		//if ( memory == 1 ) {
		//	polyStr = "";
		//	polyStrM = "";
		//}
		//else {
		//	polyStr = "poly_new.txt";
		//	polyStrM = "mBpoly_new.txt";
		//}


		if ( chanceOfGaussianRandomNumer <= static_cast<int>( chanceOfGaussianPercent ) ) {
			gaussianBlur( Defocussed_002, defocusBlob, outFile, Width, Height, size, gaussianRange, debug );
		}
		else {
			convolve( Defocussed_002, defocusBlob, outFile, Width, Height, size, polyStr, buffered, memory, debug );
			//fuzzyBloom( Defocussed_002, defocusBlob, bloomDia );
		}
		if ( length >= 3 ) {
			convolve( Defocussed_002, defocusBlob, outFile, Width, Height, size, polyStrM, bufferedM, memory, debug );
		};
		// check for txt files and remove all
		// To-do:
		//	make all txt and pgm and png kernel files 
		//	named as the input images are with prepends or appends
		//
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

		float outScaleF = 100 / static_cast<float>(scale);
		float outScaleFi = round( outScaleF * 1000.0 ) / 1000.0;

		outputScale += std::to_string( outScaleFi );
		outputScale += "%";

		//std::cout << "Scale: " << outputScale << "\n";


		std::string size_img = std::to_string( Width ) + "x" + std::to_string( Height );
		std::string size_img_b = std::to_string( Width * 1.5 ) + "x" + std::to_string( Height * 1.5 );

		DefocussedOutput.read( defocusBlob );

		int minJ = 40;
		int maxJ = 90;

		int grainChance = twitls::randgen::randomNumber( 0, 100 );
		int jpegChance = twitls::randgen::randomNumber( 0, 100 );

		double grainPercent = 100 * grain;
		double jpegPercent = 100 * jpeg;

		//std::cout << "Grain percentage: " << static_cast<int>( grainPercent ) << "\n";
		//std::cout << "Jpeg percentage: " << static_cast<int>( jpegPercent ) << "\n";

		if ( grainChance <= static_cast<int>( grainPercent ) ) {
			uni_grain( DefocussedOutput, Width, Height, size_img_b );
		}

		if ( jpegChance <= static_cast<int>( jpegPercent ) ) {
			//jPegger( DefocussedOutput, minJ, maxJ );
			DefocussedOutput.sharpen( 0, 0.6 );
			jPeggMulti( DefocussedOutput, minJ, maxJ );
		}

		DefocussedOutput.crop( Magick::Geometry( Width, Height, 64, 64 ) );
		if ( scale != 1 ) {
			DefocussedOutput.filterType( Magick::PointFilter );
			DefocussedOutput.resize( outputScale );
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
			   int& length, int& vertice, int& scale,
			   double& grain, double& jpeg, double& gaussian,
			   int& memory)
{
	int countFiles = twitls::count::countfiles( input );
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

			runProcess( inFileEXT, outFile, fileNoPathNoEXT, 
						radius, sides, debug, length, vertice, 
						scale, grain, jpeg, gaussian, memory );

			if ( debug != 1 ) {
				std::cout << "\rNumber of files remaining: " << --countFiles << "\t" << std::flush;
			}

		}
	}

}

int main(int argc, char** argv)
{
	cxxopts::Options options( argv[0] );
	options.add_options()
		( "i, input", "input image", cxxopts::value<std::string>() )
		( "o, output", "output image", cxxopts::value<std::string>() )
		( "g, grain", "Grain percentage", cxxopts::value<double>() )
		( "j, jpeg", "Jpeg percentage", cxxopts::value<double>() )
		( "b, gaussian", "Gaussian blur percentage", cxxopts::value<double>() )
		( "k, scale", "output image scale", cxxopts::value<int>() )
		( "r, radius", "defocus radius", cxxopts::value<double>() )
		( "s, sides", "defocus kernel sides", cxxopts::value<int>() )
		( "l, length", "motion blur length", cxxopts::value<int>() )
		( "v, vertice", "motion blur corners", cxxopts::value<int>() )
		( "m, memory", "process kernels in memory?", cxxopts::value<int>() )
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
	double grain;
	double jpeg;
	double gaussian;
	int scale;
	double radius;
	int sides;
	int length;
	int vertice;
	int memory = 0;
	int debug;

	if ( result.count( "input" ) )
		input = result["input"].as<std::string>();

	if ( result.count( "output" ) )
		output = result["output"].as<std::string>();

	if ( result.count( "grain" ) )
		grain = result["grain"].as<double>();

	if ( result.count( "gaussian" ) )
		gaussian = result["gaussian"].as<double>();

	if ( result.count( "jpeg" ) )
		jpeg = result["jpeg"].as<double>();

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

	if ( result.count( "memory" ) )
		memory = result["memory"].as<int>();

	if ( result.count( "debug" ) )
		debug = result["debug"].as<int>();

	std::filesystem::path inputDir( input );
	std::filesystem::path outputDir( output );

	std::string overwrite;
	if ( std::filesystem::exists( output ) ) {
		std::cout << "Output directory currently exists, do you want to overwrite? [Y}es, [N]o ";
		std::cin >> overwrite;
		if ( overwrite == "yes" || overwrite == "y" || overwrite == "yer" ) {
			std::filesystem::remove_all( output );
			if ( std::filesystem::create_directory( outputDir ) ) {
				std::cout << output << " Directory has been cleansed" << "\n";
				std::cout << output << " Directory has been created" << "\n";
			}
		}
		else if ( overwrite == "no" ) {
			return 0;
		}
	}
	else {
		if ( std::filesystem::create_directory( outputDir ) ) {
			std::cout << output << " Directory has been created" << "\n";
		}
	}

	//if ( std::filesystem::create_directory( outputDir ) ) {
	//	std::cout << output << " Directory created" << "\n";
	//}

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
			  radius, sides, debug, length, vertice, scale, grain, jpeg, gaussian, memory );

}