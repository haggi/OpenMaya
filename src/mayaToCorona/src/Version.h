#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <vector>
#include "OpenImageIO/oiioversion.h"
#include "OSL/oslversion.h"
#include "boost/version.hpp"
#include "OpenEXR\OpenExrConfig.h"
#include "CoronaCore/api/Api.h"
#define VENDOR "haggis vfx & animation"

#define MTXX_VERSION_MAJOR 0
#define MTXX_VERSION_MINOR 41
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
	Corona::LicenseInfo li = Corona::ICore::getLicenseInfo();
	std::string license = li.getDescription();
	versionData.push_back(MTXX_VERSION_STRING);
	versionData.push_back(CORONA_BUILD_NAME);
	versionData.push_back(OIIO_VERSION_STRING);
	versionData.push_back(OSL_LIBRARY_VERSION_STRING);
	versionData.push_back(BOOST_LIB_VERSION);
	versionData.push_back(OPENEXR_VERSION_STRING);
	versionData.push_back("LICENSE");
	versionData.push_back(license);
	return versionData;
}

#endif