#ifndef MAYATO_WORLD_H
#define MAYATO_WORLD_H

#include <vector>
#include <memory>
#include <maya/MFnDependencyNode.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MImage.h>
#include <maya/MTimerMessage.h>
#include <maya/MDistance.h>
#include "utilities/tools.h"
#include "rendering/renderer.h"

class MayaScene;
class RenderGlobals;

namespace MayaTo{

	class MayaToWorld
	{
	public:
		MayaToWorld();
		virtual ~MayaToWorld();

		enum WorldRenderType{
			RTYPENONE = 0,
			SWATCHRENDER,
			UIRENDER,
			BATCHRENDER,
			IPRRENDER
		};
		enum WorldRenderState{
			RSTATENONE = 0,
			RSTATERENDERING,
			RSTATESWATCHRENDERING,
			RSTATESTOPPED,
			RSTATEERROR,
			RSTATEDONE,
			RSTATETRANSLATING
		};

		WorldRenderType renderType;
		WorldRenderState renderState;

		enum RendererUpAxis{
			XUp,
			YUp,
			ZUp
		};

		MDistance::Unit internalUnit;
		MDistance::Unit rendererUnit;
		RendererUpAxis internalAxis;
		RendererUpAxis rendererAxis;

		float internalScaleFactor = 1.0f;
		float rendererScaleFactor = 1.0f;
		float toMillimeters(float mm);
		MMatrix globalConversionMatrix; // for default unit conversion e.g. centimeter to meter
		MMatrix sceneScaleMatrix; // user defined scene scale
		float scaleFactor = 1.0f;
		float sceneScale = 1.0f;

		void defineGlobalConversionMatrix();

		virtual void setRendererUnit();
		virtual void setRendererAxis();

		std::shared_ptr<MayaScene> worldScenePtr;
		std::shared_ptr<Renderer> worldRendererPtr;
		std::shared_ptr<RenderGlobals> worldRenderGlobalsPtr;

		MStringArray objectNames;
		std::vector<void *> objectPtr;
		MImage previousRenderedImage;
		bool _canDoIPR = false;
		bool canDoIPR(){ return _canDoIPR; }
		void setCanDoIPR(bool yesOrNo) { _canDoIPR = yesOrNo; };

		MStringArray shaderSearchPath;

		void *getObjPtr(MString name)
		{
			for (uint i = 0; i < objectNames.length(); i++)
				if (objectNames[i] == name)
					return objectPtr[i];
			return nullptr;
		}

		void addObjectPtr(MString name, void *ptr)
		{
			objectNames.append(name);
			objectPtr.push_back(ptr);
		}

		void initialize();
		void initializeScene();
		void initializeRenderer();
		void initializeRenderGlobals();
		void initializeRenderEnvironment();

		void setRenderType(WorldRenderType type)
		{
			this->renderType = type;
		}

		WorldRenderType getRenderType()
		{
			return this->renderType;
		}

		void setRenderState(WorldRenderState state)
		{
			this->renderState = state;
		}

		WorldRenderState getRenderState()
		{
			return this->renderState;
		}		

		void cleanUp();
		void cleanUpAfterRender();
		void afterOpenScene();
		void afterNewScene();

		static void beforeExitCallback(void *);
		static void callAfterOpenCallback(void *);
		static void callAfterNewCallback(void *);
		static MCallbackId afterOpenCallbackId;
		static MCallbackId afterNewCallbackId;
	};

	void *getObjPtr(MString name);
	static void addObjectPtr(MString name, void *ptr);

	void deleteWorld();
	void defineWorld();
	MayaToWorld *getWorldPtr();

	static MayaToWorld *worldPointer = nullptr;

	struct CmdArgs{
		CmdArgs()
		{
			MFnDependencyNode defaultGlobals(objectFromName("defaultResolution"));
			width = defaultGlobals.findPlug("width").asInt();
			height = defaultGlobals.findPlug("height").asInt();
			renderType = MayaToWorld::WorldRenderType::UIRENDER;
			useRenderRegion = false;
		}
		~CmdArgs(){
			std::cout << "Cmd args are deleted\n";
		}
		int width;
		int height;
		bool useRenderRegion;
		MDagPath cameraDagPath;
		MayaToWorld::WorldRenderType renderType;
		int userDataInt = 0;
		int userEvent = -1;
		double userDataFloat = 0.0;
		MString userDataString;
	};

};


#endif // !MAYATO_WORLD_H
