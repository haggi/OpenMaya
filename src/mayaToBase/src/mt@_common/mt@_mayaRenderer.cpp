#include "mt@_mayaRenderer.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "world.h"

#if MAYA_API_VERSION >= 201600

#include <vector>
#include <maya/MPxRenderer.h>
#include <maya/MGlobal.h>

#define kNumChannels 4

float FLT_RAND() {
	return float(rand()) / RAND_MAX;
}

mt@_MayaRenderer::mt@_MayaRenderer()
{
	width = height = initialSize;
	renderBuffer = (float*)malloc(width*height*kNumChannels*sizeof(float));
	good = true;
}

mt@_MayaRenderer::~mt@_MayaRenderer()
{
	if (!good)return;
}

bool mt@_MayaRenderer::isRunningAsync()
{
	Logging::debug("isRunningAsync");
	return true;
}

void* mt@_MayaRenderer::creator()
{
	return new mt@_MayaRenderer();
}

void mt@_MayaRenderer::render()
{
	Logging::debug("Rendering...");
	ProgressParams progressParams;
	progressParams.progress = 0.0f;
	progress(progressParams);
	isRendering = true;
	--renderFrame();
	isRendering = false;
	progressParams.progress = 1.0f;
	progress(progressParams);
}

static void startRenderThread(mt@_MayaRenderer* renderPtr)
{
	renderPtr->render();
}

static void startFbThread(mt@_MayaRenderer* renderPtr)
{
	if (!renderPtr->good)return;
	while (renderPtr->isRendering)
	{
		Logging::debug("FbThread sleeping...");
		std::this_thread::sleep_for(std::chrono::milliseconds(renderPtr->refreshInteraval * 1000));
		renderPtr->framebufferCallback();
	}
}

void mt@_MayaRenderer::finishRendering()
{
	if (good)
		cancelRender();
	if (this->renderThread.joinable())
		this->renderThread.join();
	if (this->fbThread.joinable())
		this->fbThread.join();
}

MStatus mt@_MayaRenderer::startAsync(const JobParams& params)
{
	//mt@_MayaRenderer::running = true;
	//render();
	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::stopAsync()
{
	Logging::debug("stopAsync");
	finishRendering();
	return MS::kSuccess;
}

MStatus mt@_MayaRenderer::beginSceneUpdate()
{
	Logging::debug("beginSceneUpdate");
	finishRendering();
	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::endSceneUpdate()
{
	Logging::debug("endSceneUpdate");
	completlyInitialized = true;
	this->renderThread = std::thread(startRenderThread, this);
	this->fbThread = std::thread(startFbThread, this);
	return MStatus::kSuccess;
};

static float demoRot = 0.0f;

MStatus mt@_MayaRenderer::translateMesh(const MUuid& id, const MObject& node)
{
	Logging::debug("translateMesh");

	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "mesh";
	objectArray.push_back(idn);
	lastShape = node;

	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::translateLightSource(const MUuid& id, const MObject& node)
{
	Logging::debug("translateLightSource");

	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "light";
	objectArray.push_back(idn);
	lastShape = node;

	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::translateCamera(const MUuid& id, const MObject& node)
{
	Logging::debug("translateCamera");
	IdNameStruct idn;
	idn.id = id;
	idn.mobject = node;
	idn.name = "camera";
	objectArray.push_back(idn);
	lastShape = node;
	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::translateEnvironment(const MUuid& id, EnvironmentType type)
{
	Logging::debug("translateEnvironment");
	IdNameStruct idn;
	idn.id = id;
	idn.name = "environment";
	objectArray.push_back(idn);
	lastShape = MObject::kNullObj;
	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::translateTransform(const MUuid& id, const MUuid& childId, const MMatrix& matrix)
{
	Logging::debug("translateTransform");
	IdNameStruct ids;
	ids.id = id;
	ids.name = "transform";
	objectArray.push_back(ids);

	ids.id = childId;
	ids.name = "transformChild";
	objectArray.push_back(ids);
	return MStatus::kSuccess;

	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::translateShader(const MUuid& id, const MObject& node)
{
	Logging::debug(MString("translateShader: "));
	bool alreadyExists = false;
	for (auto idn : objectArray)
	{
		if ( idn.mobject == node)
		{ 
			Logging::debug(MString("found existing shader node."));
			if (idn.instance != nullptr)
			{
				alreadyExists = true;
				break;
			}
		}
	}
	if (!alreadyExists)
	{
		IdNameStruct idn;
		idn.id = id;
		idn.name = "material";
		idn.mobject = node;
		objectArray.push_back(idn);
	}
	lastShape = node;
	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::setProperty(const MUuid& id, const MString& name, bool value)
{
	Logging::debug(MString("setProperty bool: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::setProperty(const MUuid& id, const MString& name, int value)
{
	Logging::debug(MString("setProperty int: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::setProperty(const MUuid& id, const MString& name, float value)
{
	Logging::debug(MString("setProperty float: ") + name + " " + value);
	return MStatus::kSuccess;
};
MStatus mt@_MayaRenderer::setProperty(const MUuid& id, const MString& name, const MString& value)
{
	mt@_FileLoader fl;
	Logging::debug(MString("setProperty string: ") + name + " " + value);

	MString mayaRoot = getenv("MAYA_LOCATION");
	MString blackImage = mayaRoot + "/presets/Assets/IBL/black.exr";

	IdNameStruct idNameObj;
	for (auto idobj : objectArray)
	{
		if (idobj.id == id)
		{
			Logging::debug(MString("Found id object for string property: ") + idobj.name);
			if (idobj.name == "environment")
			{
				if (name == "imageFile")
				{
					Logging::debug(MString("Setting environment image file to: ") + value);
					if (value.length() == 0)
						//
					else
					//
				}
			}
		}
	}

	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::setShader(const MUuid& id, const MUuid& shaderId)
{
	Logging::debug("setShader");
	IdNameStruct *idnShader;
	MObject meshObject;

	// first find the shader node
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == shaderId)
		{
			idnShader = &objectArray[i];
			break;
		}
	}
	
	IdNameStruct idnGeo;
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == id)
		{
			idnGeo = objectArray[i];
			break;
		}
	}

	if ((idnShader->mobject == MObject::kNullObj) || (idnGeo.mobject == MObject::kNullObj))
	{
		Logging::debug(MString("Object not fund for shader update."));
		return MS::kFailure;
	}


	// first find the shader node
	bool alreadyExists = false;
	for (uint i = 0; i < objectArray.size(); i++)
	{
		if (objectArray[i].id == id)
		{
			if (objectArray[i].name == "coronaMaterial")
			{
				objectArray[i].material = mat;
				alreadyExists = true;
				break;
			}
		}
	}
	if (!alreadyExists)
	{
		IdNameStruct idn;
		idn.id = id;
		idn.name = "coronaMaterial";
		idn.mobject = idnGeo.mobject;
		idn.material = mat;
		objectArray.push_back(idn);
	}
	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::setResolution(unsigned int w, unsigned int h)
{
	Logging::debug(MString("setResolution ") + w + " " + h);
	this->width = w;
	this->height = h;
	this->renderBuffer = (float*)realloc(this->renderBuffer, w*h*kNumChannels*sizeof(float));

	if (good)
	{
	}
	return MStatus::kSuccess;
};

MStatus mt@_MayaRenderer::destroyScene()
{
	Logging::debug("destroyScene");
	finishRendering();
	ProgressParams progressParams;
	progressParams.progress = -1.0f;
	progress(progressParams);
	objectArray.clear();
	return MStatus::kSuccess;
};
bool mt@_MayaRenderer::isSafeToUnload()
{
	Logging::debug("isSafeToUnload");
	return true;
};

void mt@_MayaRenderer::framebufferCallback()
{
	Logging::debug("framebufferCallback");


	refreshParams.bottom = 0;
	refreshParams.top = height - 1;
	refreshParams.bytesPerChannel = sizeof(float);
	refreshParams.channels = kNumChannels;
	refreshParams.left = 0;
	refreshParams.right = width - 1;
	refreshParams.width = width;
	refreshParams.height = height;
	refreshParams.data = renderBuffer;
	refresh(refreshParams);
}
#endif
