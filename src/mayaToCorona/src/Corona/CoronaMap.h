#ifndef CORONA_MAP_H
#define CORONA_MAP_H

#include <maya/MRenderUtil.h>
#include <maya/MMatrix.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MColor.h>

#include "CoronaCore/api/Api.h"


bool textureFileSupported(MString fileName);

class mtco_MapLoader : public Corona::Object {
public:

	MColor colorGain;
	MColor colorOffset;
	float  multiplier;

	mtco_MapLoader( MColor gain, MColor offset, float multi)
	{
		colorGain = gain;
		colorOffset = offset;
		multiplier = multi;
	};

	mtco_MapLoader()
	{
		colorGain = MColor(1,1,1);
		colorOffset = MColor(0,0,0);
		multiplier = 1.0f;
	};

    Corona::Abstract::Map* loadBitmap(const Corona::String& filename) 
	{
        Corona::Bitmap<Corona::Rgb> data;
        Corona::loadImage(filename, data);

        class TextureMap : public Corona::Abstract::Map 
		{
        protected:
            Corona::TextureShader shader;
        public:
			MColor colorGain;
			MColor colorOffset;
			float  multiplier;

            TextureMap(const Corona::Bitmap<Corona::Rgb>& data, const int mapChannel) 
			{
                shader.data = data;
                shader.mapChannel = mapChannel;
				colorGain = MColor(1,1,1);
				colorOffset = MColor(0,0,0);
				multiplier = 1.0f;
            }

            virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) 
			{
                outAlpha = 1.f;
				Corona::Rgb result = shader.eval(context);
				result.r() *= colorGain.r; 
				result.r() += colorOffset.r; 
				result.g() *= colorGain.g; 
				result.g() += colorOffset.g; 
				result.b() *= colorGain.b; 
				result.b() += colorOffset.b; 

				result *= multiplier; 

                return result;
            }

            /// \brief Same as evalColor, only scalar value is returned
            virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) 
			{
                outAlpha = 1.f;
                return shader.eval(context).grayValue();
            }

            /// \brief Evaluates bump mapping for this texture and returns a vector that have to be added to the
            ///        current normal to create the bump effect (the normal perturbation)
            virtual Corona::Dir evalBump(const Corona::IShadeContext& context, Corona::TextureCache* cache) 
			{
                STOP;
            }

            /// \brief Renders the map to given output bitmap. Sets the output dimensions to adequate values.
            virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output) 
			{
                shader.renderTo(output);
            }

            virtual Corona::String name() const 
			{
                return "TextureMap";
            }

        };
		TextureMap *tm = new TextureMap(data, 0);
		tm->colorGain = colorGain;
		tm->colorOffset = colorOffset;
		tm->multiplier = multiplier;

        return tm;
    }
};

// Utility class for loading bitmap textures from files. In future, various procedural textures should be also 
// loaded using this class.
class MapLoader : public Corona::Object {
public:
     Corona::Abstract::Map* loadBitmap(const Corona::String& filename) {
        Corona::Bitmap<Corona::Rgb> data;
        Corona::loadImage(filename, data);
		
        class TextureMap : public Corona::Abstract::Map {
        protected:
            Corona::TextureShader shader;
        public:
            TextureMap(const Corona::Bitmap<Corona::Rgb>& data, const int mapChannel) {
                shader.data = data;
                shader.mapChannel = mapChannel;
            }

            virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
                outAlpha = 1.f;
				
                return shader.eval(context);
            }

            /// \brief Same as evalColor, only scalar value is returned
            virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
                outAlpha = 1.f;
                return shader.eval(context).grayValue();
            }

            /// \brief Evaluates bump mapping for this texture and returns a vector that have to be added to the
            ///        current normal to create the bump effect (the normal perturbation)
            virtual Corona::Dir evalBump(const Corona::IShadeContext& context, Corona::TextureCache* cache) {
                STOP;
            }

            /// \brief Renders the map to given output bitmap. Sets the output dimensions to adequate values.
            virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output) {
                shader.renderTo(output);
            }

            virtual Corona::String name() const {
                return "TextureMap";
            }

        };

        return new TextureMap(data, 0);
    }
};


// A sample texture, that just maps the world position to a red-blue grid
class CheckerMap : public Corona::Abstract::Map {
public:
    virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
        //const Corona::Pos pos = context.getPosition()/3;
		const Corona::Pos pos = context.getMapCoords(0) * 10.0f;
        const int tmp = int(floor(pos.x()))+int(floor(pos.y()))+int(floor(pos.z()));
        outAlpha = 1.f;
        if(tmp%2) {
            return Corona::Rgb::RED;
        } else {
            return Corona::Rgb::BLUE;
        }
    }

    virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
        return evalColor(context, cache, outAlpha).grayValue();
    }

    virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*) {
        STOP; //currently not supported
    }

    virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output) {
        STOP; //currently not supported
    }
};

// A sample texture, that just maps the world position to a red-blue grid
class MayaMap : public Corona::Abstract::Map {
public:
    MString otherSidePlugName;
	MayaMap(MString otherSidePlugName){otherSidePlugName = otherSidePlugName;};

	virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) 
	{
		Corona::Dir normal = context.getGeometryNormal();
        const Corona::Pos pos = context.getPosition();
		const Corona::Pos uvw = context.getMapCoords(0);

		MFloatMatrix camMatrix;
		camMatrix.setToIdentity();
		MFloatPointArray pointArray, refPoints;
		MFloatArray uArray, vArray, filterSizes;
		MFloatVectorArray normals, uTangents, vTangents, resultColors, resultTransparencies;
		pointArray.append(pos.x(), pos.y(), pos.z());
		uArray.append(uvw.x());
		vArray.append(uvw.y());
		normals.append(MFloatVector(normal.x(), normal.y(), normal.z()));
		//MRenderUtil::sampleShadingNetwork(otherSidePlugName, numFollicles, false, true, matrix, NULL, &uCoords, &vCoords, NULL, NULL, NULL, NULL, NULL, resultColors, resultTransparencies);

		//MStatus s = MRenderUtil::sampleShadingNetwork(otherSidePlugName, 1, false, true, camMatrix, &pointArray, &uArray, &vArray, &normals, NULL, NULL, NULL, NULL, resultColors, resultTransparencies);
		//if( !s )
		//	MGlobal::displayInfo(MString("sample problem: ") + s.errorString());
		//if( resultColors.length() > 0)
		//{
		//	MFloatVector c(resultColors[0].x, resultColors[0].y, resultColors[0].z);
		//	if( s )
		//		return Corona::Rgb(resultColors[0].x, resultColors[0].y, resultColors[0].z);
		//	else{
		//		MGlobal::displayInfo(MString("sample problem: ") + s.errorString());
		//		return Corona::Rgb(0,0,1);
		//	}
		//}else{
		//	if( !s )
		//		MGlobal::displayInfo(MString("sample problem: ") + s.errorString());
		//	return Corona::Rgb(0,0,1);
		//}
		outAlpha = 1.0f;
		return Corona::Rgb(0,0,1);
    }

    virtual float evalMono(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
        return evalColor(context, cache, outAlpha).grayValue();
    }

    virtual Corona::Dir evalBump(const Corona::IShadeContext&, Corona::TextureCache*) {
        STOP; //currently not supported
    }

    virtual void renderTo(Corona::Bitmap<Corona::Rgb>& output) {
        STOP; //currently not supported
    }
};

#endif
