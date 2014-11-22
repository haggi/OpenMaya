#include "Corona.h"
#include "../mtco_common/mtco_renderGlobals.h"
#include "utilities/logging.h"
#include "utilities/pystring.h"

static Logging logger;

void CoronaRenderer::saveImage()
{
	Corona::Bitmap<Corona::Rgb, false> bitmap(this->context.fb->getImageSize());
	Corona::Bitmap<float, false> alpha(this->context.fb->getImageSize());

	bool doToneMapping = true;
	bool showRenderStamp = this->mtco_renderGlobals->renderstamp_inFile;

	this->mtco_renderGlobals->getImageName();
	Corona::String filename = this->mtco_renderGlobals->imageOutputFile.asChar();
	logger.debug(MString("Saving image as ") + this->mtco_renderGlobals->imageOutputFile);
	std::string imgFormatExt = this->mtco_renderGlobals->imageOutputFile.toLowerCase().asChar();
	std::vector<std::string> fileParts;

	// we simply expect a filename with format: <name>.number.ext or <name>.ext where name must not contain points.
	int numPoints = pystring::count(imgFormatExt, "."); 
	pystring::split(imgFormatExt, fileParts, ".");
	std::string name = fileParts.front();
	std::vector<std::string> restArray;
	for (size_t i = 1; i < fileParts.size(); i++)
		restArray.push_back(fileParts[i]);
	std::string rest = "." + pystring::join(".", restArray);
	std::string ext = fileParts.back();
	if ((ext != "exr") && (ext != "png") && (ext != "bmp") && (ext != "jpg"))
	{
		logger.warning(MString("Filename does not contain a valid extension: ") + this->mtco_renderGlobals->imageOutputFile + " adding exr.");
		filename += ".exr";
		ext = "exr";
	}

	bool isExr = ext == "exr";

	Corona::String dumpFilename = (this->mtco_renderGlobals->imageOutputFile + ".dmp").asChar();
	if (this->mtco_renderGlobals->dumpAndResume)
		this->context.fb->dumpExr(dumpFilename);

	// no gamma for exr
	bool isLinear = isExr;
	if (isExr)
		context.colorMappingData->gamma = 1.0f;

	// first write the beauty
	for (int i = 0; i < bitmap.getHeight(); ++i)
	{
		const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
		this->context.fb->setColorMapping(*context.colorMappingData);
		this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), Corona::CHANNEL_BEAUTY, doToneMapping, showRenderStamp, &bitmap[pixel], &alpha[pixel]);
	}

	Corona::saveImage(filename, Corona::RgbBitmapIterator<false>(bitmap, &alpha), isLinear, Corona::IMAGE_DETERMINE_FROM_EXT);

	logger.debug("----------------- Render pass data -------------------");
	int numPasses = this->context.renderPasses.size();
	Corona::Iterator<Corona::IRenderPass *> passIter;
	for (int passId = 0; passId < numPasses; passId++)
	//for (passIter = this->context.renderPasses.begin(); passIter != this->context.renderPasses.end(); passIter++)
	{
		Corona::IRenderPass *rp = this->context.renderPasses[passId];
		logger.debug(MString("Renderpass ") + rp->getName().cStr());
		for (int i = 0; i < bitmap.getHeight(); ++i)
		{
			const Corona::Pixel pixel(0, bitmap.getHeight() - 1 - i);
			this->context.fb->setColorMapping(*context.colorMappingData);
			this->context.fb->getRow(Corona::Pixel(0, i), bitmap.getWidth(), passId, doToneMapping, showRenderStamp, &bitmap[pixel], &alpha[pixel]);
		}
		filename = name + "_" + rp->getName().cStr() + rest;
		Corona::saveImage(filename, Corona::RgbBitmapIterator<false>(bitmap, &alpha), isLinear, Corona::IMAGE_DETERMINE_FROM_EXT);
	}

	context.colorMappingData->gamma = this->mtco_renderGlobals->colorMapping_gamma;
}
