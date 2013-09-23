#ifndef MTFU_IMGTOOLS_H
#define MTFU_IMGTOOLS_H

#include <maya/MString.h>
#include <ImathBox.h>
#include <ImfRgba.h>

//static void copy_fb_into_rgba(const float* src, Imf::Rgba* dst, int width, int height, int nchannels);
//static void write_rgba_layer(const char *filename, const Imf::Rgba* pixels, const Imath::Box2i &dispwin, const Imath::Box2i &datawin);
//static Imath::Box2i make_box2i(int *box);

namespace FujiImgTools
{
	class ImgTools
	{
	public:
		ImgTools(){};
		void FrameBufferToExr(MString fileName);
		Imath::Box2i make_box2i(int *box);
		void write_rgba_layer(const char *filename, const Imf::Rgba* pixels, const Imath::Box2i &dispwin, const Imath::Box2i &datawin);
		void copy_fb_into_rgba(const float* src, Imf::Rgba* dst, int width, int height, int nchannels);
	private:
	};

}
#endif
