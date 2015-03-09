#include "Corona.h"
#include <maya/MPlugArray.h>
#include "renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"
#include "utilities/attrTools.h"
#include "world.h"

// passes types
//0  "Alpha", - f
//1  "SourceColor", - c
//2  "Components", - c
//3  "Shadows", - c
//4  "Albedo", - c
//5  "RawComponent", - c
//6  "Normals", - c
//7  "NormalsDotProduct", - f
//8  "NormalsDiscrepancy", - f
//9  "PrimitiveCoords", - c
//10 "MapCoords", - c
//11 "Velocity", - c
//12 "ZDepth", - f
//13 "WorldPosition", - c
//14 "Id", - f
//15 "Texmap" - c


void CoronaRenderer::getPassesInfo(std::vector<MayaToRenderPass>& passes)
{
	Logging::debug("getPassesInfo");
	MFnDependencyNode globalsNode(objectFromName("coronaGlobals"));
	MPlug passesPlug = globalsNode.findPlug("AOVs");
	if (passesPlug.isNull())
		return;
	MObjectArray oa;
	for (uint i = 0; i < passesPlug.numElements(); i++)
	{
		MPlugArray tmpArray;
		passesPlug[i].connectedTo(tmpArray, true, false);
		if (tmpArray.length() == 0)
			continue;
		Logging::debug(MString("Found connected pass node for: ") + tmpArray[0].name());
		oa.append(tmpArray[0].node());
	}
	if (oa.length() == 0)
		return;

	for (uint i = 0; i < oa.length(); i++)
	{
		MObject passObj = oa[i];
		MFnDependencyNode passNode(passObj);
		Logging::debug(MString("PassNode: ") + passNode.name());
		if (passNode.typeName() != "coronaPassesNode")
			continue;
		MayaToRenderPass rp;
		rp.passType = (MayaToRenderPass::MayaToPassType)passNode.findPlug("dataType").asShort();
		rp.passName = passNode.name().asChar();
		rp.channelType = MayaToRenderPass::COLOR;
		int passTypeId = passNode.findPlug("passType").asInt();
		if ((passTypeId == 0) || (passTypeId == 7) || (passTypeId == 8) || (passTypeId == 12) || (passTypeId == 14))
			rp.channelType = MayaToRenderPass::SINGLE;
		passes.push_back(rp);
	}
}

void CoronaRenderer::saveMergedExr(Corona::String filename)
{
	MFnDependencyNode depFn(getRenderGlobalsNode());

	int width = this->context.fb->getImageSize().x;
	int height = this->context.fb->getImageSize().y;

	std::vector<MayaToRenderPass> passes;
	// add default beauty render pass
	MayaToRenderPass rpass;
	rpass.channelType = MayaToRenderPass::COLOR;
	rpass.passName = "Beauty";
	MFnDependencyNode globalsNode(objectFromName("coronaGlobals"));
	rpass.passType = MayaToRenderPass::FLOAT;
	if (globalsNode.findPlug("exrDataTypeHalf").asBool())
		rpass.passType = MayaToRenderPass::HALF;
	passes.push_back(rpass);
	getPassesInfo(passes);
	int numPasses = passes.size();
	int numChannels = 0;
	int passChannels = 0;
	uint pixelSize = 0;
	for (int passId = 0; passId < numPasses; passId++)
	{
		passChannels = 0;
		MayaToRenderPass &rp = passes[passId];
		rp.coronaRgbScanline = new Corona::Rgb[width];
		rp.coronaFloatScanline = new float[width];

		if (rp.channelType == MayaToRenderPass::COLOR)
		{
			passChannels += 3;
			if (MString(rp.passName) == "Beauty")
				passChannels++;
		}
		if (rp.channelType == MayaToRenderPass::SINGLE)
		{
			passChannels += 1;
		}

		if (rp.passType == MayaToRenderPass::FLOAT)
			pixelSize += passChannels * sizeof(float);
		if (rp.passType == MayaToRenderPass::HALF)
			pixelSize += passChannels * sizeof(half);

		numChannels += passChannels;
	}

	int numCoronaPasses = this->context.renderPasses.size();
	for (int coronaPassId = 0; coronaPassId < numCoronaPasses; coronaPassId++)
	{
		Corona::IRenderPass *crp = this->context.renderPasses[coronaPassId];
		for (int passId = 0; passId < numPasses; passId++)
		{
			MayaToRenderPass &rp = passes[passId];
			if (MString(rp.passName) == crp->getName().cStr())
			{
				rp.coronaPassId = coronaPassId;
			}
		}
	}
	for (int passId = 0; passId < numPasses; passId++)
	{
		MayaToRenderPass &rp = passes[passId];
		if (MString(rp.passName) == "Beauty")
		{
			rp.coronaPassId = -1;
		}
		if (MString(rp.passName) == "Alpha")
		{
			rp.coronaPassId = -2;
		}
	}

	//std::string fileName = "C:/daten/3dprojects/mayaToCorona/images/exrTestImage.exr";
	std::string fileName = filename.cStr();
	
	OIIO::ImageOutput *out = OIIO::ImageOutput::create(fileName);
	OIIO::ImageSpec spec(width, height, numChannels, OIIO::TypeDesc::FLOAT);

	for (int passId = 0; passId < numPasses; passId++)
	{
		int channelNum = 0;
		MayaToRenderPass &rp = passes[passId];
		if (rp.channelType == MayaToRenderPass::COLOR)
			channelNum = 3;
		if (rp.channelType == MayaToRenderPass::SINGLE)
			channelNum = 1;
		if (MString(rp.passName) == "Beauty")
			channelNum++; //default alpha

		if (rp.passType == MayaToRenderPass::FLOAT)
			for (int chId = 0; chId < channelNum; chId++)
				spec.channelformats.push_back(OIIO::TypeDesc::FLOAT);

		if (rp.passType == MayaToRenderPass::HALF)
			for (int chId = 0; chId < channelNum; chId++)
				spec.channelformats.push_back(OIIO::TypeDesc::HALF);
	}

	spec.channelnames.clear();
	for (int passId = 0; passId < numPasses; passId++)
	{
		MayaToRenderPass &rp = passes[passId];
		if (MString(rp.passName) == "Beauty")
		{
			Logging::debug("Adding rgba for beauty");
			spec.channelnames.push_back("R");
			spec.channelnames.push_back("G");
			spec.channelnames.push_back("B");
			spec.channelnames.push_back("A");
			continue;
		}
		if (rp.coronaPassId == 12) // special case Z cannel == ZDepth
		{
			spec.channelnames.push_back("Z");
			continue;
		}
		if (rp.channelType == MayaToRenderPass::COLOR)
		{
			Logging::debug(MString("Adding color channel for ") + rp.passName);
			spec.channelnames.push_back(std::string(rp.passName) + ".r");
			spec.channelnames.push_back(std::string(rp.passName) + ".g");
			spec.channelnames.push_back(std::string(rp.passName) + ".b");
			continue;
		}
		if (rp.channelType == MayaToRenderPass::SINGLE)
		{
			Logging::debug(MString("Adding single channel for ") + rp.passName);
			spec.channelnames.push_back(std::string(rp.passName) + ".a");
			continue;
		}
	}

	std::string s = "mayaToCorona";
	spec.attribute("created by", s);

	// get scanlines
	bool doToneMapping = true;
	bool showRenderStamp = getBoolAttr("renderstamp_inFile", depFn, true);
	int exrScanlineSize = pixelSize * width;
	char *scanLine = new char[exrScanlineSize];
	int z = 0;
	out->open(fileName.c_str(), spec);


	for (int y = (height - 1); y >= 0; y--)
	{
		int scanLineOffset = 0;
		memset(scanLine, 0, exrScanlineSize);
		char *currentPos = scanLine;

		// first we retrieve the scanlines from all passes
		for (int passId = 0; passId < numPasses; passId++)
		{
			MayaToRenderPass &rp = passes[passId];
			this->context.fb->getRow(Corona::Pixel(0, y), width, rp.coronaPassId, doToneMapping, showRenderStamp, rp.coronaRgbScanline, rp.coronaFloatScanline);
		}

		// all passes are filled now we fill the exr scanLine
		for (int x = 0; x < width; x++)
		{
			for (int passId = 0; passId < numPasses; passId++)
			{
				MayaToRenderPass &rp = passes[passId];
				if (rp.channelType == MayaToRenderPass::COLOR)
				{
					MColor c(rp.coronaRgbScanline[x].r(), rp.coronaRgbScanline[x].g(), rp.coronaRgbScanline[x].b());
					float f[3];
					c.get(f);
					if (rp.passType == MayaToRenderPass::FLOAT)	
					{ 
						memcpy(currentPos, f, sizeof(float)* 3);
						float *tst = (float *)currentPos;
						currentPos += sizeof(float)* 3;
					}
					if (rp.passType == MayaToRenderPass::HALF)
					{
						half h[3];
						h[0] = c.r;
						h[1] = c.g;
						h[2] = c.b;
						memcpy(currentPos, h, sizeof(half)* 3);
						currentPos += sizeof(half)* 3;
					}
				}
				// default alpha channel
				if (MString(rp.passName) == "Beauty")
				{
					float f = rp.coronaFloatScanline[x];
					if (rp.passType == MayaToRenderPass::FLOAT)
					{
						memcpy(currentPos, &f, sizeof(float));
						currentPos += sizeof(float);
					}
					if (rp.passType == MayaToRenderPass::HALF)
					{
						half h = f;
						*currentPos = h;
						//memcpy(currentPos, &h, sizeof(half));
						currentPos += sizeof(half);
					}
				}

				if (rp.channelType == MayaToRenderPass::SINGLE)
				{
					// corona uses rgb for single channel types, all
					// three channels have the same value
					float f = rp.coronaRgbScanline[x].r();
					if (rp.passType == MayaToRenderPass::FLOAT)
					{
						memcpy(currentPos, &f, sizeof(float));
						currentPos += sizeof(float);
					}
					if (rp.passType == MayaToRenderPass::HALF)
					{
						half h = f;
						memcpy(currentPos, &h, sizeof(half));
						currentPos += sizeof(half);
					}
				}
			}
		}
		
		int yReverse = (height - 1) - y;
		bool result = out->write_scanline(yReverse, z, OIIO::TypeDesc::UNKNOWN, scanLine);
		if (!result)
			Logging::debug("Problem writing OIIO scanline.");
	}

	out->close();

	delete out;
	delete[] scanLine;

}

MString CoronaRenderer::getImageFileName(MString& basename, MString& restname)
{
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;
	renderGlobals->getImageName();
	MString filename = renderGlobals->imageOutputFile.asChar();
	Logging::debug(MString("Saving image as ") + renderGlobals->imageOutputFile);
	std::string imgFormatExt = renderGlobals->imageOutputFile.toLowerCase().asChar();
	std::vector<std::string> fileParts;

	// we simply expect a filename with format: <name>.number.ext or <name>.ext where name must not contain points.
	int numPoints = pystring::count(imgFormatExt, ".");
	pystring::split(imgFormatExt, fileParts, ".");
	std::string name = fileParts.front();
	basename = name.c_str();
	std::vector<std::string> restArray;
	for (size_t i = 1; i < fileParts.size(); i++)
		restArray.push_back(fileParts[i]);
	std::string rest = "." + pystring::join(".", restArray);
	restname = rest.c_str();
	std::string ext = fileParts.back();
	if ((ext != "exr") && (ext != "png") && (ext != "bmp") && (ext != "jpg"))
	{
		Logging::warning(MString("Filename does not contain a valid extension: ") + renderGlobals->imageOutputFile + " adding exr.");
		filename += ".exr";
		ext = "exr";
	}
	return filename;
}

void CoronaRenderer::saveImage()
{
	MFnDependencyNode depFn(getRenderGlobalsNode());
	std::shared_ptr<RenderGlobals> renderGlobals = MayaTo::getWorldPtr()->worldRenderGlobalsPtr;

	Corona::Bitmap<Corona::Rgb, false> bitmap(this->context.fb->getImageSize());
	Corona::Bitmap<float, false> alpha(this->context.fb->getImageSize());

	bool doToneMapping = true;
	bool showRenderStamp = getBoolAttr("renderstamp_inFile", depFn, true);
	MString name, rest;
	Corona::String filename = getImageFileName(name, rest).asChar();
	bool isExr = pystring::endswith(filename.cStr(), ".exr");

	Corona::String dumpFilename = (renderGlobals->imageOutputFile + ".dmp").asChar();
	if (getBoolAttr("dumpAndResume", depFn, false))
		this->context.fb->dumpExr(dumpFilename);

	// no gamma for exr
	bool isLinear = isExr;
	if (isExr)
		context.colorMappingData->gamma = 1.0f;

	this->context.fb->setColorMapping(*context.colorMappingData);
	// first write the beauty
	for (int i = 0; i < bitmap.getHeight(); ++i)
	{
		const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
		this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, &bitmap[pixel], &alpha[pixel]);
	}

	if (isExr)
	{
		if (getBoolAttr("exrMergeChannels", depFn, true))
		{
			saveMergedExr(filename);
			context.colorMappingData->gamma = getFloatAttr("colorMapping_gamma", depFn, 2.2);
			return;
		}
	}

	Corona::saveImage(filename, Corona::RgbBitmapIterator<false>(bitmap, &alpha), isLinear, Corona::IMAGE_DETERMINE_FROM_EXT);

	int numPasses = this->context.renderPasses.size();
	Corona::Iterator<Corona::IRenderPass *> passIter;
	for (int passId = 0; passId < numPasses; passId++)
	{
		Corona::IRenderPass *rp = this->context.renderPasses[passId];
		Logging::debug(MString("Renderpass ") + rp->getName().cStr());
		for (int i = 0; i < bitmap.getHeight(); ++i)
		{
			const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
			this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), passId, doToneMapping, showRenderStamp, &bitmap[pixel], &alpha[pixel]);
		}
		filename = std::string(name.asChar()) + "_" + rp->getName().cStr() + rest.asChar();
		Corona::saveImage(filename, Corona::RgbBitmapIterator<false>(bitmap, &alpha), isLinear, Corona::IMAGE_DETERMINE_FROM_EXT);
	}

}

