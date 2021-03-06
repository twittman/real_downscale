#include "defocusBlur.h"

void print( std::list<std::string> const &list )
{
	for ( auto const& i : list ) {
		std::cout << i << std::endl;
	}
}
// Read the Blob and edit the output array
void read_PGM_as_string( const char* filename, std::list<std::string>& lines, int& width, int& height )
{
	//std::cout << "Reading PGM as string\n";
	lines.clear();
	std::ifstream file( filename );
	std::string s;
	while ( std::getline( file, s ) )
		lines.push_back( s );
	lines.pop_front();
	lines.pop_front();
	lines.pop_front();

	std::ostringstream stringReplace;

	stringReplace << width << "x" << height << ":";
	std::string width_height = stringReplace.str();
	lines.push_front( width_height );
}

void polyVertices_De( Magick::Blob polyBlob, Magick::Blob polyPGM, 
				   double radius, int num, 
				   double rad1, double rad2, double offset, 
				   double diameter, 
				   std::string& scaleVal, 
				   std::string& fileNoPathNoEXT, std::stringstream& buffered, int& debug, int& memory,
					double& largeRadiiB, double& smallRadiiB, int& largeDispl, int& smallDispl, double& ranRot)
{
	try {
		int distAmnt_01 = static_cast<int>( rad1 ) / 7;
		int distAmnt_02 = static_cast<int>( rad1 ) / 13;
		//std::cout << distAmnt << std::endl;


		double pi = 2 * acos( 0.0 );
		int number_01 = num;
		double radius_01 = rad1;
		double radius_02 = rad2;
		double offset_01 = offset;
		double xx;
		double yy;
		std::vector<Magick::Coordinate> vertice_locations;
		for ( int i = 0; i < number_01; i++ ) {
			double angle = i * static_cast<double>( 360 ) / number_01 + offset;
			xx = radius_02 * cos( angle * pi / 180 ) + radius_01 - 0.5;
			yy = radius_02 * sin( angle * pi / 180 ) + radius_01 - 0.5;
			vertice_locations.emplace_back( xx, yy );
		}
		defocussBlurr( polyBlob, polyPGM, 
					   radius, vertice_locations, 
					   static_cast<int>( diameter ), 
					   distAmnt_01, distAmnt_02, 
					   scaleVal,
					   fileNoPathNoEXT, buffered, debug, memory,
					   largeRadiiB, smallRadiiB, largeDispl, smallDispl, ranRot);
	}
	catch ( const std::exception& e ) {
		std::cerr << "Problem with polyVertices_De: " << e.what() << std::endl;
	}
}
void defocussBlurr( Magick::Blob polyBlob, Magick::Blob polyPGM, 
					int radius, 
					std::vector<Magick::Coordinate> vertices, 
					int diameter, int distAmnt_01, int distAmnt_02, 
					std::string& scaleVal,
					std::string& fileNoPathNoEXT, std::stringstream& buffered, int& debug, int& memory,
					double& largeRadiiB, double& smallRadiiB, int& largeDispl, int& smallDispl, double& ranRot)
{



	try {
		std::string xDisp;
		std::string yDisp;

		if (radius > 6) {
			xDisp = std::to_string(largeDispl) + "x" + std::to_string(0);
			yDisp = std::to_string(0) + "x" + std::to_string(largeDispl);
		}
		else {
			xDisp = std::to_string(smallDispl) + "x" + std::to_string(0);
			yDisp = std::to_string(0) + "x" + std::to_string(smallDispl);
		}

		//if ( radius > 6 ) {
		//	xDisp = "13x0";
		//	yDisp = "0x13";
		//}
		//else {
		//	xDisp = "5x0";
		//	yDisp = "0x5";
		//}

		Magick::Image polyGon( Magick::Geometry( diameter, diameter ), "black" ),
			displaceNoise( Magick::Geometry( diameter, diameter ), "fractal" ),
			polyGon8x,
			polyBlobPGM;
		Magick::Blob bufferBlob;
		//std::cout << "\nGenerating grain for displacement\n";
		displaceNoise.resize( "1000%" );
		displaceNoise.addNoise( Magick::PoissonNoise );

		if ( radius > 12 ) {
			displaceNoise.gaussianBlur( 0, largeRadiiB );
		}
		else {
			displaceNoise.gaussianBlur( 0, smallRadiiB);
		}

		displaceNoise.colorSpace( Magick::GRAYColorspace );
		displaceNoise.autoLevel();
		displaceNoise.resize( "50%" );

		if ( debug == 1 ) {
			displaceNoise.write( "noise.png" );
		}

		//std::cout << "Drawing Polygon\n";
		polyGon.magick( "PNG" );
		polyGon.fillColor( "white" );
		polyGon.draw( Magick::DrawablePolygon( vertices ) );
		polyGon.trim();

		std::string Eucl_01 = std::to_string( distAmnt_01 ) + ',' + std::to_string( distAmnt_02 ) + '!';

		polyGon.borderColor( "Black" );
		polyGon.border( Magick::Geometry( 9, 9, 9, 9 ) );

		//std::cout << "Displacing Polygon\n";
		// apply displacement to kernel for randomness
		polyGon.artifact( "compose:args", xDisp );
		polyGon.composite( displaceNoise, 0, 0, Magick::DisplaceCompositeOp );
		polyGon.artifact( "compose:args", yDisp );
		polyGon.composite( displaceNoise, 0, 0, Magick::DisplaceCompositeOp );

		polyGon.backgroundColor(Magick::Color("black"));
		polyGon.rotate(ranRot);

		polyGon.morphology( Magick::DistanceMorphology, Magick::EuclideanKernel, Eucl_01 );
		if ( radius > 5 ) {
			polyGon.blur( 0, 2.18 );
		}
		else {
			polyGon.blur( 0, 1.4 );
		}

		if ( radius < 3.0 ) {
			polyGon.resize( scaleVal );
		}
		

		try {
			polyGon.trim();
		}
		catch ( Magick::Exception& error_ ) {
			std::cerr << "Caught exception Trimming first convolution: " << error_.what() << std::endl;
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

		int polyWidth = static_cast<int>( polyBlobPGM.baseColumns() );
		int polyHeight = static_cast<int>( polyBlobPGM.baseRows() );

		std::string dimensions = std::to_string( polyWidth ) + "x" + std::to_string( polyHeight );

		polyBlobPGM.magick( "PGM" );
		polyBlobPGM.compressType( Magick::NoCompression );
		polyBlobPGM.depth( 8 );

		std::list<std::string> lines;
		std::ofstream text;
		std::stringstream* textBuff;

		if ( memory == 1 ) {
			polyBlobPGM.write( "poly.txt" );
			/*polyBlobPGM.write( &bufferBlob );
			void* b = static_cast<void*>( &bufferBlob );
			textBuff = static_cast<std::stringstream*>( b );*/
			read_PGM_as_string( "poly.txt", lines, polyWidth, polyHeight );
			for ( auto v : lines ) {
				buffered << v << "\n";
				//std::cout << v << std::endl;
			}
		}
		else {
			polyBlobPGM.write( "poly.txt" );
			read_PGM_as_string( "poly.txt", lines, polyWidth, polyHeight );
			text.open( "poly_new.txt" );
			for ( auto v : lines ) {
				text << v << "\n";
				//std::cout << v << std::endl;
			}
			text.close();
		}
	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, defocus generator: " << error_.what() << std::endl;
	}
}
