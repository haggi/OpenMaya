#ifndef KRAY_RENDERER_H
#define KRAY_RENDERER_H

#include <iostream>
#include <fstream>

#include "kraysdk/proto/direct.h"
#include "kraysdk/symbol/mesh.h"
#include "kraysdk/symbol/vertexmap.h"
#include "kraysdk/type/double.h"
#include "kraysdk/type/vector.h"
#include "kraysdk/type/axes.h"

#include "kraysdk/proto/direct.h"
//#include "kraysdk/SharedSources/libraryDir.h"
#include "kraysdk/kray.h"
#include "kraysdk/proto/offline.h"

#include "krayEventListener.h"
#include <vector>

//#include "kraysdk/proto/direct.h"
//#include "krayEventListener.h"

class mtkr_MayaObject;
class mtkr_MayaScene;
class mtkr_RenderGlobals;
class MObject;

#define KRAY_MATERIAL_ID 0x0011CF4B
//namespace Kray{
//	class DirectPrototyper;
//	class Library;
//	class Instance;
//};

namespace krayRender
{

	class OstreamPrototyper : public Kray::OfflinePrototyper{
		std::ostream& stream;
	public:
		OstreamPrototyper(std::ostream& stream,bool packEnums=true):OfflinePrototyper(packEnums),stream(stream){
		}

		void out(const char* scriptLine){
			stream << scriptLine << std::endl;
		}
	};

	class OstreamPlusDirectPrototyper : public Kray::OfflinePrototyper{
	   std::ostream& stream;
	   Kray::Instance& ki;
	public:
	   OstreamPlusDirectPrototyper(std::ostream& stream,Kray::Instance& ki,bool packEnums=false):stream(stream),ki(ki),OfflinePrototyper(packEnums){
	   }

	   void out(const char* scriptLine){
		  stream << scriptLine << std::endl;
		  ki.parse(scriptLine);
	   }
	};

	class KrayRenderer
	{
	public:
		KrayRenderer();
		~KrayRenderer();
		void definePrototyper();

		mtkr_MayaScene *mtkr_scene;
		mtkr_RenderGlobals *mtkr_renderGlobals;
		//Kray::DirectPrototyper *pro;
		Kray::Prototyper *pro;
		Kray::Instance *kin;
		Kray::Library *kli;
		EventListener listener;
		std::ofstream outStream;
		bool is_good;
		bool good();

		std::vector<mtkr_MayaObject *> interactiveUpdateList;
		std::vector<MObject> interactiveUpdateMOList;
		std::vector<Kray::Symbol *> krayMaterialList; // this is a list of all already defined materials

		void updateTransform(mtkr_MayaObject *obj);	// updates the transform node in renderer
		void updateDeform(mtkr_MayaObject *obj);	// updates the mesh deform node in renderer

		void defineCamera();
		void defineSampling();
		void defineGeometry(mtkr_MayaObject *obj);
		Kray::MeshSymbol *defineMesh(mtkr_MayaObject *obj);
		void writeImageFile(MString fileName);
		void defineObjectMaterial(mtkr_MayaObject *obj);

		void defineFilter();
		void definePixelOrder();
		void defineEnvironment();
		void defineBackground();

		void defineLigths();
		void definePointLight(mtkr_MayaObject *obj);
		void defineDirectionalLight(mtkr_MayaObject *obj);
		void defineSpotLight(mtkr_MayaObject *obj);
		void defineAreaLight(mtkr_MayaObject *obj);

		void render();

	private:
	};
} // namespace

#endif
