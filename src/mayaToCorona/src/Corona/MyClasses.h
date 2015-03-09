#ifndef MYCLASSES_H
#define MYCLASSES_H

#include "CoronaCore/api/Api.h"
#include <map>

float FLT_RAND() {
    return float(rand())/RAND_MAX;
}


// Basic settings implementation - stores all settings in a std::map. Real plugin implementation would probably
// want to utilize some system for settings management in a host 3D application for a subset of the settings (the 
// ones exposed to the user)
class MySettings : public Corona::Abstract::Settings {
protected:
    std::map<int, Corona::Abstract::Settings::Property> values;
public:
    virtual Corona::Abstract::Settings::Property get(const int id) const {
        const std::map<int, Corona::Abstract::Settings::Property>::const_iterator result = values.find(id);
        if(result != values.end()) {
            return result->second;
        } else {
			throw Corona::Exception::propertyNotFound(Corona::PropertyDescriptor::get(id)->name);
        }
    }
    virtual void set(const int id, const Corona::Abstract::Settings::Property& property) {
        values[id] = property;
    }
};


// A sample texture, that just maps the world position to a red-blue grid
class MyCheckerMap : public Corona::Abstract::Map {
public:
    virtual Corona::Rgb evalColor(const Corona::IShadeContext& context, Corona::TextureCache* cache, float& outAlpha) {
        //const Corona::Pos pos = context.getPosition()/3;
		const Corona::Pos pos = context.getMapCoords(0);
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

	virtual void getChildren(Corona::Stack<Corona::Resource*>&) {}
};


#endif