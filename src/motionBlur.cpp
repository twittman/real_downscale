#include "motionBlur.h"

// Read the Blob and edit the output array
void read_PGM_as_string_mB( const char* filename, std::list<std::string>& lines, int& width, int& height )
{
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
// Create the polygon shape
void mBlurr( std::vector<Magick::Coordinate> vertices, int& maxlength, int& debug )
{
	try {
		Magick::Image polyGon( Magick::Geometry( maxlength, maxlength ), "black" ), mBpolyBlobPGM, polyGon8x;
		Magick::Blob mBpolyBlob, mBpolyPGM;
		polyGon.magick( "PNG" );
		polyGon.strokeWidth( 1 );
		polyGon.strokeColor( "white" );
		polyGon.strokeAntiAlias( true );
		polyGon.draw( Magick::DrawableBezier( vertices ) );

		try {
			polyGon.trim();
		}
		catch ( Magick::Exception& error_ ) {
			std::cerr << "Caught exception, Trimming Motion blur kernel: " << error_.what() << std::endl;
		}
		polyGon.borderColor( "Black" );
		polyGon.border( Magick::Geometry( 3, 3, 3, 3 ) );
		polyGon.write( &mBpolyBlob ); // Write to memory
		if ( debug == 1 ) {
			polyGon.write( "mBpoly.png" ); // Write to png
			polyGon8x = polyGon;
			polyGon8x.filterType( Magick::PointFilter );
			polyGon8x.resize( "800%" );
			polyGon8x.write( "mBpoly_8x.png" );
		}



		mBpolyBlobPGM.read( mBpolyBlob );

		int polyWidth = static_cast<int>( mBpolyBlobPGM.baseColumns() );
		int polyHeight = static_cast<int>( mBpolyBlobPGM.baseRows() );

		std::string dimensions = std::to_string( polyWidth ) + "x" + std::to_string( polyHeight );

		mBpolyBlobPGM.magick( "PGM" );
		mBpolyBlobPGM.compressType( Magick::NoCompression );
		mBpolyBlobPGM.depth( 8 );
		mBpolyBlobPGM.write( "mBpoly.txt" );


		std::list<std::string> lines;
		read_PGM_as_string_mB( "mBpoly.txt", lines, polyWidth, polyHeight );
		std::ofstream text;
		text.open( "mBpoly_new.txt" );
		for ( auto v : lines ) {
			text << v << "\n";
		}
		text.close();
	}
	catch ( Magick::Exception& error_ ) {
		std::cerr << "Caught exception, motion blur kernel: " << error_.what() << std::endl;
	}
}
// generate vertices
void motion_blur_kernel( int& maxlength, int& vertices, std::string& fileNoPathNoEXT, int& debug )
{
	std::random_device rd;
	std::mt19937 generator( rd() );
	int num = vertices;
	int xx;
	int yy;
	int vNum = 0;

	std::vector<Magick::Coordinate> vertice_loc;
	for ( int i = 0; i < num; i++ ) {

		std::uniform_int_distribution<int> xxR( 0, maxlength );
		std::uniform_int_distribution<int> yyR( 0, maxlength );
		xx = xxR( rd );
		yy = yyR( rd );

		vertice_loc.emplace_back( xx, yy );
		if ( debug == 1 ) {
			std::cout << "\nVertice #" << ++vNum << ": " << "x:" << xx << " " << "y:" << yy << std::endl;
		}
	}

	mBlurr( vertice_loc, maxlength, debug );
}