#include <Magick++.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <random>
#include <stdio.h>
#include <fstream>
#include <streambuf>

void print( std::list<std::string> const &list );
void read_PGM_as_string( const char* filename, std::list<std::string>& lines, int& width, int& height );
void defocussBlurr( Magick::Blob polyBlob, Magick::Blob polyPGM,
					int radius,
					std::vector<Magick::Coordinate> vertices,
					int diameter, int distAmnt_01, int distAmnt_02,
					std::string& scaleVal,
					std::string& fileNoPathNoEXT, 
					std::stringstream& buffered, int& debug, int& memory,
					double& largeRadiiB, double& smallRadiiB, int& largeDispl, int& smallDispl, double& ranRot);

void polyVertices_De( Magick::Blob polyBlob, Magick::Blob polyPGM,
				   double radius, int num,
				   double rad1, double rad2, double offset,
				   double diameter, std::string& scaleVal, 
			       std::string& fileNoPathNoEXT, std::stringstream& buffered, int& debug, int& memory,
				   double& largeRadiiB, double& smallRadiiB, int& largeDispl, int& smallDispl, double& ranRot);