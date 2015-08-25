#ifndef APPLESEED_GEOMETRY_H
#define APPLESEED_GEOMETRY_H

#include "renderer/api/object.h"
#include <maya/MObject.h>

namespace asf = foundation;
namespace asr = renderer;

namespace MTAP_GEOMETRY{

	asf::auto_release_ptr<asr::MeshObject> defineStandardPlane();
	asf::auto_release_ptr<asr::MeshObject> createMesh(MObject& mobject);

};

#endif