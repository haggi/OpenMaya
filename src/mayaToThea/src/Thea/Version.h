#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <vector>
#include <SDK/Integration/sdk.h>
#include "boost/version.hpp"
#define VENDOR "haggis vfx & animation"

#define MTXX_VERSION_MAJOR 0
#define MTXX_VERSION_MINOR 02
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
	std::string theaVersion = TheaSDK::GetVersion().getSafeData();
	int vNumber = TheaSDK::GetVersionNumber();
	std::string edition = TheaSDK::GetEdition().getSafeData();
	versionData.push_back(MTXX_VERSION_STRING);
	versionData.push_back(theaVersion);
	versionData.push_back((MString("") + vNumber).asChar());
	versionData.push_back(edition);
	versionData.push_back(BOOST_LIB_VERSION);
	return versionData;
}

#endif