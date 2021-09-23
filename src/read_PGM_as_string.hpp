#pragma once
#include "main.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


namespace readDatThing {
	namespace PGM_read {
		std::ifstream PGM_fix( const char* filename, std::list<std::string>& lines, int& width, int& height )
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

			return file;
		}
	}
}
