#ifndef CORONA_MAP_H
#define CORONA_MAP_H

#include "CoronaCore/api/Api.h"

// Utility class for loading bitmap textures from files. In future, various procedural textures should be also 
// loaded using this class.
class MapLoader : public Corona::Object {
public:
    
    /// \brief Creates a TextureMap (child class of Abstract::Map) from a texture with given filename.
    /// \param isreflectance Specifies if the loaded TextureMap should return reflectance values (e.g. diffuse 
    ///                      color), or radiance values (e.g. emission color)
    Corona::Abstract::Map* loadBitmap(const Corona::String& filename) {
        Corona::Bitmap<Corona::Rgb> data;
        Corona::Wx::loadImage(filename, data);

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

#endif
