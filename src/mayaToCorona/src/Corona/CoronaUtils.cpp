#include "Corona.h"
#include "../mtco_common/mtco_mayaObject.h"
#include "utilities/tools.h"
#include "renderGlobals.h"
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MColor.h>
#include "utilities/logging.h"
#include "world.h"

static Logging logger;

Corona::Rgb toCorona(MColor& color)
{
	return Corona::Rgb(color.r, color.g, color.b);
}

void setTransformationMatrix(Corona::AffineTm& atm, MMatrix& mat)
{
	MMatrix t = mat.transpose();
	Corona::Float4 row1(t[0][0], t[0][1], t[0][2], t[0][3]);
	Corona::Float4 row2(t[1][0], t[1][1], t[1][2], t[1][3]);
	Corona::Float4 row3(t[2][0], t[2][1], t[2][2], t[2][3]);
	atm = Corona::AffineTm(row1, row2, row3);
}

void CoronaRenderer::setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, std::shared_ptr<MayaObject> obj)
{
	MMatrix to, from;

	// a segment contains start and end, 2 mb steps are one segment, 3 mb steps are 2 segments
	int numSegments = (obj->transformMatrices.size() - 1) * ((int)MayaTo::getWorldPtr()->worldRenderGlobalsPtr->doMb);
	atm.setSegments(numSegments); 

	for( size_t mId = 0; mId < (numSegments + 1); mId++)
	{
		MMatrix c = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->globalConversionMatrix;
		float fl[4][4];
		c.get(fl);
		//for (int y = 0; y < 4; y++)
		//{
		//	for (int x = 0; x < 4; x++)
		//	{
		//		std::cout << " " << fl[y][x];
		//	}
		//	std::cout << "\n";
		//}
		MMatrix t = (obj->transformMatrices[mId] * c).transpose();
		//Logging::debug(MString("Corona::AffineTm tm;"));
		//Logging::debug(MString("Corona::Float4 row1(") + t[0][0] + " " + t[0][1] + " " + t[0][2] + " " + t[0][3] + ");");
		//Logging::debug(MString("Corona::Float4 row2(") + t[1][0] + " " + t[1][1] + " " + t[1][2] + " " + t[1][3] + ");");
		//Logging::debug(MString("Corona::Float4 row3(") + t[2][0] + " " + t[2][1] + " " + t[2][2] + " " + t[2][3] + ");");
		//Logging::debug(MString("") + c[3][0] + " " + c[3][1] + " " + c[3][2] + " " + c[3][3]);
		Corona::AffineTm tm;
		Corona::Float4 row1(t[0][0],t[0][1],t[0][2],t[0][3]);
		Corona::Float4 row2(t[1][0],t[1][1],t[1][2],t[1][3]);
		Corona::Float4 row3(t[2][0],t[2][1],t[2][2],t[2][3]);
		tm = Corona::AffineTm(row1, row2, row3);
		atm[mId] = Corona::AffineTm::IDENTITY;
		atm[mId] = tm;
	}
}

void CoronaRenderer::setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat)
{
	MMatrix to, from;

	// a segment contains start and end, 2 mb steps are one segment, 3 mb steps are 2 segments
	int numSegments = 0;
	atm.setSegments(numSegments); 

	for( size_t mId = 0; mId < (numSegments + 1); mId++)
	{
		MMatrix c = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->globalConversionMatrix;
		MMatrix t = (mat * c).transpose();
		Corona::AffineTm tm;
		//Logging::debug(MString("Corona::AffineTm tm;"));
		//Logging::debug(MString("Corona::Float4 row1(") + t[0][0] + " " + t[0][1] + " " + t[0][2] + " " + t[0][3] + ");");
		//Logging::debug(MString("Corona::Float4 row2(") + t[1][0] + " " + t[1][1] + " " + t[1][2] + " " + t[1][3] + ");");
		//Logging::debug(MString("Corona::Float4 row3(") + t[2][0] + " " + t[2][1] + " " + t[2][2] + " " + t[2][3] + ");");
		Corona::Float4 row1(t[0][0], t[0][1], t[0][2], t[0][3]);
		Corona::Float4 row2(t[1][0],t[1][1],t[1][2],t[1][3]);
		Corona::Float4 row3(t[2][0],t[2][1],t[2][2],t[2][3]);
		tm = Corona::AffineTm(row1, row2, row3);
		atm[mId] = Corona::AffineTm::IDENTITY;
		atm[mId] = tm;
	}
}

void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, std::shared_ptr<MayaObject> obj)
{
	MMatrix to, from;

	// a segment contains start and end, 2 mb steps are one segment, 3 mb steps are 2 segments
	bool doMb = false;
	MMatrix globalConversionMatrix = MayaTo::getWorldPtr()->globalConversionMatrix;

	if (MayaTo::getWorldPtr()->worldRenderGlobalsPtr != nullptr)
	{
		doMb = MayaTo::getWorldPtr()->worldRenderGlobalsPtr->doMb;
	}
	int numSegments = (obj->transformMatrices.size() - 1) * ((int)doMb);
	atm.setSegments(numSegments);

	for (size_t mId = 0; mId < (numSegments + 1); mId++)
	{
		MMatrix t = (obj->transformMatrices[mId] * globalConversionMatrix).transpose();
		Corona::AffineTm tm;
		Corona::Float4 row1(t[0][0], t[0][1], t[0][2], t[0][3]);
		Corona::Float4 row2(t[1][0], t[1][1], t[1][2], t[1][3]);
		Corona::Float4 row3(t[2][0], t[2][1], t[2][2], t[2][3]);
		tm = Corona::AffineTm(row1, row2, row3);
		atm[mId] = Corona::AffineTm::IDENTITY;
		atm[mId] = tm;
	}
}

void setAnimatedTransformationMatrix(Corona::AnimatedAffineTm& atm, MMatrix& mat)
{
	MMatrix to, from;
	int numSegments = 0;
	atm.setSegments(numSegments);
	MMatrix globalConversionMatrix = MayaTo::getWorldPtr()->globalConversionMatrix;

	for (size_t mId = 0; mId < (numSegments + 1); mId++)
	{
		MMatrix t = (mat * globalConversionMatrix).transpose();
		Corona::AffineTm tm;
		Corona::Float4 row1(t[0][0], t[0][1], t[0][2], t[0][3]);
		Corona::Float4 row2(t[1][0], t[1][1], t[1][2], t[1][3]);
		Corona::Float4 row3(t[2][0], t[2][1], t[2][2], t[2][3]);
		tm = Corona::AffineTm(row1, row2, row3);
		atm[mId] = Corona::AffineTm::IDENTITY;
		atm[mId] = tm;
	}
}
