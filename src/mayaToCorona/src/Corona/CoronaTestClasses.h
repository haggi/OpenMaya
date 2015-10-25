#pragma once

// Single include to include the Corona API. 
// The API include folder (where CoronaCore folder is) has to be in the additional include paths of the compiler. 
// Because of that we can include without specifying the path to the APi
#include "CoronaCore/api/Api.h"
#include <map>
#include <iostream>
using namespace Corona;

// generates single floating point random number between 0 and 1
float FLT_RAND() {
	return float(rand()) / RAND_MAX;
}

// Simple implementation of the Logger class from the API. Simply outputs all messages to the standard output.
class MyLogger : public Abstract::Logger {
public:

	MyLogger(ICore* core) : Abstract::Logger(&core->getStats()) {}

	virtual void logMsg(const Corona::LogMessage& message)
	{
#ifdef _DEBUG
		std::cout << message.text.cStr() << std::endl;
#endif
	}
	virtual void setProgress(const float progress) {
		std::cout << "Progress: " << progress << std::endl;
	}
};

// Basic settings implementation - stores all settings in a std::map. Real plugin implementation would probably
// want to utilize some system for settings management in a host 3D application for a subset of the settings (the 
// ones exposed to the user)
class MySettings : public Abstract::Settings {
protected:
	std::map<int, Abstract::Settings::Property> values;
public:
	virtual Abstract::Settings::Property get(const int id) const {
		const std::map<int, Abstract::Settings::Property>::const_iterator result = values.find(id);
		if (result != values.end()) {
			return result->second;
		}
		else {
			throw Exception::propertyNotFound(PropertyDescriptor::get(id)->name);
		}
	}
	virtual void set(const int id, const Abstract::Settings::Property& property) {
		values[id] = property;
	}

};

// Callbacks for handling user actions in the wx frame buffer.
class MyVfbCallbacks : public Abstract::VfbCallbacks {
public:
	ICore* core;
	bool isWindowClosed;

	MyVfbCallbacks() {
		isWindowClosed = false;
	}
	virtual void windowClosed() {
		this->isWindowClosed = true;
		core->cancelRender();
		core->getWxVfb().renderFinished();
	}
	virtual void renderStop(const bool) {
		core->cancelRender();
		core->getWxVfb().renderFinished();
	}
	virtual void renderStart(const bool interactive) {}
	virtual void renderPause() {}
	virtual void duplicate(const int selectedChannelId) {}
	virtual void save(const int selectedChannelId) {}

	virtual wxMenuBar* buildMenu() {
		return NULL;
	}
	virtual void menuCommand(const int menuId, const bool checked) {}
	virtual void sidebarActivated(const bool activated) {}
	virtual void sidebarTabChanged(const WxVfbTab tab) {}
	virtual void textEditFocus() {}
	virtual void textEditDefocus() {}
	virtual void colorMappingChanged(const ColorMappingData& newValues) {}
	virtual void saveAll() {}
	virtual void canvasSizeChanged(const Pixel newSize) {}
	virtual bool zoomingEnabled() {
		return true;
	}
};

// A sample texture, that just maps the world position to a red-blue grid
class MyCheckerMap : public Abstract::Map {
public:
	virtual Rgb evalColor(const IShadeContext& context, TextureCache* cache, float& outAlpha) {
		const Pos pos = context.getPosition() *2.f;
		const int tmp = int(floor(pos.x())) + int(floor(pos.y())) + int(floor(pos.z()));
		outAlpha = 1.f;
		if (tmp % 2) {
			return Rgb::RED;
		}
		else {
			return Rgb::BLUE;
		}
	}

	virtual float evalMono(const IShadeContext& context, TextureCache* cache, float& outAlpha) {
		return evalColor(context, cache, outAlpha).grayValue();
	}

	virtual Dir evalBump(const IShadeContext&, TextureCache*) {
		STOP; //currently not supported
	}

	virtual void renderTo(Bitmap<Rgb>& output) {
		STOP; //currently not supported
	}
	virtual void getChildren(Stack<Resource*>& output) { }
};

// advanced example - custom light shader
class MyLight : public Abstract::LightShader, public Noncopyable {
public:

	const Pos LIGHT_POS;
	const Dir LIGHT_DIR;

	MyLight() : LIGHT_POS(Pos(-3, -3, -7)), LIGHT_DIR(Dir(1, 1, -1.f).getNormalized()) { }

	// simulate spot light with colorful directional falloff
	virtual BsdfComponents getIllumination(IShadeContext& context, Spectrum& transport) const {
		const Pos surface = context.getPosition();
		float distance;
		const Dir toLight = (LIGHT_POS - surface).getNormalized(distance); // direction + distance to light

		const float cosAngle = Utils::max(0.f, -dot(toLight, LIGHT_DIR));
		// calculate the light emission: divide by d^2 to get physical attenuation
		const Spectrum emitted = 50 * cosAngle* Spectrum::max(Spectrum::BLACK, Spectrum(cosAngle * 2 - 1, 1 - 2 * abs(cosAngle - 0.5f), (1 - cosAngle) * 2 - 1)) / (distance*distance);

		// shadow transmission, including occlusion effects
		transport = context.shadowTransmission(LIGHT_POS, RAY_NORMAL);
		float dummy;

		// bsdf value: amount of light incident from light reflected to camera/previous point
		BsdfComponents brdf;
		context.forwardBsdfCos(toLight, brdf, dummy);

		return brdf * (PI*emitted);
	}
};
