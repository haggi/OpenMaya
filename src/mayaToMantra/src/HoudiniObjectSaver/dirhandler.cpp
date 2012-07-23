#include <iostream>
#include <stdlib.h>

#include <vector>
#include <fstream>
#include "dirhandler.h"
#include "pystring.h"

void getHsffFiles( const fs::path & directory, std::vector<std::string>& filesToConvert, bool binary )
{
	if( fs::exists( directory ) )
	{
		fs::directory_iterator end ;
		for( fs::directory_iterator iter(directory) ; iter != end ; ++iter )
		{
			if ( !fs::is_directory( *iter ) )
			{
				std::string fn(iter->path().string());
				//std::string fn(iter->filename());
				if( pystring::endswith(fn, ".hsff"))
				{
					//std::cout << "Found hsff file.. searching for .geo or .bgeo\n";
					fs::path hsffFile(fn);
					std::string geoF = pystring::replace(fn, ".hsff", ".geo");
					if( binary )
						geoF = pystring::replace(fn, ".hsff", ".bgeo");
						
					fs::path geoFile(geoF);
					//std::cout << "Check " << geoFile << " for existence\n";
					if(fs::exists(geoFile))
					{
						//std::cout << "Geo file exists checking modification dates.\n";
						std::time_t tg = fs::last_write_time( geoFile ) ;
						//std::cout << "UTC geoFile: " << std::asctime( std::gmtime(&tg) ) ;
						std::time_t th = fs::last_write_time( hsffFile ) ;
						//std::cout << "UTC hsffFile: " << std::asctime( std::gmtime(&th) ) ;
						if( th > tg)
						{
							//std::cout << "hsffFile ("<< th <<") is newer than geo file ("<< tg <<"), adding to translate list\n";
							filesToConvert.push_back(hsffFile.string());
						}

					}else{
						//std::cout << "No geo file, appending hsff to translate list\n";
						filesToConvert.push_back(hsffFile.string());
					}

				}
			}
		}
	}	
}

bool isValid(const fs::path& file)
{
	if(!fs::exists( file))
		return false;
	if(!fs::is_regular_file( file ))
		return false;
	if(!pystring::endswith(file.string(), "geo"))
		return false;
	return true;
}

//void mtime()
//{
//	std::string filename("C:/Users/haggi/coding/mayaTo/mtm_VS/HoudiniObjectSaver/x64/Debug/_particle1_PSystem.0.hsff");
//	struct tm *mt;				// create a time structure
//	struct stat attrib;			// create a file attribute structure
//	stat(filename.c_str(), &attrib);		// get the attributes of afile.txt
//	mt = gmtime(&(attrib.st_mtime));	// Get the last modified time and put it into the time structure
//
//   struct stat fileInfo;
//
//   if (stat(filename.c_str(), &fileInfo) != 0) {  // Use stat( ) to get the info
//      std::cerr << "Error: " << strerror(errno) << '\n';
//      return;
//   }
//
//   std::cout << "Type:         : ";
//   if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) { // From sys/types.h
//      std::cout << "Directory\n";
//   } else {
//      std::cout << "File\n";
//   }
//
//   std::cout << "Size          : " <<
//      fileInfo.st_size << '\n';               // Size in bytes
//   std::cout << "Device        : " <<
//      (char)(fileInfo.st_dev + 'A') << '\n';  // Device number
//   std::cout << "Created       : " <<
//      std::ctime(&fileInfo.st_ctime);         // Creation time
//   std::cout << "Modified      : " <<
//      std::ctime(&fileInfo.st_mtime);         // Last mod time
//
//}	
// clock->tm_year returns the year (since 1900)
	
// clock->tm_mon returns the month (January = 0)
	
// clock->tm_mday returns the day of the month

