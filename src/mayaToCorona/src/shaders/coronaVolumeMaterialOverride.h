#ifndef VolumeMaterialOverride_H
#define VolumeMaterialOverride_H

#include <maya/MPxSurfaceShadingNodeOverride.h>

class VolumeMaterialOverride : public MHWRender::MPxSurfaceShadingNodeOverride
{
public:
	static MHWRender::MPxSurfaceShadingNodeOverride* creator(const MObject& obj);

	virtual ~VolumeMaterialOverride();
	virtual MHWRender::DrawAPI supportedDrawAPIs() const;
	virtual MString fragmentName() const;
	virtual MString primaryColorParameter() const;
	virtual MString transparencyParameter() const;
private:
	VolumeMaterialOverride(const MObject& obj);
};

#endif // _VolumeMaterialOverride
