#include "appleseed.h"
#include <maya/MColor.h>
#include "../mtap_common/mtap_mayaScene.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/logging.h"

static Logging logger;

using namespace AppleRender;

//
// colors are defined in the scene scope, makes handling easier
//


void AppleseedRenderer::defineColor(MString& name, MColor& color, float intensity)
{
	float colorDef[3];
	colorDef[0] = color.r;
	colorDef[1] = color.g;
	colorDef[2] = color.b;
	float alpha = color.a;

	asf::auto_release_ptr<asr::ColorEntity> colorEntity;
	
	asr::ColorEntity *entity = this->scenePtr->colors().get_by_name(name.asChar());
	if( entity != NULL)
	{
		this->scenePtr->colors().remove(entity);
		//logger.debug(MString("Found color: ") + name);
		//asr::ColorValueArray cva = entity->get_values();
		//cva[0] = colorDef[0];
		//cva[1] = colorDef[1];
		//cva[2] = colorDef[2];
		//return;
	}
	MString intensityString = MString("") + intensity;
	colorEntity = asr::ColorEntityFactory::create(
				name.asChar(),
				asr::ParamArray()
					.insert("color_space", "srgb")
					.insert("multiplier", intensityString.asChar()),
				asr::ColorValueArray(3, colorDef),
				asr::ColorValueArray(1, &alpha));

	this->scenePtr->colors().insert(colorEntity);
}


void AppleseedRenderer::fillTransformMatices(MMatrix matrix, asr::AssemblyInstance *assInstance)
{
	assInstance->transform_sequence().clear();
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = matrix;
	this->MMatrixToAMatrix(colMatrix, appMatrix);
	assInstance->transform_sequence().set_transform(
			0.0f,
			asf::Transformd::from_local_to_parent(appMatrix));
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::AssemblyInstance *assInstance, MMatrix correctorMatrix)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	asf::Matrix4d appMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		colMatrix *= correctorMatrix;
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::AssemblyInstance *assInstance)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	asf::Matrix4d appMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		this->MMatrixToAMatrix(colMatrix, appMatrix);

		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::Camera *assInstance)
{
	assInstance->transform_sequence().clear();
	size_t numSteps =  obj->transformMatrices.size();
	size_t divSteps = numSteps;
	if( divSteps > 1)
		divSteps -= 1;
	float stepSize = 1.0f / (float)divSteps;
	float start = 0.0f;

	// here only the transform will be scaled because a scaled camera will result in different renderings (e.g. dof)

	asf::Matrix4d appMatrix;
	MMatrix transformMatrix;
	for( size_t matrixId = 0; matrixId < numSteps; matrixId++)
	{
		transformMatrix.setToIdentity();
		transformMatrix.matrix[3][0] = obj->transformMatrices[matrixId].matrix[3][0];
		transformMatrix.matrix[3][1] = obj->transformMatrices[matrixId].matrix[3][1];
		transformMatrix.matrix[3][2] = obj->transformMatrices[matrixId].matrix[3][2];
		transformMatrix *= this->renderGlobals->sceneScaleMatrix;
		MMatrix colMatrix = obj->transformMatrices[matrixId];
		colMatrix.matrix[3][0] = transformMatrix.matrix[3][0];
		colMatrix.matrix[3][1] = transformMatrix.matrix[3][1];
		colMatrix.matrix[3][2] = transformMatrix.matrix[3][2];
		this->MMatrixToAMatrix(colMatrix, appMatrix);
		logger.trace(MString("cam mat ") + colMatrix.matrix[3][0] + " " + colMatrix.matrix[3][1] + " " + colMatrix.matrix[3][2]);
		assInstance->transform_sequence().set_transform(
			start + stepSize * matrixId,
			asf::Transformd::from_local_to_parent(appMatrix));
	}
}

void AppleseedRenderer::fillTransformMatices(mtap_MayaObject *obj, asr::Light *light)
{
	asf::Matrix4d appMatrix;
	MMatrix colMatrix = obj->transformMatrices[0];
	this->MMatrixToAMatrix(colMatrix, appMatrix);
	light->set_transform(asf::Transformd::from_local_to_parent(appMatrix));	
}

void AppleseedRenderer::MMatrixToAMatrix(MMatrix& mayaMatrix, asf::Matrix4d& appleMatrix)
{
	MMatrix rowMatrix;
	rowToColumn(mayaMatrix, rowMatrix);
	for( int i = 0; i < 4; i++)
		for( int k = 0; k < 4; k++)
			appleMatrix[i * 4 + k] = rowMatrix[i][k];
}
