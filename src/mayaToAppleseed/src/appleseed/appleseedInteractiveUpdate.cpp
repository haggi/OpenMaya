#include "appleseed.h"
#include "appleseedUtils.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "mayaScene.h"

using namespace AppleRender;

void AppleseedRenderer::doInteractiveUpdate()
{
	Logging::debug("AppleseedRenderer::doInteractiveUpdate");
	if (interactiveUpdateList.empty())
		return;
	for (auto iElement : interactiveUpdateList)
	{
		if (iElement->node.hasFn(MFn::kShadingEngine))
		{
			if (iElement->obj)
			{
				Logging::debug(MString("AppleseedRenderer::doInteractiveUpdate - found shadingEngine.") + iElement->name);
				MObject surface = getConnectedInNode(iElement->node, "surfaceShader");
				if (surface != MObject::kNullObj)
				{
					MFnDependencyNode depFn(surface);
					if (depFn.typeName() == "aeLayeredShader")
					{
						std::shared_ptr<mtap_MayaObject> obj = std::static_pointer_cast<mtap_MayaObject>(iElement->obj);
						//this->defineMaterial(obj->instance, obj);
					}
				}
			}
		}
		if (iElement->node.hasFn(MFn::kCamera))
		{
			Logging::debug(MString("AppleseedRenderer::doInteractiveUpdate - found camera.") + iElement->name);
			if (iElement->obj)
				defineCamera(iElement->obj);
		}
		if (iElement->node.hasFn(MFn::kLight))
		{
			Logging::debug(MString("AppleseedRenderer::doInteractiveUpdate - found light.") + iElement->name);
			if (iElement->obj)
			{
				defineLight(iElement->obj);
			}
		}
		if (iElement->node.hasFn(MFn::kPluginDependNode))
		{
			Logging::debug(MString("AppleseedRenderer::doInteractiveUpdate - found shader.") + iElement->name);
			MFnDependencyNode depFn(iElement->node);
			if (depFn.typeName() == "asLayeredShader")
			{
				std::shared_ptr<mtap_MayaObject> obj = std::static_pointer_cast<mtap_MayaObject>(iElement->obj);
			}
		}
		if (iElement->node.hasFn(MFn::kMesh))
		{
			std::shared_ptr<mtap_MayaObject> obj = std::static_pointer_cast<mtap_MayaObject>(iElement->obj);
			if (obj->removed)
			{
				continue;
			}

			if (iElement->triggeredFromTransform)
			{
				Logging::debug(MString("AppleseedRenderer::doInteractiveUpdate - found mesh triggered from transform - update instance.") + iElement->name);
				MStatus stat;
				MFnDagNode dn(iElement->node, &stat);
				MDagPath mdp;
				stat = dn.getPath(mdp);
				asr::AssemblyInstance *assInst = getExistingObjectAssemblyInstance(obj.get());
				if (assInst == nullptr)
					continue;

				MMatrix m = mdp.inclusiveMatrix(&stat);
				if (!stat)
					Logging::debug(MString("Error ") + stat.errorString());
				assInst->transform_sequence().clear();
				fillTransformMatices(m, assInst);
				assInst->bump_version_id();
			}
			else{
				// update geo shape
			}
		}
	}
	interactiveUpdateList.clear();
}