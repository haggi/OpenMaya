#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <vector>
#include "boost/version.hpp"
#define VENDOR "haggis vfx & animation"

#define MTXX_VERSION_MAJOR 0
#define MTXX_VERSION_MINOR 12
#define MTXX_VERSION_PATCH 0
#define MTXX_VERSION_RELEASE_TYPE dev

#define MTXX_MAKE_VERSION_STRING2(a,b,c,d) #a "." #b "." #c #d
#define MTXX_MAKE_VERSION_STRING(a,b,c,d) MTXX_MAKE_VERSION_STRING2(a,b,c,d)
#define MTXX_VERSION_STRING \
    MTXX_MAKE_VERSION_STRING(MTXX_VERSION_MAJOR, \
                             MTXX_VERSION_MINOR, MTXX_VERSION_PATCH, \
                             MTXX_VERSION_RELEASE_TYPE)


inline std::vector<std::string> getFullVersionString()
{
	std::vector<std::string> versionData;
	versionData.push_back(MTXX_VERSION_STRING);
	return versionData;
}

#endif