#ifndef DIR_HANDLER_H
#define DIR_HANDLER_H

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
void getHsffFiles( const fs::path& directory, std::vector<std::string>& filesToConvert, bool binary );

bool isValid( const fs::path& file);
#endif