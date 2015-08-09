#ifndef MAYATO_SWATCHGEO_H
#define MAYATO_SWATCHGEO_H

#include <maya/MPointArray.h>
#include <maya/MPoint.h>
#include <maya/MVectorArray.h>
#include <maya/MFloatArray.h>

namespace SwatchRenderGeometry{

	MPointArray previewSphereShapeVertices(382);
	MVectorArray previewSphereShapeNormals(382);
	MFloatArray previewSphereShapeu(439);
	MFloatArray previewSphereShapev(439);
	MPointArray meshLightShapeVertices(4);
	MVectorArray meshLightShapeNormals(4);
	MFloatArray meshLightShapeu(4);
	MFloatArray meshLightShapev(4);
	MVectorArray BgShapeNormals(91);
	MPointArray BgShapeVertices(91);
	MFloatArray BgShapeu(91);
	MFloatArray BgShapev(91);


	void initGeometry()
	{
		previewSphereShapeVertices.append(MPoint(0.148778, -0.987688, -0.0483409));
		previewSphereShapeVertices.append(MPoint(0.126558, -0.987688, -0.0919499));
		previewSphereShapeVertices.append(MPoint(0.0919499, -0.987688, -0.126558));
		previewSphereShapeVertices.append(MPoint(0.0483409, -0.987688, -0.148778));
		previewSphereShapeVertices.append(MPoint(0, -0.987688, -0.156435));
		previewSphereShapeVertices.append(MPoint(-0.0483409, -0.987688, -0.148778));
		previewSphereShapeVertices.append(MPoint(-0.0919499, -0.987688, -0.126558));
		previewSphereShapeVertices.append(MPoint(-0.126558, -0.987688, -0.0919499));
		previewSphereShapeVertices.append(MPoint(-0.148778, -0.987688, -0.0483409));
		previewSphereShapeVertices.append(MPoint(-0.156435, -0.987688, 0));
		previewSphereShapeVertices.append(MPoint(-0.148778, -0.987688, 0.0483409));
		previewSphereShapeVertices.append(MPoint(-0.126558, -0.987688, 0.0919499));
		previewSphereShapeVertices.append(MPoint(-0.0919499, -0.987688, 0.126558));
		previewSphereShapeVertices.append(MPoint(-0.0483409, -0.987688, 0.148778));
		previewSphereShapeVertices.append(MPoint(-4.66211e-009, -0.987688, 0.156434));
		previewSphereShapeVertices.append(MPoint(0.0483409, -0.987688, 0.148778));
		previewSphereShapeVertices.append(MPoint(0.0919499, -0.987688, 0.126558));
		previewSphereShapeVertices.append(MPoint(0.126558, -0.987688, 0.0919499));
		previewSphereShapeVertices.append(MPoint(0.148778, -0.987688, 0.0483409));
		previewSphereShapeVertices.append(MPoint(0.156434, -0.987688, 0));
		previewSphereShapeVertices.append(MPoint(0.293893, -0.951057, -0.0954916));
		previewSphereShapeVertices.append(MPoint(0.25, -0.951057, -0.181636));
		previewSphereShapeVertices.append(MPoint(0.181636, -0.951057, -0.25));
		previewSphereShapeVertices.append(MPoint(0.0954916, -0.951057, -0.293893));
		previewSphereShapeVertices.append(MPoint(0, -0.951057, -0.309017));
		previewSphereShapeVertices.append(MPoint(-0.0954916, -0.951057, -0.293893));
		previewSphereShapeVertices.append(MPoint(-0.181636, -0.951057, -0.25));
		previewSphereShapeVertices.append(MPoint(-0.25, -0.951057, -0.181636));
		previewSphereShapeVertices.append(MPoint(-0.293893, -0.951057, -0.0954915));
		previewSphereShapeVertices.append(MPoint(-0.309017, -0.951057, 0));
		previewSphereShapeVertices.append(MPoint(-0.293893, -0.951057, 0.0954915));
		previewSphereShapeVertices.append(MPoint(-0.25, -0.951057, 0.181636));
		previewSphereShapeVertices.append(MPoint(-0.181636, -0.951057, 0.25));
		previewSphereShapeVertices.append(MPoint(-0.0954915, -0.951057, 0.293893));
		previewSphereShapeVertices.append(MPoint(-9.20942e-009, -0.951057, 0.309017));
		previewSphereShapeVertices.append(MPoint(0.0954915, -0.951057, 0.293893));
		previewSphereShapeVertices.append(MPoint(0.181636, -0.951057, 0.25));
		previewSphereShapeVertices.append(MPoint(0.25, -0.951057, 0.181636));
		previewSphereShapeVertices.append(MPoint(0.293893, -0.951057, 0.0954915));
		previewSphereShapeVertices.append(MPoint(0.309017, -0.951057, 0));
		previewSphereShapeVertices.append(MPoint(0.431771, -0.891007, -0.140291));
		previewSphereShapeVertices.append(MPoint(0.367286, -0.891007, -0.266849));
		previewSphereShapeVertices.append(MPoint(0.266849, -0.891007, -0.367286));
		previewSphereShapeVertices.append(MPoint(0.140291, -0.891007, -0.431771));
		previewSphereShapeVertices.append(MPoint(0, -0.891007, -0.453991));
		previewSphereShapeVertices.append(MPoint(-0.140291, -0.891007, -0.431771));
		previewSphereShapeVertices.append(MPoint(-0.266849, -0.891007, -0.367286));
		previewSphereShapeVertices.append(MPoint(-0.367286, -0.891007, -0.266849));
		previewSphereShapeVertices.append(MPoint(-0.431771, -0.891007, -0.140291));
		previewSphereShapeVertices.append(MPoint(-0.453991, -0.891007, 0));
		previewSphereShapeVertices.append(MPoint(-0.431771, -0.891007, 0.140291));
		previewSphereShapeVertices.append(MPoint(-0.367286, -0.891007, 0.266849));
		previewSphereShapeVertices.append(MPoint(-0.266849, -0.891007, 0.367286));
		previewSphereShapeVertices.append(MPoint(-0.140291, -0.891007, 0.431771));
		previewSphereShapeVertices.append(MPoint(-1.353e-008, -0.891007, 0.453991));
		previewSphereShapeVertices.append(MPoint(0.140291, -0.891007, 0.431771));
		previewSphereShapeVertices.append(MPoint(0.266849, -0.891007, 0.367286));
		previewSphereShapeVertices.append(MPoint(0.367286, -0.891007, 0.266849));
		previewSphereShapeVertices.append(MPoint(0.431771, -0.891007, 0.140291));
		previewSphereShapeVertices.append(MPoint(0.453991, -0.891007, 0));
		previewSphereShapeVertices.append(MPoint(0.559017, -0.809017, -0.181636));
		previewSphereShapeVertices.append(MPoint(0.475529, -0.809017, -0.345492));
		previewSphereShapeVertices.append(MPoint(0.345492, -0.809017, -0.475529));
		previewSphereShapeVertices.append(MPoint(0.181636, -0.809017, -0.559017));
		previewSphereShapeVertices.append(MPoint(0, -0.809017, -0.587786));
		previewSphereShapeVertices.append(MPoint(-0.181636, -0.809017, -0.559017));
		previewSphereShapeVertices.append(MPoint(-0.345492, -0.809017, -0.475528));
		previewSphereShapeVertices.append(MPoint(-0.475528, -0.809017, -0.345492));
		previewSphereShapeVertices.append(MPoint(-0.559017, -0.809017, -0.181636));
		previewSphereShapeVertices.append(MPoint(-0.587785, -0.809017, 0));
		previewSphereShapeVertices.append(MPoint(-0.559017, -0.809017, 0.181636));
		previewSphereShapeVertices.append(MPoint(-0.475528, -0.809017, 0.345492));
		previewSphereShapeVertices.append(MPoint(-0.345492, -0.809017, 0.475528));
		previewSphereShapeVertices.append(MPoint(-0.181636, -0.809017, 0.559017));
		previewSphereShapeVertices.append(MPoint(-1.75174e-008, -0.809017, 0.587785));
		previewSphereShapeVertices.append(MPoint(0.181636, -0.809017, 0.559017));
		previewSphereShapeVertices.append(MPoint(0.345491, -0.809017, 0.475528));
		previewSphereShapeVertices.append(MPoint(0.475528, -0.809017, 0.345492));
		previewSphereShapeVertices.append(MPoint(0.559017, -0.809017, 0.181636));
		previewSphereShapeVertices.append(MPoint(0.587785, -0.809017, 0));
		previewSphereShapeVertices.append(MPoint(0.672499, -0.707107, -0.218508));
		previewSphereShapeVertices.append(MPoint(0.572062, -0.707107, -0.415627));
		previewSphereShapeVertices.append(MPoint(0.415627, -0.707107, -0.572062));
		previewSphereShapeVertices.append(MPoint(0.218508, -0.707107, -0.672499));
		previewSphereShapeVertices.append(MPoint(0, -0.707107, -0.707107));
		previewSphereShapeVertices.append(MPoint(-0.218508, -0.707107, -0.672499));
		previewSphereShapeVertices.append(MPoint(-0.415627, -0.707107, -0.572062));
		previewSphereShapeVertices.append(MPoint(-0.572062, -0.707107, -0.415627));
		previewSphereShapeVertices.append(MPoint(-0.672499, -0.707107, -0.218508));
		previewSphereShapeVertices.append(MPoint(-0.707107, -0.707107, 0));
		previewSphereShapeVertices.append(MPoint(-0.672499, -0.707107, 0.218508));
		previewSphereShapeVertices.append(MPoint(-0.572062, -0.707107, 0.415627));
		previewSphereShapeVertices.append(MPoint(-0.415627, -0.707107, 0.572061));
		previewSphereShapeVertices.append(MPoint(-0.218508, -0.707107, 0.672499));
		previewSphereShapeVertices.append(MPoint(-2.10734e-008, -0.707107, 0.707107));
		previewSphereShapeVertices.append(MPoint(0.218508, -0.707107, 0.672499));
		previewSphereShapeVertices.append(MPoint(0.415627, -0.707107, 0.572061));
		previewSphereShapeVertices.append(MPoint(0.572061, -0.707107, 0.415627));
		previewSphereShapeVertices.append(MPoint(0.672499, -0.707107, 0.218508));
		previewSphereShapeVertices.append(MPoint(0.707107, -0.707107, 0));
		previewSphereShapeVertices.append(MPoint(0.769421, -0.587785, -0.25));
		previewSphereShapeVertices.append(MPoint(0.654509, -0.587785, -0.475529));
		previewSphereShapeVertices.append(MPoint(0.475529, -0.587785, -0.654509));
		previewSphereShapeVertices.append(MPoint(0.25, -0.587785, -0.769421));
		previewSphereShapeVertices.append(MPoint(0, -0.587785, -0.809017));
		previewSphereShapeVertices.append(MPoint(-0.25, -0.587785, -0.769421));
		previewSphereShapeVertices.append(MPoint(-0.475528, -0.587785, -0.654509));
		previewSphereShapeVertices.append(MPoint(-0.654509, -0.587785, -0.475528));
		previewSphereShapeVertices.append(MPoint(-0.769421, -0.587785, -0.25));
		previewSphereShapeVertices.append(MPoint(-0.809017, -0.587785, 0));
		previewSphereShapeVertices.append(MPoint(-0.769421, -0.587785, 0.25));
		previewSphereShapeVertices.append(MPoint(-0.654509, -0.587785, 0.475528));
		previewSphereShapeVertices.append(MPoint(-0.475528, -0.587785, 0.654509));
		previewSphereShapeVertices.append(MPoint(-0.25, -0.587785, 0.769421));
		previewSphereShapeVertices.append(MPoint(-2.41106e-008, -0.587785, 0.809017));
		previewSphereShapeVertices.append(MPoint(0.25, -0.587785, 0.769421));
		previewSphereShapeVertices.append(MPoint(0.475528, -0.587785, 0.654509));
		previewSphereShapeVertices.append(MPoint(0.654509, -0.587785, 0.475528));
		previewSphereShapeVertices.append(MPoint(0.769421, -0.587785, 0.25));
		previewSphereShapeVertices.append(MPoint(0.809017, -0.587785, 0));
		previewSphereShapeVertices.append(MPoint(0.847398, -0.453991, -0.275336));
		previewSphereShapeVertices.append(MPoint(0.72084, -0.453991, -0.523721));
		previewSphereShapeVertices.append(MPoint(0.523721, -0.453991, -0.72084));
		previewSphereShapeVertices.append(MPoint(0.275336, -0.453991, -0.847398));
		previewSphereShapeVertices.append(MPoint(0, -0.453991, -0.891007));
		previewSphereShapeVertices.append(MPoint(-0.275336, -0.453991, -0.847398));
		previewSphereShapeVertices.append(MPoint(-0.523721, -0.453991, -0.72084));
		previewSphereShapeVertices.append(MPoint(-0.72084, -0.453991, -0.523721));
		previewSphereShapeVertices.append(MPoint(-0.847398, -0.453991, -0.275336));
		previewSphereShapeVertices.append(MPoint(-0.891007, -0.453991, 0));
		previewSphereShapeVertices.append(MPoint(-0.847398, -0.453991, 0.275336));
		previewSphereShapeVertices.append(MPoint(-0.72084, -0.453991, 0.523721));
		previewSphereShapeVertices.append(MPoint(-0.523721, -0.453991, 0.72084));
		previewSphereShapeVertices.append(MPoint(-0.275336, -0.453991, 0.847398));
		previewSphereShapeVertices.append(MPoint(-2.65541e-008, -0.453991, 0.891007));
		previewSphereShapeVertices.append(MPoint(0.275336, -0.453991, 0.847398));
		previewSphereShapeVertices.append(MPoint(0.523721, -0.453991, 0.72084));
		previewSphereShapeVertices.append(MPoint(0.720839, -0.453991, 0.523721));
		previewSphereShapeVertices.append(MPoint(0.847398, -0.453991, 0.275336));
		previewSphereShapeVertices.append(MPoint(0.891007, -0.453991, 0));
		previewSphereShapeVertices.append(MPoint(0.904509, -0.309017, -0.293893));
		previewSphereShapeVertices.append(MPoint(0.769421, -0.309017, -0.559017));
		previewSphereShapeVertices.append(MPoint(0.559017, -0.309017, -0.769421));
		previewSphereShapeVertices.append(MPoint(0.293893, -0.309017, -0.904509));
		previewSphereShapeVertices.append(MPoint(0, -0.309017, -0.951057));
		previewSphereShapeVertices.append(MPoint(-0.293893, -0.309017, -0.904509));
		previewSphereShapeVertices.append(MPoint(-0.559017, -0.309017, -0.769421));
		previewSphereShapeVertices.append(MPoint(-0.769421, -0.309017, -0.559017));
		previewSphereShapeVertices.append(MPoint(-0.904509, -0.309017, -0.293893));
		previewSphereShapeVertices.append(MPoint(-0.951057, -0.309017, 0));
		previewSphereShapeVertices.append(MPoint(-0.904509, -0.309017, 0.293893));
		previewSphereShapeVertices.append(MPoint(-0.769421, -0.309017, 0.559017));
		previewSphereShapeVertices.append(MPoint(-0.559017, -0.309017, 0.769421));
		previewSphereShapeVertices.append(MPoint(-0.293893, -0.309017, 0.904509));
		previewSphereShapeVertices.append(MPoint(-2.83437e-008, -0.309017, 0.951057));
		previewSphereShapeVertices.append(MPoint(0.293893, -0.309017, 0.904509));
		previewSphereShapeVertices.append(MPoint(0.559017, -0.309017, 0.769421));
		previewSphereShapeVertices.append(MPoint(0.769421, -0.309017, 0.559017));
		previewSphereShapeVertices.append(MPoint(0.904509, -0.309017, 0.293893));
		previewSphereShapeVertices.append(MPoint(0.951057, -0.309017, 0));
		previewSphereShapeVertices.append(MPoint(0.939348, -0.156434, -0.305213));
		previewSphereShapeVertices.append(MPoint(0.799057, -0.156434, -0.580549));
		previewSphereShapeVertices.append(MPoint(0.580549, -0.156434, -0.799057));
		previewSphereShapeVertices.append(MPoint(0.305213, -0.156434, -0.939348));
		previewSphereShapeVertices.append(MPoint(0, -0.156434, -0.987689));
		previewSphereShapeVertices.append(MPoint(-0.305213, -0.156434, -0.939348));
		previewSphereShapeVertices.append(MPoint(-0.580549, -0.156434, -0.799057));
		previewSphereShapeVertices.append(MPoint(-0.799057, -0.156434, -0.580549));
		previewSphereShapeVertices.append(MPoint(-0.939348, -0.156434, -0.305213));
		previewSphereShapeVertices.append(MPoint(-0.987689, -0.156434, 0));
		previewSphereShapeVertices.append(MPoint(-0.939348, -0.156434, 0.305213));
		previewSphereShapeVertices.append(MPoint(-0.799057, -0.156434, 0.580549));
		previewSphereShapeVertices.append(MPoint(-0.580549, -0.156434, 0.799057));
		previewSphereShapeVertices.append(MPoint(-0.305213, -0.156434, 0.939348));
		previewSphereShapeVertices.append(MPoint(-2.94354e-008, -0.156434, 0.987688));
		previewSphereShapeVertices.append(MPoint(0.305212, -0.156434, 0.939348));
		previewSphereShapeVertices.append(MPoint(0.580549, -0.156434, 0.799057));
		previewSphereShapeVertices.append(MPoint(0.799057, -0.156434, 0.580549));
		previewSphereShapeVertices.append(MPoint(0.939348, -0.156434, 0.305212));
		previewSphereShapeVertices.append(MPoint(0.987688, -0.156434, 0));
		previewSphereShapeVertices.append(MPoint(0.951057, 0, -0.309017));
		previewSphereShapeVertices.append(MPoint(0.809018, 0, -0.587786));
		previewSphereShapeVertices.append(MPoint(0.587786, 0, -0.809017));
		previewSphereShapeVertices.append(MPoint(0.309017, 0, -0.951057));
		previewSphereShapeVertices.append(MPoint(0, 0, -1));
		previewSphereShapeVertices.append(MPoint(-0.309017, 0, -0.951057));
		previewSphereShapeVertices.append(MPoint(-0.587785, 0, -0.809017));
		previewSphereShapeVertices.append(MPoint(-0.809017, 0, -0.587785));
		previewSphereShapeVertices.append(MPoint(-0.951057, 0, -0.309017));
		previewSphereShapeVertices.append(MPoint(-1, 0, 0));
		previewSphereShapeVertices.append(MPoint(-0.951057, 0, 0.309017));
		previewSphereShapeVertices.append(MPoint(-0.809017, 0, 0.587785));
		previewSphereShapeVertices.append(MPoint(-0.587785, 0, 0.809017));
		previewSphereShapeVertices.append(MPoint(-0.309017, 0, 0.951057));
		previewSphereShapeVertices.append(MPoint(-2.98023e-008, 0, 1));
		previewSphereShapeVertices.append(MPoint(0.309017, 0, 0.951057));
		previewSphereShapeVertices.append(MPoint(0.587785, 0, 0.809017));
		previewSphereShapeVertices.append(MPoint(0.809017, 0, 0.587785));
		previewSphereShapeVertices.append(MPoint(0.951057, 0, 0.309017));
		previewSphereShapeVertices.append(MPoint(1, 0, 0));
		previewSphereShapeVertices.append(MPoint(0.939348, 0.156434, -0.305213));
		previewSphereShapeVertices.append(MPoint(0.799057, 0.156434, -0.580549));
		previewSphereShapeVertices.append(MPoint(0.580549, 0.156434, -0.799057));
		previewSphereShapeVertices.append(MPoint(0.305213, 0.156434, -0.939348));
		previewSphereShapeVertices.append(MPoint(0, 0.156434, -0.987689));
		previewSphereShapeVertices.append(MPoint(-0.305213, 0.156434, -0.939348));
		previewSphereShapeVertices.append(MPoint(-0.580549, 0.156434, -0.799057));
		previewSphereShapeVertices.append(MPoint(-0.799057, 0.156434, -0.580549));
		previewSphereShapeVertices.append(MPoint(-0.939348, 0.156434, -0.305213));
		previewSphereShapeVertices.append(MPoint(-0.987689, 0.156434, 0));
		previewSphereShapeVertices.append(MPoint(-0.939348, 0.156434, 0.305213));
		previewSphereShapeVertices.append(MPoint(-0.799057, 0.156434, 0.580549));
		previewSphereShapeVertices.append(MPoint(-0.580549, 0.156434, 0.799057));
		previewSphereShapeVertices.append(MPoint(-0.305213, 0.156434, 0.939348));
		previewSphereShapeVertices.append(MPoint(-2.94354e-008, 0.156434, 0.987688));
		previewSphereShapeVertices.append(MPoint(0.305212, 0.156434, 0.939348));
		previewSphereShapeVertices.append(MPoint(0.580549, 0.156434, 0.799057));
		previewSphereShapeVertices.append(MPoint(0.799057, 0.156434, 0.580549));
		previewSphereShapeVertices.append(MPoint(0.939348, 0.156434, 0.305212));
		previewSphereShapeVertices.append(MPoint(0.987688, 0.156434, 0));
		previewSphereShapeVertices.append(MPoint(0.904509, 0.309017, -0.293893));
		previewSphereShapeVertices.append(MPoint(0.769421, 0.309017, -0.559017));
		previewSphereShapeVertices.append(MPoint(0.559017, 0.309017, -0.769421));
		previewSphereShapeVertices.append(MPoint(0.293893, 0.309017, -0.904509));
		previewSphereShapeVertices.append(MPoint(0, 0.309017, -0.951057));
		previewSphereShapeVertices.append(MPoint(-0.293893, 0.309017, -0.904509));
		previewSphereShapeVertices.append(MPoint(-0.559017, 0.309017, -0.769421));
		previewSphereShapeVertices.append(MPoint(-0.769421, 0.309017, -0.559017));
		previewSphereShapeVertices.append(MPoint(-0.904509, 0.309017, -0.293893));
		previewSphereShapeVertices.append(MPoint(-0.951057, 0.309017, 0));
		previewSphereShapeVertices.append(MPoint(-0.904509, 0.309017, 0.293893));
		previewSphereShapeVertices.append(MPoint(-0.769421, 0.309017, 0.559017));
		previewSphereShapeVertices.append(MPoint(-0.559017, 0.309017, 0.769421));
		previewSphereShapeVertices.append(MPoint(-0.293893, 0.309017, 0.904509));
		previewSphereShapeVertices.append(MPoint(-2.83437e-008, 0.309017, 0.951057));
		previewSphereShapeVertices.append(MPoint(0.293893, 0.309017, 0.904509));
		previewSphereShapeVertices.append(MPoint(0.559017, 0.309017, 0.769421));
		previewSphereShapeVertices.append(MPoint(0.769421, 0.309017, 0.559017));
		previewSphereShapeVertices.append(MPoint(0.904509, 0.309017, 0.293893));
		previewSphereShapeVertices.append(MPoint(0.951057, 0.309017, 0));
		previewSphereShapeVertices.append(MPoint(0.847398, 0.453991, -0.275336));
		previewSphereShapeVertices.append(MPoint(0.72084, 0.453991, -0.523721));
		previewSphereShapeVertices.append(MPoint(0.523721, 0.453991, -0.72084));
		previewSphereShapeVertices.append(MPoint(0.275336, 0.453991, -0.847398));
		previewSphereShapeVertices.append(MPoint(0, 0.453991, -0.891007));
		previewSphereShapeVertices.append(MPoint(-0.275336, 0.453991, -0.847398));
		previewSphereShapeVertices.append(MPoint(-0.523721, 0.453991, -0.72084));
		previewSphereShapeVertices.append(MPoint(-0.72084, 0.453991, -0.523721));
		previewSphereShapeVertices.append(MPoint(-0.847398, 0.453991, -0.275336));
		previewSphereShapeVertices.append(MPoint(-0.891007, 0.453991, 0));
		previewSphereShapeVertices.append(MPoint(-0.847398, 0.453991, 0.275336));
		previewSphereShapeVertices.append(MPoint(-0.72084, 0.453991, 0.523721));
		previewSphereShapeVertices.append(MPoint(-0.523721, 0.453991, 0.72084));
		previewSphereShapeVertices.append(MPoint(-0.275336, 0.453991, 0.847398));
		previewSphereShapeVertices.append(MPoint(-2.65541e-008, 0.453991, 0.891007));
		previewSphereShapeVertices.append(MPoint(0.275336, 0.453991, 0.847398));
		previewSphereShapeVertices.append(MPoint(0.523721, 0.453991, 0.72084));
		previewSphereShapeVertices.append(MPoint(0.720839, 0.453991, 0.523721));
		previewSphereShapeVertices.append(MPoint(0.847398, 0.453991, 0.275336));
		previewSphereShapeVertices.append(MPoint(0.891007, 0.453991, 0));
		previewSphereShapeVertices.append(MPoint(0.769421, 0.587785, -0.25));
		previewSphereShapeVertices.append(MPoint(0.654509, 0.587785, -0.475529));
		previewSphereShapeVertices.append(MPoint(0.475529, 0.587785, -0.654509));
		previewSphereShapeVertices.append(MPoint(0.25, 0.587785, -0.769421));
		previewSphereShapeVertices.append(MPoint(0, 0.587785, -0.809017));
		previewSphereShapeVertices.append(MPoint(-0.25, 0.587785, -0.769421));
		previewSphereShapeVertices.append(MPoint(-0.475528, 0.587785, -0.654509));
		previewSphereShapeVertices.append(MPoint(-0.654509, 0.587785, -0.475528));
		previewSphereShapeVertices.append(MPoint(-0.769421, 0.587785, -0.25));
		previewSphereShapeVertices.append(MPoint(-0.809017, 0.587785, 0));
		previewSphereShapeVertices.append(MPoint(-0.769421, 0.587785, 0.25));
		previewSphereShapeVertices.append(MPoint(-0.654509, 0.587785, 0.475528));
		previewSphereShapeVertices.append(MPoint(-0.475528, 0.587785, 0.654509));
		previewSphereShapeVertices.append(MPoint(-0.25, 0.587785, 0.769421));
		previewSphereShapeVertices.append(MPoint(-2.41106e-008, 0.587785, 0.809017));
		previewSphereShapeVertices.append(MPoint(0.25, 0.587785, 0.769421));
		previewSphereShapeVertices.append(MPoint(0.475528, 0.587785, 0.654509));
		previewSphereShapeVertices.append(MPoint(0.654509, 0.587785, 0.475528));
		previewSphereShapeVertices.append(MPoint(0.769421, 0.587785, 0.25));
		previewSphereShapeVertices.append(MPoint(0.809017, 0.587785, 0));
		previewSphereShapeVertices.append(MPoint(0.672499, 0.707107, -0.218508));
		previewSphereShapeVertices.append(MPoint(0.572062, 0.707107, -0.415627));
		previewSphereShapeVertices.append(MPoint(0.415627, 0.707107, -0.572062));
		previewSphereShapeVertices.append(MPoint(0.218508, 0.707107, -0.672499));
		previewSphereShapeVertices.append(MPoint(0, 0.707107, -0.707107));
		previewSphereShapeVertices.append(MPoint(-0.218508, 0.707107, -0.672499));
		previewSphereShapeVertices.append(MPoint(-0.415627, 0.707107, -0.572062));
		previewSphereShapeVertices.append(MPoint(-0.572062, 0.707107, -0.415627));
		previewSphereShapeVertices.append(MPoint(-0.672499, 0.707107, -0.218508));
		previewSphereShapeVertices.append(MPoint(-0.707107, 0.707107, 0));
		previewSphereShapeVertices.append(MPoint(-0.672499, 0.707107, 0.218508));
		previewSphereShapeVertices.append(MPoint(-0.572062, 0.707107, 0.415627));
		previewSphereShapeVertices.append(MPoint(-0.415627, 0.707107, 0.572061));
		previewSphereShapeVertices.append(MPoint(-0.218508, 0.707107, 0.672499));
		previewSphereShapeVertices.append(MPoint(-2.10734e-008, 0.707107, 0.707107));
		previewSphereShapeVertices.append(MPoint(0.218508, 0.707107, 0.672499));
		previewSphereShapeVertices.append(MPoint(0.415627, 0.707107, 0.572061));
		previewSphereShapeVertices.append(MPoint(0.572061, 0.707107, 0.415627));
		previewSphereShapeVertices.append(MPoint(0.672499, 0.707107, 0.218508));
		previewSphereShapeVertices.append(MPoint(0.707107, 0.707107, 0));
		previewSphereShapeVertices.append(MPoint(0.559017, 0.809017, -0.181636));
		previewSphereShapeVertices.append(MPoint(0.475529, 0.809017, -0.345492));
		previewSphereShapeVertices.append(MPoint(0.345492, 0.809017, -0.475529));
		previewSphereShapeVertices.append(MPoint(0.181636, 0.809017, -0.559017));
		previewSphereShapeVertices.append(MPoint(0, 0.809017, -0.587786));
		previewSphereShapeVertices.append(MPoint(-0.181636, 0.809017, -0.559017));
		previewSphereShapeVertices.append(MPoint(-0.345492, 0.809017, -0.475528));
		previewSphereShapeVertices.append(MPoint(-0.475528, 0.809017, -0.345492));
		previewSphereShapeVertices.append(MPoint(-0.559017, 0.809017, -0.181636));
		previewSphereShapeVertices.append(MPoint(-0.587785, 0.809017, 0));
		previewSphereShapeVertices.append(MPoint(-0.559017, 0.809017, 0.181636));
		previewSphereShapeVertices.append(MPoint(-0.475528, 0.809017, 0.345492));
		previewSphereShapeVertices.append(MPoint(-0.345492, 0.809017, 0.475528));
		previewSphereShapeVertices.append(MPoint(-0.181636, 0.809017, 0.559017));
		previewSphereShapeVertices.append(MPoint(-1.75174e-008, 0.809017, 0.587785));
		previewSphereShapeVertices.append(MPoint(0.181636, 0.809017, 0.559017));
		previewSphereShapeVertices.append(MPoint(0.345491, 0.809017, 0.475528));
		previewSphereShapeVertices.append(MPoint(0.475528, 0.809017, 0.345492));
		previewSphereShapeVertices.append(MPoint(0.559017, 0.809017, 0.181636));
		previewSphereShapeVertices.append(MPoint(0.587785, 0.809017, 0));
		previewSphereShapeVertices.append(MPoint(0.431771, 0.891007, -0.140291));
		previewSphereShapeVertices.append(MPoint(0.367286, 0.891007, -0.266849));
		previewSphereShapeVertices.append(MPoint(0.266849, 0.891007, -0.367286));
		previewSphereShapeVertices.append(MPoint(0.140291, 0.891007, -0.431771));
		previewSphereShapeVertices.append(MPoint(0, 0.891007, -0.453991));
		previewSphereShapeVertices.append(MPoint(-0.140291, 0.891007, -0.431771));
		previewSphereShapeVertices.append(MPoint(-0.266849, 0.891007, -0.367286));
		previewSphereShapeVertices.append(MPoint(-0.367286, 0.891007, -0.266849));
		previewSphereShapeVertices.append(MPoint(-0.431771, 0.891007, -0.140291));
		previewSphereShapeVertices.append(MPoint(-0.453991, 0.891007, 0));
		previewSphereShapeVertices.append(MPoint(-0.431771, 0.891007, 0.140291));
		previewSphereShapeVertices.append(MPoint(-0.367286, 0.891007, 0.266849));
		previewSphereShapeVertices.append(MPoint(-0.266849, 0.891007, 0.367286));
		previewSphereShapeVertices.append(MPoint(-0.140291, 0.891007, 0.431771));
		previewSphereShapeVertices.append(MPoint(-1.353e-008, 0.891007, 0.453991));
		previewSphereShapeVertices.append(MPoint(0.140291, 0.891007, 0.431771));
		previewSphereShapeVertices.append(MPoint(0.266849, 0.891007, 0.367286));
		previewSphereShapeVertices.append(MPoint(0.367286, 0.891007, 0.266849));
		previewSphereShapeVertices.append(MPoint(0.431771, 0.891007, 0.140291));
		previewSphereShapeVertices.append(MPoint(0.453991, 0.891007, 0));
		previewSphereShapeVertices.append(MPoint(0.293893, 0.951057, -0.0954916));
		previewSphereShapeVertices.append(MPoint(0.25, 0.951057, -0.181636));
		previewSphereShapeVertices.append(MPoint(0.181636, 0.951057, -0.25));
		previewSphereShapeVertices.append(MPoint(0.0954916, 0.951057, -0.293893));
		previewSphereShapeVertices.append(MPoint(0, 0.951057, -0.309017));
		previewSphereShapeVertices.append(MPoint(-0.0954916, 0.951057, -0.293893));
		previewSphereShapeVertices.append(MPoint(-0.181636, 0.951057, -0.25));
		previewSphereShapeVertices.append(MPoint(-0.25, 0.951057, -0.181636));
		previewSphereShapeVertices.append(MPoint(-0.293893, 0.951057, -0.0954915));
		previewSphereShapeVertices.append(MPoint(-0.309017, 0.951057, 0));
		previewSphereShapeVertices.append(MPoint(-0.293893, 0.951057, 0.0954915));
		previewSphereShapeVertices.append(MPoint(-0.25, 0.951057, 0.181636));
		previewSphereShapeVertices.append(MPoint(-0.181636, 0.951057, 0.25));
		previewSphereShapeVertices.append(MPoint(-0.0954915, 0.951057, 0.293893));
		previewSphereShapeVertices.append(MPoint(-9.20942e-009, 0.951057, 0.309017));
		previewSphereShapeVertices.append(MPoint(0.0954915, 0.951057, 0.293893));
		previewSphereShapeVertices.append(MPoint(0.181636, 0.951057, 0.25));
		previewSphereShapeVertices.append(MPoint(0.25, 0.951057, 0.181636));
		previewSphereShapeVertices.append(MPoint(0.293893, 0.951057, 0.0954915));
		previewSphereShapeVertices.append(MPoint(0.309017, 0.951057, 0));
		previewSphereShapeVertices.append(MPoint(0.148778, 0.987688, -0.0483409));
		previewSphereShapeVertices.append(MPoint(0.126558, 0.987688, -0.0919499));
		previewSphereShapeVertices.append(MPoint(0.0919499, 0.987688, -0.126558));
		previewSphereShapeVertices.append(MPoint(0.0483409, 0.987688, -0.148778));
		previewSphereShapeVertices.append(MPoint(0, 0.987688, -0.156435));
		previewSphereShapeVertices.append(MPoint(-0.0483409, 0.987688, -0.148778));
		previewSphereShapeVertices.append(MPoint(-0.0919499, 0.987688, -0.126558));
		previewSphereShapeVertices.append(MPoint(-0.126558, 0.987688, -0.0919499));
		previewSphereShapeVertices.append(MPoint(-0.148778, 0.987688, -0.0483409));
		previewSphereShapeVertices.append(MPoint(-0.156435, 0.987688, 0));
		previewSphereShapeVertices.append(MPoint(-0.148778, 0.987688, 0.0483409));
		previewSphereShapeVertices.append(MPoint(-0.126558, 0.987688, 0.0919499));
		previewSphereShapeVertices.append(MPoint(-0.0919499, 0.987688, 0.126558));
		previewSphereShapeVertices.append(MPoint(-0.0483409, 0.987688, 0.148778));
		previewSphereShapeVertices.append(MPoint(-4.66211e-009, 0.987688, 0.156434));
		previewSphereShapeVertices.append(MPoint(0.0483409, 0.987688, 0.148778));
		previewSphereShapeVertices.append(MPoint(0.0919499, 0.987688, 0.126558));
		previewSphereShapeVertices.append(MPoint(0.126558, 0.987688, 0.0919499));
		previewSphereShapeVertices.append(MPoint(0.148778, 0.987688, 0.0483409));
		previewSphereShapeVertices.append(MPoint(0.156434, 0.987688, 0));
		previewSphereShapeVertices.append(MPoint(0, -1, 0));
		previewSphereShapeVertices.append(MPoint(0, 1, 0));

		previewSphereShapeNormals.append(MVector(0.190555, -0.979722, -0.0619152));
		previewSphereShapeNormals.append(MVector(0.162096, -0.979722, -0.117769));
		previewSphereShapeNormals.append(MVector(0.26987, -0.942723, -0.196072));
		previewSphereShapeNormals.append(MVector(0.317252, -0.942722, -0.103081));
		previewSphereShapeNormals.append(MVector(0.117769, -0.979722, -0.162095));
		previewSphereShapeNormals.append(MVector(0.196072, -0.942723, -0.26987));
		previewSphereShapeNormals.append(MVector(0.061915, -0.979722, -0.190555));
		previewSphereShapeNormals.append(MVector(0.103081, -0.942723, -0.317251));
		previewSphereShapeNormals.append(MVector(-3.36447e-008, -0.979722, -0.200361));
		previewSphereShapeNormals.append(MVector(-5.01225e-008, -0.942723, -0.333578));
		previewSphereShapeNormals.append(MVector(-0.061915, -0.979722, -0.190555));
		previewSphereShapeNormals.append(MVector(-0.103081, -0.942722, -0.317252));
		previewSphereShapeNormals.append(MVector(-0.117769, -0.979722, -0.162096));
		previewSphereShapeNormals.append(MVector(-0.196072, -0.942722, -0.269871));
		previewSphereShapeNormals.append(MVector(-0.162096, -0.979722, -0.11777));
		previewSphereShapeNormals.append(MVector(-0.26987, -0.942722, -0.196072));
		previewSphereShapeNormals.append(MVector(-0.190555, -0.979722, -0.0619154));
		previewSphereShapeNormals.append(MVector(-0.317252, -0.942722, -0.103081));
		previewSphereShapeNormals.append(MVector(-0.200361, -0.979722, 0));
		previewSphereShapeNormals.append(MVector(-0.333578, -0.942722, -2.445e-009));
		previewSphereShapeNormals.append(MVector(-0.190555, -0.979722, 0.0619154));
		previewSphereShapeNormals.append(MVector(-0.317252, -0.942722, 0.103081));
		previewSphereShapeNormals.append(MVector(-0.162096, -0.979722, 0.117769));
		previewSphereShapeNormals.append(MVector(-0.26987, -0.942722, 0.196072));
		previewSphereShapeNormals.append(MVector(-0.117769, -0.979722, 0.162095));
		previewSphereShapeNormals.append(MVector(-0.196072, -0.942722, 0.26987));
		previewSphereShapeNormals.append(MVector(-0.061915, -0.979722, 0.190555));
		previewSphereShapeNormals.append(MVector(-0.103081, -0.942722, 0.317252));
		previewSphereShapeNormals.append(MVector(3.87869e-008, -0.979722, 0.200361));
		previewSphereShapeNormals.append(MVector(8.67976e-008, -0.942722, 0.333578));
		previewSphereShapeNormals.append(MVector(0.0619151, -0.979722, 0.190555));
		previewSphereShapeNormals.append(MVector(0.103081, -0.942722, 0.317252));
		previewSphereShapeNormals.append(MVector(0.117769, -0.979722, 0.162096));
		previewSphereShapeNormals.append(MVector(0.196072, -0.942722, 0.26987));
		previewSphereShapeNormals.append(MVector(0.162096, -0.979722, 0.117769));
		previewSphereShapeNormals.append(MVector(0.26987, -0.942722, 0.196073));
		previewSphereShapeNormals.append(MVector(0.190555, -0.979722, 0.0619154));
		previewSphereShapeNormals.append(MVector(0.317252, -0.942722, 0.103081));
		previewSphereShapeNormals.append(MVector(0.200361, -0.979722, 5.31557e-007));
		previewSphereShapeNormals.append(MVector(0.333578, -0.942722, 2.37165e-007));
		previewSphereShapeNormals.append(MVector(0.380846, -0.882266, -0.276701));
		previewSphereShapeNormals.append(MVector(0.447711, -0.882266, -0.14547));
		previewSphereShapeNormals.append(MVector(0.276701, -0.882266, -0.380846));
		previewSphereShapeNormals.append(MVector(0.14547, -0.882266, -0.447711));
		previewSphereShapeNormals.append(MVector(-6.05382e-008, -0.882266, -0.470752));
		previewSphereShapeNormals.append(MVector(-0.14547, -0.882266, -0.447711));
		previewSphereShapeNormals.append(MVector(-0.276701, -0.882266, -0.380846));
		previewSphereShapeNormals.append(MVector(-0.380846, -0.882266, -0.276701));
		previewSphereShapeNormals.append(MVector(-0.447712, -0.882266, -0.14547));
		previewSphereShapeNormals.append(MVector(-0.470752, -0.882266, 8.91257e-008));
		previewSphereShapeNormals.append(MVector(-0.447712, -0.882266, 0.14547));
		previewSphereShapeNormals.append(MVector(-0.380846, -0.882266, 0.276701));
		previewSphereShapeNormals.append(MVector(-0.276701, -0.882266, 0.380846));
		previewSphereShapeNormals.append(MVector(-0.14547, -0.882266, 0.447712));
		previewSphereShapeNormals.append(MVector(4.03588e-008, -0.882266, 0.470752));
		previewSphereShapeNormals.append(MVector(0.14547, -0.882266, 0.447712));
		previewSphereShapeNormals.append(MVector(0.276701, -0.882266, 0.380846));
		previewSphereShapeNormals.append(MVector(0.380846, -0.882266, 0.276701));
		previewSphereShapeNormals.append(MVector(0.447712, -0.882266, 0.14547));
		previewSphereShapeNormals.append(MVector(0.470752, -0.882266, 1.93386e-007));
		previewSphereShapeNormals.append(MVector(0.485138, -0.800252, -0.352474));
		previewSphereShapeNormals.append(MVector(0.570314, -0.800252, -0.185306));
		previewSphereShapeNormals.append(MVector(0.352473, -0.800252, -0.485138));
		previewSphereShapeNormals.append(MVector(0.185306, -0.800252, -0.570314));
		previewSphereShapeNormals.append(MVector(-2.87481e-008, -0.800252, -0.599664));
		previewSphereShapeNormals.append(MVector(-0.185307, -0.800252, -0.570314));
		previewSphereShapeNormals.append(MVector(-0.352474, -0.800252, -0.485138));
		previewSphereShapeNormals.append(MVector(-0.485138, -0.800252, -0.352474));
		previewSphereShapeNormals.append(MVector(-0.570314, -0.800252, -0.185306));
		previewSphereShapeNormals.append(MVector(-0.599664, -0.800252, 5.22693e-008));
		previewSphereShapeNormals.append(MVector(-0.570314, -0.800252, 0.185306));
		previewSphereShapeNormals.append(MVector(-0.485138, -0.800252, 0.352474));
		previewSphereShapeNormals.append(MVector(-0.352474, -0.800252, 0.485139));
		previewSphereShapeNormals.append(MVector(-0.185306, -0.800252, 0.570314));
		previewSphereShapeNormals.append(MVector(0, -0.800252, 0.599664));
		previewSphereShapeNormals.append(MVector(0.185306, -0.800252, 0.570314));
		previewSphereShapeNormals.append(MVector(0.352474, -0.800252, 0.485139));
		previewSphereShapeNormals.append(MVector(0.485138, -0.800252, 0.352474));
		previewSphereShapeNormals.append(MVector(0.570314, -0.800252, 0.185306));
		previewSphereShapeNormals.append(MVector(0.599664, -0.800252, 6.27231e-007));
		previewSphereShapeNormals.append(MVector(0.578713, -0.698789, -0.420459));
		previewSphereShapeNormals.append(MVector(0.680318, -0.698789, -0.221048));
		previewSphereShapeNormals.append(MVector(0.420459, -0.698789, -0.578713));
		previewSphereShapeNormals.append(MVector(0.221049, -0.698789, -0.680318));
		previewSphereShapeNormals.append(MVector(-6.3263e-008, -0.698789, -0.715328));
		previewSphereShapeNormals.append(MVector(-0.221049, -0.698789, -0.680318));
		previewSphereShapeNormals.append(MVector(-0.42046, -0.698789, -0.578713));
		previewSphereShapeNormals.append(MVector(-0.578713, -0.698789, -0.420459));
		previewSphereShapeNormals.append(MVector(-0.680318, -0.698788, -0.221049));
		previewSphereShapeNormals.append(MVector(-0.715328, -0.698788, -4.36297e-009));
		previewSphereShapeNormals.append(MVector(-0.680318, -0.698789, 0.221049));
		previewSphereShapeNormals.append(MVector(-0.578713, -0.698788, 0.42046));
		previewSphereShapeNormals.append(MVector(-0.420459, -0.698788, 0.578713));
		previewSphereShapeNormals.append(MVector(-0.221049, -0.698788, 0.680318));
		previewSphereShapeNormals.append(MVector(-6.54445e-009, -0.698788, 0.715328));
		previewSphereShapeNormals.append(MVector(0.221049, -0.698789, 0.680318));
		previewSphereShapeNormals.append(MVector(0.42046, -0.698788, 0.578713));
		previewSphereShapeNormals.append(MVector(0.578713, -0.698788, 0.420459));
		previewSphereShapeNormals.append(MVector(0.680318, -0.698788, 0.221049));
		previewSphereShapeNormals.append(MVector(0.715328, -0.698788, 7.70064e-007));
		previewSphereShapeNormals.append(MVector(0.658808, -0.580401, -0.478652));
		previewSphereShapeNormals.append(MVector(0.774475, -0.580401, -0.251641));
		previewSphereShapeNormals.append(MVector(0.478652, -0.580401, -0.658808));
		previewSphereShapeNormals.append(MVector(0.251642, -0.580401, -0.774475));
		previewSphereShapeNormals.append(MVector(-1.03272e-007, -0.580401, -0.814331));
		previewSphereShapeNormals.append(MVector(-0.251642, -0.580401, -0.774475));
		previewSphereShapeNormals.append(MVector(-0.478652, -0.580401, -0.658808));
		previewSphereShapeNormals.append(MVector(-0.658808, -0.580401, -0.478652));
		previewSphereShapeNormals.append(MVector(-0.774475, -0.580401, -0.251642));
		previewSphereShapeNormals.append(MVector(-0.814331, -0.580401, -2.29493e-008));
		previewSphereShapeNormals.append(MVector(-0.774475, -0.580401, 0.251642));
		previewSphereShapeNormals.append(MVector(-0.658808, -0.580401, 0.478652));
		previewSphereShapeNormals.append(MVector(-0.478652, -0.580401, 0.658808));
		previewSphereShapeNormals.append(MVector(-0.251642, -0.580401, 0.774475));
		previewSphereShapeNormals.append(MVector(7.64978e-009, -0.580401, 0.814331));
		previewSphereShapeNormals.append(MVector(0.251642, -0.580401, 0.774475));
		previewSphereShapeNormals.append(MVector(0.478652, -0.580401, 0.658808));
		previewSphereShapeNormals.append(MVector(0.658808, -0.580401, 0.478652));
		previewSphereShapeNormals.append(MVector(0.774475, -0.580401, 0.251642));
		previewSphereShapeNormals.append(MVector(0.814331, -0.580401, 7.95577e-007));
		previewSphereShapeNormals.append(MVector(0.723291, -0.447994, -0.525502));
		previewSphereShapeNormals.append(MVector(0.85028, -0.447994, -0.276272));
		previewSphereShapeNormals.append(MVector(0.525501, -0.447994, -0.723291));
		previewSphereShapeNormals.append(MVector(0.276272, -0.447994, -0.850279));
		previewSphereShapeNormals.append(MVector(-4.87277e-008, -0.447994, -0.894037));
		previewSphereShapeNormals.append(MVector(-0.276273, -0.447994, -0.850279));
		previewSphereShapeNormals.append(MVector(-0.525502, -0.447994, -0.723291));
		previewSphereShapeNormals.append(MVector(-0.723291, -0.447994, -0.525501));
		previewSphereShapeNormals.append(MVector(-0.850279, -0.447994, -0.276272));
		previewSphereShapeNormals.append(MVector(-0.894037, -0.447994, -3.48055e-008));
		previewSphereShapeNormals.append(MVector(-0.850279, -0.447994, 0.276272));
		previewSphereShapeNormals.append(MVector(-0.723291, -0.447994, 0.525502));
		previewSphereShapeNormals.append(MVector(-0.525502, -0.447994, 0.723291));
		previewSphereShapeNormals.append(MVector(-0.276273, -0.447994, 0.850279));
		previewSphereShapeNormals.append(MVector(1.04416e-007, -0.447994, 0.894037));
		previewSphereShapeNormals.append(MVector(0.276273, -0.447994, 0.850279));
		previewSphereShapeNormals.append(MVector(0.525502, -0.447994, 0.723291));
		previewSphereShapeNormals.append(MVector(0.723291, -0.447994, 0.525502));
		previewSphereShapeNormals.append(MVector(0.85028, -0.447994, 0.276273));
		previewSphereShapeNormals.append(MVector(0.894037, -0.447994, 8.97982e-007));
		previewSphereShapeNormals.append(MVector(0.770524, -0.304789, -0.559819));
		previewSphereShapeNormals.append(MVector(0.905806, -0.304789, -0.294313));
		previewSphereShapeNormals.append(MVector(0.559818, -0.304789, -0.770524));
		previewSphereShapeNormals.append(MVector(0.294314, -0.304789, -0.905805));
		previewSphereShapeNormals.append(MVector(-6.5314e-009, -0.304789, -0.95242));
		previewSphereShapeNormals.append(MVector(-0.294314, -0.304789, -0.905805));
		previewSphereShapeNormals.append(MVector(-0.559819, -0.304789, -0.770524));
		previewSphereShapeNormals.append(MVector(-0.770524, -0.304789, -0.559818));
		previewSphereShapeNormals.append(MVector(-0.905805, -0.304789, -0.294314));
		previewSphereShapeNormals.append(MVector(-0.95242, -0.304789, -1.63285e-008));
		previewSphereShapeNormals.append(MVector(-0.905805, -0.304789, 0.294314));
		previewSphereShapeNormals.append(MVector(-0.770524, -0.304788, 0.559819));
		previewSphereShapeNormals.append(MVector(-0.559818, -0.304788, 0.770524));
		previewSphereShapeNormals.append(MVector(-0.294314, -0.304789, 0.905805));
		previewSphereShapeNormals.append(MVector(1.60019e-007, -0.304789, 0.95242));
		previewSphereShapeNormals.append(MVector(0.294314, -0.304789, 0.905805));
		previewSphereShapeNormals.append(MVector(0.559818, -0.304789, 0.770524));
		previewSphereShapeNormals.append(MVector(0.770524, -0.304789, 0.559818));
		previewSphereShapeNormals.append(MVector(0.905805, -0.304789, 0.294314));
		previewSphereShapeNormals.append(MVector(0.95242, -0.304788, 9.20927e-007));
		previewSphereShapeNormals.append(MVector(0.799335, -0.154249, -0.580751));
		previewSphereShapeNormals.append(MVector(0.939675, -0.154249, -0.305318));
		previewSphereShapeNormals.append(MVector(0.580751, -0.154249, -0.799335));
		previewSphereShapeNormals.append(MVector(0.305319, -0.154249, -0.939674));
		previewSphereShapeNormals.append(MVector(0, -0.154249, -0.988032));
		previewSphereShapeNormals.append(MVector(-0.305319, -0.154249, -0.939674));
		previewSphereShapeNormals.append(MVector(-0.580751, -0.154249, -0.799335));
		previewSphereShapeNormals.append(MVector(-0.799335, -0.154249, -0.580751));
		previewSphereShapeNormals.append(MVector(-0.939674, -0.154248, -0.305319));
		previewSphereShapeNormals.append(MVector(-0.988032, -0.154248, 3.14737e-009));
		previewSphereShapeNormals.append(MVector(-0.939674, -0.154248, 0.305319));
		previewSphereShapeNormals.append(MVector(-0.799335, -0.154248, 0.580751));
		previewSphereShapeNormals.append(MVector(-0.580751, -0.154249, 0.799335));
		previewSphereShapeNormals.append(MVector(-0.305319, -0.154249, 0.939674));
		previewSphereShapeNormals.append(MVector(7.55368e-008, -0.154249, 0.988032));
		previewSphereShapeNormals.append(MVector(0.305319, -0.154249, 0.939674));
		previewSphereShapeNormals.append(MVector(0.580751, -0.154249, 0.799335));
		previewSphereShapeNormals.append(MVector(0.799335, -0.154249, 0.580751));
		previewSphereShapeNormals.append(MVector(0.939674, -0.154249, 0.305319));
		previewSphereShapeNormals.append(MVector(0.988032, -0.154249, 9.15883e-007));
		previewSphereShapeNormals.append(MVector(0.809017, 3.10952e-009, -0.587785));
		previewSphereShapeNormals.append(MVector(0.951057, -1.55476e-009, -0.309016));
		previewSphereShapeNormals.append(MVector(0.587785, -2.17667e-008, -0.809017));
		previewSphereShapeNormals.append(MVector(0.309017, -2.17667e-008, -0.951057));
		previewSphereShapeNormals.append(MVector(0, 0, -1));
		previewSphereShapeNormals.append(MVector(-0.309017, -2.17667e-008, -0.951056));
		previewSphereShapeNormals.append(MVector(-0.587785, -2.02119e-008, -0.809017));
		previewSphereShapeNormals.append(MVector(-0.809017, 3.10953e-009, -0.587785));
		previewSphereShapeNormals.append(MVector(-0.951057, 1.55476e-009, -0.309017));
		previewSphereShapeNormals.append(MVector(-1, -1.55476e-009, 3.10953e-009));
		previewSphereShapeNormals.append(MVector(-0.951056, -3.10953e-009, 0.309017));
		previewSphereShapeNormals.append(MVector(-0.809017, 0, 0.587785));
		previewSphereShapeNormals.append(MVector(-0.587785, -1.86572e-008, 0.809017));
		previewSphereShapeNormals.append(MVector(-0.309017, -2.48762e-008, 0.951057));
		previewSphereShapeNormals.append(MVector(0, 0, 1));
		previewSphereShapeNormals.append(MVector(0.309017, -1.55476e-009, 0.951057));
		previewSphereShapeNormals.append(MVector(0.587785, -3.10953e-009, 0.809017));
		previewSphereShapeNormals.append(MVector(0.809017, -1.55476e-009, 0.587785));
		previewSphereShapeNormals.append(MVector(0.951057, 0, 0.309017));
		previewSphereShapeNormals.append(MVector(1, 1.55476e-009, 9.26639e-007));
		previewSphereShapeNormals.append(MVector(0.799335, 0.154249, -0.580751));
		previewSphereShapeNormals.append(MVector(0.939675, 0.154249, -0.305318));
		previewSphereShapeNormals.append(MVector(0.580751, 0.154249, -0.799335));
		previewSphereShapeNormals.append(MVector(0.305319, 0.154249, -0.939674));
		previewSphereShapeNormals.append(MVector(0, 0.154249, -0.988032));
		previewSphereShapeNormals.append(MVector(-0.305319, 0.154249, -0.939674));
		previewSphereShapeNormals.append(MVector(-0.580751, 0.154249, -0.799335));
		previewSphereShapeNormals.append(MVector(-0.799335, 0.154249, -0.580751));
		previewSphereShapeNormals.append(MVector(-0.939674, 0.154248, -0.305319));
		previewSphereShapeNormals.append(MVector(-0.988032, 0.154248, 6.29473e-009));
		previewSphereShapeNormals.append(MVector(-0.939674, 0.154248, 0.305319));
		previewSphereShapeNormals.append(MVector(-0.799335, 0.154248, 0.580751));
		previewSphereShapeNormals.append(MVector(-0.580751, 0.154249, 0.799335));
		previewSphereShapeNormals.append(MVector(-0.305319, 0.154249, 0.939674));
		previewSphereShapeNormals.append(MVector(7.86841e-008, 0.154249, 0.988032));
		previewSphereShapeNormals.append(MVector(0.305319, 0.154249, 0.939674));
		previewSphereShapeNormals.append(MVector(0.580751, 0.154249, 0.799335));
		previewSphereShapeNormals.append(MVector(0.799335, 0.154249, 0.580751));
		previewSphereShapeNormals.append(MVector(0.939674, 0.154249, 0.305319));
		previewSphereShapeNormals.append(MVector(0.988032, 0.154249, 9.12736e-007));
		previewSphereShapeNormals.append(MVector(0.770524, 0.304789, -0.559819));
		previewSphereShapeNormals.append(MVector(0.905806, 0.304789, -0.294313));
		previewSphereShapeNormals.append(MVector(0.559818, 0.304789, -0.770524));
		previewSphereShapeNormals.append(MVector(0.294314, 0.304789, -0.905805));
		previewSphereShapeNormals.append(MVector(-6.5314e-009, 0.304789, -0.95242));
		previewSphereShapeNormals.append(MVector(-0.294314, 0.304789, -0.905805));
		previewSphereShapeNormals.append(MVector(-0.559819, 0.304789, -0.770524));
		previewSphereShapeNormals.append(MVector(-0.770524, 0.304789, -0.559818));
		previewSphereShapeNormals.append(MVector(-0.905805, 0.304789, -0.294314));
		previewSphereShapeNormals.append(MVector(-0.95242, 0.304789, -9.7971e-009));
		previewSphereShapeNormals.append(MVector(-0.905805, 0.304789, 0.294314));
		previewSphereShapeNormals.append(MVector(-0.770524, 0.304788, 0.559819));
		previewSphereShapeNormals.append(MVector(-0.559818, 0.304788, 0.770524));
		previewSphereShapeNormals.append(MVector(-0.294314, 0.304789, 0.905805));
		previewSphereShapeNormals.append(MVector(1.63285e-007, 0.304789, 0.95242));
		previewSphereShapeNormals.append(MVector(0.294314, 0.304789, 0.905805));
		previewSphereShapeNormals.append(MVector(0.559818, 0.304789, 0.770524));
		previewSphereShapeNormals.append(MVector(0.770524, 0.304789, 0.559818));
		previewSphereShapeNormals.append(MVector(0.905805, 0.304788, 0.294314));
		previewSphereShapeNormals.append(MVector(0.95242, 0.304789, 8.98067e-007));
		previewSphereShapeNormals.append(MVector(0.723291, 0.447994, -0.525502));
		previewSphereShapeNormals.append(MVector(0.85028, 0.447994, -0.276272));
		previewSphereShapeNormals.append(MVector(0.525501, 0.447994, -0.723291));
		previewSphereShapeNormals.append(MVector(0.276272, 0.447994, -0.85028));
		previewSphereShapeNormals.append(MVector(-4.87277e-008, 0.447994, -0.894037));
		previewSphereShapeNormals.append(MVector(-0.276273, 0.447994, -0.850279));
		previewSphereShapeNormals.append(MVector(-0.525502, 0.447994, -0.723291));
		previewSphereShapeNormals.append(MVector(-0.723291, 0.447994, -0.525501));
		previewSphereShapeNormals.append(MVector(-0.85028, 0.447994, -0.276273));
		previewSphereShapeNormals.append(MVector(-0.894037, 0.447994, -2.78444e-008));
		previewSphereShapeNormals.append(MVector(-0.850279, 0.447994, 0.276272));
		previewSphereShapeNormals.append(MVector(-0.723291, 0.447994, 0.525502));
		previewSphereShapeNormals.append(MVector(-0.525502, 0.447994, 0.723291));
		previewSphereShapeNormals.append(MVector(-0.276273, 0.447994, 0.850279));
		previewSphereShapeNormals.append(MVector(1.04416e-007, 0.447994, 0.894037));
		previewSphereShapeNormals.append(MVector(0.276273, 0.447994, 0.850279));
		previewSphereShapeNormals.append(MVector(0.525502, 0.447994, 0.723291));
		previewSphereShapeNormals.append(MVector(0.723291, 0.447994, 0.525502));
		previewSphereShapeNormals.append(MVector(0.85028, 0.447994, 0.276273));
		previewSphereShapeNormals.append(MVector(0.894037, 0.447994, 8.73618e-007));
		previewSphereShapeNormals.append(MVector(0.658808, 0.580401, -0.478652));
		previewSphereShapeNormals.append(MVector(0.774475, 0.580401, -0.251641));
		previewSphereShapeNormals.append(MVector(0.478652, 0.580401, -0.658808));
		previewSphereShapeNormals.append(MVector(0.251642, 0.580401, -0.774475));
		previewSphereShapeNormals.append(MVector(-9.94472e-008, 0.580401, -0.814331));
		previewSphereShapeNormals.append(MVector(-0.251642, 0.580401, -0.774475));
		previewSphereShapeNormals.append(MVector(-0.478652, 0.580401, -0.658808));
		previewSphereShapeNormals.append(MVector(-0.658808, 0.580401, -0.478652));
		previewSphereShapeNormals.append(MVector(-0.774475, 0.580401, -0.251642));
		previewSphereShapeNormals.append(MVector(-0.814331, 0.580401, -2.29493e-008));
		previewSphereShapeNormals.append(MVector(-0.774475, 0.580401, 0.251642));
		previewSphereShapeNormals.append(MVector(-0.658808, 0.580401, 0.478652));
		previewSphereShapeNormals.append(MVector(-0.478652, 0.580401, 0.658808));
		previewSphereShapeNormals.append(MVector(-0.251642, 0.580401, 0.774475));
		previewSphereShapeNormals.append(MVector(5.73734e-009, 0.580401, 0.814331));
		previewSphereShapeNormals.append(MVector(0.251642, 0.580401, 0.774475));
		previewSphereShapeNormals.append(MVector(0.478652, 0.580401, 0.658808));
		previewSphereShapeNormals.append(MVector(0.658808, 0.580401, 0.478652));
		previewSphereShapeNormals.append(MVector(0.774475, 0.580401, 0.251642));
		previewSphereShapeNormals.append(MVector(0.814331, 0.580401, 7.9749e-007));
		previewSphereShapeNormals.append(MVector(0.578712, 0.698789, -0.420459));
		previewSphereShapeNormals.append(MVector(0.680318, 0.698789, -0.221048));
		previewSphereShapeNormals.append(MVector(0.420459, 0.698789, -0.578713));
		previewSphereShapeNormals.append(MVector(0.221049, 0.698789, -0.680318));
		previewSphereShapeNormals.append(MVector(-6.3263e-008, 0.698789, -0.715328));
		previewSphereShapeNormals.append(MVector(-0.221049, 0.698789, -0.680318));
		previewSphereShapeNormals.append(MVector(-0.42046, 0.698789, -0.578713));
		previewSphereShapeNormals.append(MVector(-0.578713, 0.698789, -0.420459));
		previewSphereShapeNormals.append(MVector(-0.680318, 0.698789, -0.221049));
		previewSphereShapeNormals.append(MVector(-0.715328, 0.698788, 0));
		previewSphereShapeNormals.append(MVector(-0.680318, 0.698789, 0.221049));
		previewSphereShapeNormals.append(MVector(-0.578713, 0.698788, 0.42046));
		previewSphereShapeNormals.append(MVector(-0.420459, 0.698788, 0.578713));
		previewSphereShapeNormals.append(MVector(-0.221049, 0.698788, 0.680318));
		previewSphereShapeNormals.append(MVector(-6.54445e-009, 0.698788, 0.715328));
		previewSphereShapeNormals.append(MVector(0.221049, 0.698789, 0.680318));
		previewSphereShapeNormals.append(MVector(0.42046, 0.698788, 0.578713));
		previewSphereShapeNormals.append(MVector(0.578713, 0.698788, 0.42046));
		previewSphereShapeNormals.append(MVector(0.680318, 0.698788, 0.221049));
		previewSphereShapeNormals.append(MVector(0.715328, 0.698788, 7.70064e-007));
		previewSphereShapeNormals.append(MVector(0.485138, 0.800252, -0.352474));
		previewSphereShapeNormals.append(MVector(0.570314, 0.800252, -0.185306));
		previewSphereShapeNormals.append(MVector(0.352473, 0.800252, -0.485139));
		previewSphereShapeNormals.append(MVector(0.185306, 0.800252, -0.570314));
		previewSphereShapeNormals.append(MVector(-3.13615e-008, 0.800252, -0.599664));
		previewSphereShapeNormals.append(MVector(-0.185307, 0.800252, -0.570314));
		previewSphereShapeNormals.append(MVector(-0.352474, 0.800252, -0.485138));
		previewSphereShapeNormals.append(MVector(-0.485138, 0.800252, -0.352474));
		previewSphereShapeNormals.append(MVector(-0.570314, 0.800252, -0.185306));
		previewSphereShapeNormals.append(MVector(-0.599664, 0.800252, 6.01097e-008));
		previewSphereShapeNormals.append(MVector(-0.570314, 0.800252, 0.185306));
		previewSphereShapeNormals.append(MVector(-0.485138, 0.800252, 0.352474));
		previewSphereShapeNormals.append(MVector(-0.352474, 0.800252, 0.485138));
		previewSphereShapeNormals.append(MVector(-0.185306, 0.800252, 0.570314));
		previewSphereShapeNormals.append(MVector(0, 0.800252, 0.599664));
		previewSphereShapeNormals.append(MVector(0.185306, 0.800252, 0.570315));
		previewSphereShapeNormals.append(MVector(0.352474, 0.800252, 0.485139));
		previewSphereShapeNormals.append(MVector(0.485139, 0.800252, 0.352474));
		previewSphereShapeNormals.append(MVector(0.570314, 0.800252, 0.185306));
		previewSphereShapeNormals.append(MVector(0.599664, 0.800252, 6.86034e-007));
		previewSphereShapeNormals.append(MVector(0.380846, 0.882266, -0.276701));
		previewSphereShapeNormals.append(MVector(0.447711, 0.882266, -0.14547));
		previewSphereShapeNormals.append(MVector(0.276701, 0.882266, -0.380846));
		previewSphereShapeNormals.append(MVector(0.14547, 0.882266, -0.447711));
		previewSphereShapeNormals.append(MVector(-6.64238e-008, 0.882266, -0.470752));
		previewSphereShapeNormals.append(MVector(-0.14547, 0.882266, -0.447711));
		previewSphereShapeNormals.append(MVector(-0.276701, 0.882266, -0.380846));
		previewSphereShapeNormals.append(MVector(-0.380846, 0.882266, -0.276701));
		previewSphereShapeNormals.append(MVector(-0.447712, 0.882266, -0.14547));
		previewSphereShapeNormals.append(MVector(-0.470752, 0.882266, 8.91257e-008));
		previewSphereShapeNormals.append(MVector(-0.447712, 0.882266, 0.145471));
		previewSphereShapeNormals.append(MVector(-0.380846, 0.882266, 0.276701));
		previewSphereShapeNormals.append(MVector(-0.276701, 0.882266, 0.380846));
		previewSphereShapeNormals.append(MVector(-0.14547, 0.882266, 0.447712));
		previewSphereShapeNormals.append(MVector(3.9518e-008, 0.882266, 0.470752));
		previewSphereShapeNormals.append(MVector(0.14547, 0.882266, 0.447712));
		previewSphereShapeNormals.append(MVector(0.276701, 0.882266, 0.380846));
		previewSphereShapeNormals.append(MVector(0.380846, 0.882266, 0.276701));
		previewSphereShapeNormals.append(MVector(0.447712, 0.882266, 0.14547));
		previewSphereShapeNormals.append(MVector(0.470752, 0.882266, 2.83352e-007));
		previewSphereShapeNormals.append(MVector(0.26987, 0.942722, -0.196073));
		previewSphereShapeNormals.append(MVector(0.317252, 0.942722, -0.103081));
		previewSphereShapeNormals.append(MVector(0.196072, 0.942722, -0.269871));
		previewSphereShapeNormals.append(MVector(0.103081, 0.942723, -0.317252));
		previewSphereShapeNormals.append(MVector(-5.01225e-008, 0.942723, -0.333578));
		previewSphereShapeNormals.append(MVector(-0.103081, 0.942723, -0.317251));
		previewSphereShapeNormals.append(MVector(-0.196072, 0.942722, -0.26987));
		previewSphereShapeNormals.append(MVector(-0.26987, 0.942723, -0.196072));
		previewSphereShapeNormals.append(MVector(-0.317252, 0.942722, -0.103081));
		previewSphereShapeNormals.append(MVector(-0.333578, 0.942722, 0));
		previewSphereShapeNormals.append(MVector(-0.317252, 0.942722, 0.103082));
		previewSphereShapeNormals.append(MVector(-0.26987, 0.942722, 0.196072));
		previewSphereShapeNormals.append(MVector(-0.196072, 0.942722, 0.26987));
		previewSphereShapeNormals.append(MVector(-0.103081, 0.942722, 0.317252));
		previewSphereShapeNormals.append(MVector(8.55751e-008, 0.942722, 0.333578));
		previewSphereShapeNormals.append(MVector(0.103081, 0.942723, 0.317252));
		previewSphereShapeNormals.append(MVector(0.196072, 0.942722, 0.26987));
		previewSphereShapeNormals.append(MVector(0.26987, 0.942722, 0.196072));
		previewSphereShapeNormals.append(MVector(0.317252, 0.942722, 0.103081));
		previewSphereShapeNormals.append(MVector(0.333578, 0.942723, 2.36554e-007));
		previewSphereShapeNormals.append(MVector(0.162096, 0.979722, -0.117769));
		previewSphereShapeNormals.append(MVector(0.190555, 0.979722, -0.0619148));
		previewSphereShapeNormals.append(MVector(0.117769, 0.979722, -0.162096));
		previewSphereShapeNormals.append(MVector(0.061915, 0.979722, -0.190555));
		previewSphereShapeNormals.append(MVector(-3.482e-008, 0.979722, -0.200361));
		previewSphereShapeNormals.append(MVector(-0.061915, 0.979722, -0.190555));
		previewSphereShapeNormals.append(MVector(-0.117769, 0.979722, -0.162095));
		previewSphereShapeNormals.append(MVector(-0.162096, 0.979722, -0.117769));
		previewSphereShapeNormals.append(MVector(-0.190555, 0.979722, -0.0619151));
		previewSphereShapeNormals.append(MVector(-0.200361, 0.979722, 0));
		previewSphereShapeNormals.append(MVector(-0.190555, 0.979722, 0.0619154));
		previewSphereShapeNormals.append(MVector(-0.162096, 0.979722, 0.117769));
		previewSphereShapeNormals.append(MVector(-0.117769, 0.979722, 0.162096));
		previewSphereShapeNormals.append(MVector(-0.061915, 0.979722, 0.190555));
		previewSphereShapeNormals.append(MVector(3.54078e-008, 0.979722, 0.200361));
		previewSphereShapeNormals.append(MVector(0.0619151, 0.979722, 0.190555));
		previewSphereShapeNormals.append(MVector(0.117769, 0.979722, 0.162096));
		previewSphereShapeNormals.append(MVector(0.162096, 0.979722, 0.117769));
		previewSphereShapeNormals.append(MVector(0.190555, 0.979722, 0.0619151));
		previewSphereShapeNormals.append(MVector(0.200361, 0.979722, 5.30382e-007));
		previewSphereShapeNormals.append(MVector(-3.37938e-009, -1, -1.59522e-007));
		previewSphereShapeNormals.append(MVector(-2.15052e-009, 1, 2.08831e-007));

		meshLightShapeVertices.append(MPoint(-0.5, 0, 0.5));
		meshLightShapeVertices.append(MPoint(0.5, 0, 0.5));
		meshLightShapeVertices.append(MPoint(-0.5, 0, -0.5));
		meshLightShapeVertices.append(MPoint(0.5, 0, -0.5));
		meshLightShapeNormals.append(MVector(0, 1, 0));
		meshLightShapeNormals.append(MVector(0, 1, 0));
		meshLightShapeNormals.append(MVector(0, 1, 0));
		meshLightShapeNormals.append(MVector(0, 1, 0));

		BgShapeVertices.append(MPoint(-5, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(-1.666667, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(1.666667, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(5, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(-5, 2.333333, -5));
		BgShapeVertices.append(MPoint(-1.666667, 2.333333, -5));
		BgShapeVertices.append(MPoint(1.666667, 2.333333, -5));
		BgShapeVertices.append(MPoint(5, 2.333333, -5));
		BgShapeVertices.append(MPoint(-5, 5.666667, -5));
		BgShapeVertices.append(MPoint(-1.666667, 5.666667, -5));
		BgShapeVertices.append(MPoint(1.666667, 5.666667, -5));
		BgShapeVertices.append(MPoint(5, 5.666667, -5));
		BgShapeVertices.append(MPoint(-5, 9, -5));
		BgShapeVertices.append(MPoint(-1.666667, 9, -5));
		BgShapeVertices.append(MPoint(1.666667, 9, -5));
		BgShapeVertices.append(MPoint(5, 9, -5));
		BgShapeVertices.append(MPoint(-5, -1, 5));
		BgShapeVertices.append(MPoint(-1.666667, -1, 5));
		BgShapeVertices.append(MPoint(1.666667, -1, 5));
		BgShapeVertices.append(MPoint(5, -1, 5));
		BgShapeVertices.append(MPoint(-5, -1, 1.666667));
		BgShapeVertices.append(MPoint(-1.666667, -1, 1.666667));
		BgShapeVertices.append(MPoint(1.666667, -1, 1.666667));
		BgShapeVertices.append(MPoint(5, -1, 1.666667));
		BgShapeVertices.append(MPoint(-5, -1, -1.666667));
		BgShapeVertices.append(MPoint(-1.666667, -1, -1.666667));
		BgShapeVertices.append(MPoint(1.666667, -1, -1.666667));
		BgShapeVertices.append(MPoint(5, -1, -1.666667));
		BgShapeVertices.append(MPoint(-5, 4, -5));
		BgShapeVertices.append(MPoint(-1.666667, 4, -5));
		BgShapeVertices.append(MPoint(-3.333333, 5.666667, -5));
		BgShapeVertices.append(MPoint(1.666667, 4, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 5.666667, -5));
		BgShapeVertices.append(MPoint(5, 4, -5));
		BgShapeVertices.append(MPoint(3.333333, 5.666667, -5));
		BgShapeVertices.append(MPoint(-5, 7.333333, -5));
		BgShapeVertices.append(MPoint(-1.666667, 7.333333, -5));
		BgShapeVertices.append(MPoint(-3.333333, 9, -5));
		BgShapeVertices.append(MPoint(1.666667, 7.333333, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 9, -5));
		BgShapeVertices.append(MPoint(5, 7.333333, -5));
		BgShapeVertices.append(MPoint(3.333333, 9, -5));
		BgShapeVertices.append(MPoint(-3.333333, -1, 5));
		BgShapeVertices.append(MPoint(-5, -1, 3.333333));
		BgShapeVertices.append(MPoint(-1.666667, -1, 3.333333));
		BgShapeVertices.append(MPoint(-3.333333, -1, 1.666667));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, 5));
		BgShapeVertices.append(MPoint(1.666667, -1, 3.333333));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, 1.666667));
		BgShapeVertices.append(MPoint(3.333333, -1, 5));
		BgShapeVertices.append(MPoint(5, -1, 3.333333));
		BgShapeVertices.append(MPoint(3.333333, -1, 1.666667));
		BgShapeVertices.append(MPoint(-5, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(-1.666667, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(-3.333333, -1, -1.666667));
		BgShapeVertices.append(MPoint(1.666667, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, -1.666667));
		BgShapeVertices.append(MPoint(5, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(3.333333, -1, -1.666667));
		BgShapeVertices.append(MPoint(-5, -1, -3.333333));
		BgShapeVertices.append(MPoint(-1.666667, -1, -3.333333));
		BgShapeVertices.append(MPoint(1.666667, -1, -3.333333));
		BgShapeVertices.append(MPoint(5, -1, -3.333333));
		BgShapeVertices.append(MPoint(-3.333333, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(-5, 0.666667, -5));
		BgShapeVertices.append(MPoint(-1.666667, 0.666667, -5));
		BgShapeVertices.append(MPoint(-3.333333, 2.333333, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(1.666667, 0.666667, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 2.333333, -5));
		BgShapeVertices.append(MPoint(3.333333, -0.583333, -4.583333));
		BgShapeVertices.append(MPoint(5, 0.666667, -5));
		BgShapeVertices.append(MPoint(3.333333, 2.333333, -5));
		BgShapeVertices.append(MPoint(-3.333333, 0.666667, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 0.666667, -5));
		BgShapeVertices.append(MPoint(3.333333, 0.666667, -5));
		BgShapeVertices.append(MPoint(-3.333333, 4, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 4, -5));
		BgShapeVertices.append(MPoint(3.333333, 4, -5));
		BgShapeVertices.append(MPoint(-3.333333, 7.333333, -5));
		BgShapeVertices.append(MPoint(1.78814e-007, 7.333333, -5));
		BgShapeVertices.append(MPoint(3.333333, 7.333333, -5));
		BgShapeVertices.append(MPoint(-3.333333, -1, 3.333333));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, 3.333333));
		BgShapeVertices.append(MPoint(3.333333, -1, 3.333333));
		BgShapeVertices.append(MPoint(-3.333333, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(3.333333, -1, -1.78814e-007));
		BgShapeVertices.append(MPoint(-3.333333, -1, -3.333333));
		BgShapeVertices.append(MPoint(1.78814e-007, -1, -3.333333));
		BgShapeVertices.append(MPoint(3.333333, -1, -3.333333));

		BgShapeNormals.append(MVector(2.73112e-007, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(1.94213e-007, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(0, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(1.36556e-007, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(9.71063e-008, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0.707107, 0.707107));
		BgShapeNormals.append(MVector(0, 0.141421, 0.98995));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-8.58307e-008, 0, 1));
		BgShapeNormals.append(MVector(-1.71661e-007, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 0, 1));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 1.07288e-008));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, -2.14577e-008));
		BgShapeNormals.append(MVector(0, 1, 1.07288e-008));
		BgShapeNormals.append(MVector(0, 1, -2.14577e-008));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 1, 0));
		BgShapeNormals.append(MVector(0, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(2.42766e-008, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(0, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(0, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(0, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(1.21383e-008, 0.98995, 0.141421));
		BgShapeNormals.append(MVector(0, 0.98995, 0.141421));

		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.05);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.1);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.15);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.2);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.25);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.3);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.35);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.4);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.45);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.5);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.55);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.6);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.65);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.7);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.75);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.8);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.85);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.9);
		previewSphereShapeu.append(0);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.05);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.1);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.15);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.2);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.25);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.3);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.35);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.4);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.45);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.5);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.55);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.6);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.65);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.7);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.75);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.8);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.85);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.9);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.95);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(1);
		previewSphereShapev.append(0.95);
		previewSphereShapeu.append(0.025);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.075);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.125);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.175);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.225);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.275);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.325);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.375);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.425);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.475);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.525);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.575);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.625);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.675);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.725);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.775);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.825);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.875);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.925);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.975);
		previewSphereShapev.append(0);
		previewSphereShapeu.append(0.025);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.075);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.125);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.175);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.225);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.275);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.325);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.375);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.425);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.475);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.525);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.575);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.625);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.675);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.725);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.775);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.825);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.875);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.925);
		previewSphereShapev.append(1);
		previewSphereShapeu.append(0.975);
		previewSphereShapev.append(1);

		meshLightShapeu.append(0);
		meshLightShapev.append(0);
		meshLightShapeu.append(1);
		meshLightShapev.append(0);
		meshLightShapeu.append(0);
		meshLightShapev.append(1);
		meshLightShapeu.append(1);
		meshLightShapev.append(1);

		BgShapeu.append(0.498753);
		BgShapev.append(0.999998);
		BgShapeu.append(0.498753);
		BgShapev.append(0.833332);
		BgShapeu.append(0.583125);
		BgShapev.append(0.833333);
		BgShapeu.append(0.583125);
		BgShapev.append(0.999999);
		BgShapeu.append(0.498753);
		BgShapev.append(0.166667);
		BgShapeu.append(0.498753);
		BgShapev.append(0.666666);
		BgShapeu.append(0.583125);
		BgShapev.append(0.666666);
		BgShapeu.append(0.498753);
		BgShapev.append(1.43051e-006);
		BgShapeu.append(0.415628);
		BgShapev.append(7.15256e-007);
		BgShapeu.append(0.415628);
		BgShapev.append(0.166667);
		BgShapeu.append(0.667186);
		BgShapev.append(0.666667);
		BgShapeu.append(0.667186);
		BgShapev.append(0.833333);
		BgShapeu.append(0.498753);
		BgShapev.append(0.5);
		BgShapeu.append(0.498753);
		BgShapev.append(0.333333);
		BgShapeu.append(0.415628);
		BgShapev.append(0.333333);
		BgShapeu.append(0.667497);
		BgShapev.append(1);
		BgShapeu.append(0.415628);
		BgShapev.append(0.5);
		BgShapeu.append(0.916874);
		BgShapev.append(0.833333);
		BgShapeu.append(0.583125);
		BgShapev.append(0.5);
		BgShapeu.append(0.916874);
		BgShapev.append(1);
		BgShapeu.append(0.916874);
		BgShapev.append(0.666667);
		BgShapeu.append(1);
		BgShapev.append(0.666667);
		BgShapeu.append(0.583125);
		BgShapev.append(0.333333);
		BgShapeu.append(1);
		BgShapev.append(0.833333);
		BgShapeu.append(0.415628);
		BgShapev.append(0.666666);
		BgShapeu.append(0.415628);
		BgShapev.append(0.999999);
		BgShapeu.append(0.667186);
		BgShapev.append(0.333333);
		BgShapeu.append(0.667186);
		BgShapev.append(0.5);
		BgShapeu.append(0.415628);
		BgShapev.append(0.833332);
		BgShapeu.append(0.332502);
		BgShapev.append(0.166667);
		BgShapeu.append(0.332503);
		BgShapev.append(0);
		BgShapeu.append(0.249377);
		BgShapev.append(0);
		BgShapeu.append(0.249377);
		BgShapev.append(0.166667);
		BgShapeu.append(1);
		BgShapev.append(1);
		BgShapeu.append(0.583125);
		BgShapev.append(0.166667);
		BgShapeu.append(0.332502);
		BgShapev.append(0.5);
		BgShapeu.append(0.332503);
		BgShapev.append(0.333333);
		BgShapeu.append(0.833749);
		BgShapev.append(0.166667);
		BgShapeu.append(0.583125);
		BgShapev.append(8.34465e-007);
		BgShapeu.append(0.249377);
		BgShapev.append(0.333333);
		BgShapeu.append(0.249377);
		BgShapev.append(0.5);
		BgShapeu.append(0.332503);
		BgShapev.append(1);
		BgShapeu.append(0.667498);
		BgShapev.append(1.19209e-007);
		BgShapeu.append(0.667186);
		BgShapev.append(0.166667);
		BgShapeu.append(0.332502);
		BgShapev.append(0.833333);
		BgShapeu.append(0.332503);
		BgShapev.append(0.666666);
		BgShapeu.append(0.249377);
		BgShapev.append(0.666666);
		BgShapeu.append(0.249377);
		BgShapev.append(1);
		BgShapeu.append(0.249377);
		BgShapev.append(0.833333);
		BgShapeu.append(0.166251);
		BgShapev.append(0.166667);
		BgShapeu.append(0.750623);
		BgShapev.append(0.833333);
		BgShapeu.append(0.750623);
		BgShapev.append(1);
		BgShapeu.append(0.166251);
		BgShapev.append(0);
		BgShapeu.append(0.0831257);
		BgShapev.append(0);
		BgShapeu.append(0.750623);
		BgShapev.append(0.666667);
		BgShapeu.append(0);
		BgShapev.append(0);
		BgShapeu.append(0.0831257);
		BgShapev.append(0.166667);
		BgShapeu.append(0);
		BgShapev.append(0.166667);
		BgShapeu.append(0.833749);
		BgShapev.append(0.666667);
		BgShapeu.append(0.833749);
		BgShapev.append(0.833333);
		BgShapeu.append(0.166251);
		BgShapev.append(0.5);
		BgShapeu.append(0.166251);
		BgShapev.append(0.333333);
		BgShapeu.append(0.0831257);
		BgShapev.append(0.333333);
		BgShapeu.append(0.833749);
		BgShapev.append(1);
		BgShapeu.append(0);
		BgShapev.append(0.333333);
		BgShapeu.append(0.0831257);
		BgShapev.append(0.5);
		BgShapeu.append(0.750623);
		BgShapev.append(0.5);
		BgShapeu.append(0);
		BgShapev.append(0.5);
		BgShapeu.append(0.166251);
		BgShapev.append(1);
		BgShapeu.append(0.166251);
		BgShapev.append(0.833333);
		BgShapeu.append(0.750623);
		BgShapev.append(0.333333);
		BgShapeu.append(0.166251);
		BgShapev.append(0.666666);
		BgShapeu.append(0.0831257);
		BgShapev.append(0.666666);
		BgShapeu.append(0);
		BgShapev.append(0.666666);
		BgShapeu.append(0.833749);
		BgShapev.append(0.333333);
		BgShapeu.append(0.833749);
		BgShapev.append(0.5);
		BgShapeu.append(0.0831257);
		BgShapev.append(1);
		BgShapeu.append(0.0831257);
		BgShapev.append(0.833333);
		BgShapeu.append(0);
		BgShapev.append(0.833333);
		BgShapeu.append(0);
		BgShapev.append(1);
		BgShapeu.append(1);
		BgShapev.append(0.166667);
		BgShapeu.append(1);
		BgShapev.append(3.57628e-007);
		BgShapeu.append(0.750623);
		BgShapev.append(0.166667);
		BgShapeu.append(0.916874);
		BgShapev.append(2.38419e-007);
		BgShapeu.append(0.916874);
		BgShapev.append(0.166667);
		BgShapeu.append(1);
		BgShapev.append(0.5);
		BgShapeu.append(0.750623);
		BgShapev.append(1.19209e-007);
		BgShapeu.append(1);
		BgShapev.append(0.333333);
		BgShapeu.append(0.916874);
		BgShapev.append(0.333333);
		BgShapeu.append(0.916874);
		BgShapev.append(0.5);
		BgShapeu.append(0.833749);
		BgShapev.append(2.38419e-007);
	}

	//MIntArray triData(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup
	int previewSphereShapetriData[] = {
		0, 1, 20, 0, 1, 3, 0, 1, 21, 0,
		20, 1, 21, 3, 1, 2, 21, 1, 22, 0,
		1, 2, 21, 1, 4, 2, 1, 2, 22, 0,
		21, 2, 22, 2, 4, 5, 22, 2, 23, 0,
		2, 3, 22, 4, 6, 5, 2, 3, 23, 0,
		22, 3, 23, 5, 6, 7, 23, 3, 24, 0,
		3, 4, 23, 6, 8, 7, 3, 4, 24, 0,
		23, 4, 24, 7, 8, 9, 24, 4, 25, 0,
		4, 5, 24, 8, 10, 9, 4, 5, 25, 0,
		24, 5, 25, 9, 10, 11, 25, 5, 26, 0,
		5, 6, 25, 10, 12, 11, 5, 6, 26, 0,
		25, 6, 26, 11, 12, 13, 26, 6, 27, 0,
		6, 7, 26, 12, 14, 13, 6, 7, 27, 0,
		26, 7, 27, 13, 14, 15, 27, 7, 28, 0,
		7, 8, 27, 14, 16, 15, 7, 8, 28, 0,
		27, 8, 28, 15, 16, 17, 28, 8, 29, 0,
		8, 9, 28, 16, 18, 17, 8, 9, 29, 0,
		28, 9, 29, 17, 18, 19, 29, 9, 30, 0,
		9, 10, 29, 18, 20, 19, 9, 10, 30, 0,
		29, 10, 30, 19, 20, 21, 30, 10, 31, 0,
		10, 11, 30, 20, 22, 21, 10, 11, 31, 0,
		30, 11, 31, 21, 22, 23, 31, 11, 32, 0,
		11, 12, 31, 22, 24, 23, 11, 12, 32, 0,
		31, 12, 32, 23, 24, 25, 32, 12, 33, 0,
		12, 13, 32, 24, 26, 25, 12, 13, 33, 0,
		32, 13, 33, 25, 26, 27, 33, 13, 34, 0,
		13, 14, 33, 26, 28, 27, 13, 14, 34, 0,
		33, 14, 34, 27, 28, 29, 34, 14, 35, 0,
		14, 15, 34, 28, 30, 29, 14, 15, 35, 0,
		34, 15, 35, 29, 30, 31, 35, 15, 36, 0,
		15, 16, 35, 30, 32, 31, 15, 16, 36, 0,
		35, 16, 36, 31, 32, 33, 36, 16, 37, 0,
		16, 17, 36, 32, 34, 33, 16, 17, 37, 0,
		36, 17, 37, 33, 34, 35, 37, 17, 38, 0,
		17, 18, 37, 34, 36, 35, 17, 18, 38, 0,
		37, 18, 38, 35, 36, 37, 38, 18, 39, 0,
		18, 19, 38, 36, 38, 37, 18, 19, 39, 0,
		38, 19, 39, 37, 38, 39, 39, 19, 40, 0,
		19, 0, 39, 38, 0, 39, 19, 20, 40, 0,
		39, 0, 20, 39, 0, 3, 40, 20, 41, 0,
		20, 21, 40, 3, 2, 41, 21, 22, 42, 0,
		40, 21, 41, 41, 2, 40, 42, 22, 43, 0,
		21, 22, 41, 2, 5, 40, 22, 23, 43, 0,
		41, 22, 42, 40, 5, 42, 43, 23, 44, 0,
		22, 23, 42, 5, 7, 42, 23, 24, 44, 0,
		42, 23, 43, 42, 7, 43, 44, 24, 45, 0,
		23, 24, 43, 7, 9, 43, 24, 25, 45, 0,
		43, 24, 44, 43, 9, 44, 45, 25, 46, 0,
		24, 25, 44, 9, 11, 44, 25, 26, 46, 0,
		44, 25, 45, 44, 11, 45, 46, 26, 47, 0,
		25, 26, 45, 11, 13, 45, 26, 27, 47, 0,
		45, 26, 46, 45, 13, 46, 47, 27, 48, 0,
		26, 27, 46, 13, 15, 46, 27, 28, 48, 0,
		46, 27, 47, 46, 15, 47, 48, 28, 49, 0,
		27, 28, 47, 15, 17, 47, 28, 29, 49, 0,
		47, 28, 48, 47, 17, 48, 49, 29, 50, 0,
		28, 29, 48, 17, 19, 48, 29, 30, 50, 0,
		48, 29, 49, 48, 19, 49, 50, 30, 51, 0,
		29, 30, 49, 19, 21, 49, 30, 31, 51, 0,
		49, 30, 50, 49, 21, 50, 51, 31, 52, 0,
		30, 31, 50, 21, 23, 50, 31, 32, 52, 0,
		50, 31, 51, 50, 23, 51, 52, 32, 53, 0,
		31, 32, 51, 23, 25, 51, 32, 33, 53, 0,
		51, 32, 52, 51, 25, 52, 53, 33, 54, 0,
		32, 33, 52, 25, 27, 52, 33, 34, 54, 0,
		52, 33, 53, 52, 27, 53, 54, 34, 55, 0,
		33, 34, 53, 27, 29, 53, 34, 35, 55, 0,
		53, 34, 54, 53, 29, 54, 55, 35, 56, 0,
		34, 35, 54, 29, 31, 54, 35, 36, 56, 0,
		54, 35, 55, 54, 31, 55, 56, 36, 57, 0,
		35, 36, 55, 31, 33, 55, 36, 37, 57, 0,
		55, 36, 56, 55, 33, 56, 57, 37, 58, 0,
		36, 37, 56, 33, 35, 56, 37, 38, 58, 0,
		56, 37, 57, 56, 35, 57, 58, 38, 59, 0,
		37, 38, 57, 35, 37, 57, 38, 39, 59, 0,
		57, 38, 58, 57, 37, 58, 59, 39, 60, 0,
		38, 39, 58, 37, 39, 58, 39, 40, 60, 0,
		58, 39, 59, 58, 39, 59, 60, 40, 61, 0,
		39, 20, 59, 39, 3, 59, 40, 41, 61, 0,
		59, 20, 40, 59, 3, 41, 61, 41, 62, 0,
		40, 41, 60, 41, 40, 61, 42, 43, 63, 0,
		60, 41, 61, 61, 40, 60, 63, 43, 64, 0,
		41, 42, 61, 40, 42, 60, 43, 44, 64, 0,
		61, 42, 62, 60, 42, 62, 64, 44, 65, 0,
		42, 43, 62, 42, 43, 62, 44, 45, 65, 0,
		62, 43, 63, 62, 43, 63, 65, 45, 66, 0,
		43, 44, 63, 43, 44, 63, 45, 46, 66, 0,
		63, 44, 64, 63, 44, 64, 66, 46, 67, 0,
		44, 45, 64, 44, 45, 64, 46, 47, 67, 0,
		64, 45, 65, 64, 45, 65, 67, 47, 68, 0,
		45, 46, 65, 45, 46, 65, 47, 48, 68, 0,
		65, 46, 66, 65, 46, 66, 68, 48, 69, 0,
		46, 47, 66, 46, 47, 66, 48, 49, 69, 0,
		66, 47, 67, 66, 47, 67, 69, 49, 70, 0,
		47, 48, 67, 47, 48, 67, 49, 50, 70, 0,
		67, 48, 68, 67, 48, 68, 70, 50, 71, 0,
		48, 49, 68, 48, 49, 68, 50, 51, 71, 0,
		68, 49, 69, 68, 49, 69, 71, 51, 72, 0,
		49, 50, 69, 49, 50, 69, 51, 52, 72, 0,
		69, 50, 70, 69, 50, 70, 72, 52, 73, 0,
		50, 51, 70, 50, 51, 70, 52, 53, 73, 0,
		70, 51, 71, 70, 51, 71, 73, 53, 74, 0,
		51, 52, 71, 51, 52, 71, 53, 54, 74, 0,
		71, 52, 72, 71, 52, 72, 74, 54, 75, 0,
		52, 53, 72, 52, 53, 72, 54, 55, 75, 0,
		72, 53, 73, 72, 53, 73, 75, 55, 76, 0,
		53, 54, 73, 53, 54, 73, 55, 56, 76, 0,
		73, 54, 74, 73, 54, 74, 76, 56, 77, 0,
		54, 55, 74, 54, 55, 74, 56, 57, 77, 0,
		74, 55, 75, 74, 55, 75, 77, 57, 78, 0,
		55, 56, 75, 55, 56, 75, 57, 58, 78, 0,
		75, 56, 76, 75, 56, 76, 78, 58, 79, 0,
		56, 57, 76, 56, 57, 76, 58, 59, 79, 0,
		76, 57, 77, 76, 57, 77, 79, 59, 80, 0,
		57, 58, 77, 57, 58, 77, 59, 60, 80, 0,
		77, 58, 78, 77, 58, 78, 80, 60, 81, 0,
		58, 59, 78, 58, 59, 78, 60, 61, 81, 0,
		78, 59, 79, 78, 59, 79, 81, 61, 82, 0,
		59, 40, 79, 59, 41, 79, 61, 62, 82, 0,
		79, 40, 60, 79, 41, 61, 82, 62, 83, 0,
		60, 61, 80, 61, 60, 81, 63, 64, 84, 0,
		80, 61, 81, 81, 60, 80, 84, 64, 85, 0,
		61, 62, 81, 60, 62, 80, 64, 65, 85, 0,
		81, 62, 82, 80, 62, 82, 85, 65, 86, 0,
		62, 63, 82, 62, 63, 82, 65, 66, 86, 0,
		82, 63, 83, 82, 63, 83, 86, 66, 87, 0,
		63, 64, 83, 63, 64, 83, 66, 67, 87, 0,
		83, 64, 84, 83, 64, 84, 87, 67, 88, 0,
		64, 65, 84, 64, 65, 84, 67, 68, 88, 0,
		84, 65, 85, 84, 65, 85, 88, 68, 89, 0,
		65, 66, 85, 65, 66, 85, 68, 69, 89, 0,
		85, 66, 86, 85, 66, 86, 89, 69, 90, 0,
		66, 67, 86, 66, 67, 86, 69, 70, 90, 0,
		86, 67, 87, 86, 67, 87, 90, 70, 91, 0,
		67, 68, 87, 67, 68, 87, 70, 71, 91, 0,
		87, 68, 88, 87, 68, 88, 91, 71, 92, 0,
		68, 69, 88, 68, 69, 88, 71, 72, 92, 0,
		88, 69, 89, 88, 69, 89, 92, 72, 93, 0,
		69, 70, 89, 69, 70, 89, 72, 73, 93, 0,
		89, 70, 90, 89, 70, 90, 93, 73, 94, 0,
		70, 71, 90, 70, 71, 90, 73, 74, 94, 0,
		90, 71, 91, 90, 71, 91, 94, 74, 95, 0,
		71, 72, 91, 71, 72, 91, 74, 75, 95, 0,
		91, 72, 92, 91, 72, 92, 95, 75, 96, 0,
		72, 73, 92, 72, 73, 92, 75, 76, 96, 0,
		92, 73, 93, 92, 73, 93, 96, 76, 97, 0,
		73, 74, 93, 73, 74, 93, 76, 77, 97, 0,
		93, 74, 94, 93, 74, 94, 97, 77, 98, 0,
		74, 75, 94, 74, 75, 94, 77, 78, 98, 0,
		94, 75, 95, 94, 75, 95, 98, 78, 99, 0,
		75, 76, 95, 75, 76, 95, 78, 79, 99, 0,
		95, 76, 96, 95, 76, 96, 99, 79, 100, 0,
		76, 77, 96, 76, 77, 96, 79, 80, 100, 0,
		96, 77, 97, 96, 77, 97, 100, 80, 101, 0,
		77, 78, 97, 77, 78, 97, 80, 81, 101, 0,
		97, 78, 98, 97, 78, 98, 101, 81, 102, 0,
		78, 79, 98, 78, 79, 98, 81, 82, 102, 0,
		98, 79, 99, 98, 79, 99, 102, 82, 103, 0,
		79, 60, 99, 79, 61, 99, 82, 83, 103, 0,
		99, 60, 80, 99, 61, 81, 103, 83, 104, 0,
		80, 81, 100, 81, 80, 101, 84, 85, 105, 0,
		100, 81, 101, 101, 80, 100, 105, 85, 106, 0,
		81, 82, 101, 80, 82, 100, 85, 86, 106, 0,
		101, 82, 102, 100, 82, 102, 106, 86, 107, 0,
		82, 83, 102, 82, 83, 102, 86, 87, 107, 0,
		102, 83, 103, 102, 83, 103, 107, 87, 108, 0,
		83, 84, 103, 83, 84, 103, 87, 88, 108, 0,
		103, 84, 104, 103, 84, 104, 108, 88, 109, 0,
		84, 85, 104, 84, 85, 104, 88, 89, 109, 0,
		104, 85, 105, 104, 85, 105, 109, 89, 110, 0,
		85, 86, 105, 85, 86, 105, 89, 90, 110, 0,
		105, 86, 106, 105, 86, 106, 110, 90, 111, 0,
		86, 87, 106, 86, 87, 106, 90, 91, 111, 0,
		106, 87, 107, 106, 87, 107, 111, 91, 112, 0,
		87, 88, 107, 87, 88, 107, 91, 92, 112, 0,
		107, 88, 108, 107, 88, 108, 112, 92, 113, 0,
		88, 89, 108, 88, 89, 108, 92, 93, 113, 0,
		108, 89, 109, 108, 89, 109, 113, 93, 114, 0,
		89, 90, 109, 89, 90, 109, 93, 94, 114, 0,
		109, 90, 110, 109, 90, 110, 114, 94, 115, 0,
		90, 91, 110, 90, 91, 110, 94, 95, 115, 0,
		110, 91, 111, 110, 91, 111, 115, 95, 116, 0,
		91, 92, 111, 91, 92, 111, 95, 96, 116, 0,
		111, 92, 112, 111, 92, 112, 116, 96, 117, 0,
		92, 93, 112, 92, 93, 112, 96, 97, 117, 0,
		112, 93, 113, 112, 93, 113, 117, 97, 118, 0,
		93, 94, 113, 93, 94, 113, 97, 98, 118, 0,
		113, 94, 114, 113, 94, 114, 118, 98, 119, 0,
		94, 95, 114, 94, 95, 114, 98, 99, 119, 0,
		114, 95, 115, 114, 95, 115, 119, 99, 120, 0,
		95, 96, 115, 95, 96, 115, 99, 100, 120, 0,
		115, 96, 116, 115, 96, 116, 120, 100, 121, 0,
		96, 97, 116, 96, 97, 116, 100, 101, 121, 0,
		116, 97, 117, 116, 97, 117, 121, 101, 122, 0,
		97, 98, 117, 97, 98, 117, 101, 102, 122, 0,
		117, 98, 118, 117, 98, 118, 122, 102, 123, 0,
		98, 99, 118, 98, 99, 118, 102, 103, 123, 0,
		118, 99, 119, 118, 99, 119, 123, 103, 124, 0,
		99, 80, 119, 99, 81, 119, 103, 104, 124, 0,
		119, 80, 100, 119, 81, 101, 124, 104, 125, 0,
		100, 101, 120, 101, 100, 121, 105, 106, 126, 0,
		120, 101, 121, 121, 100, 120, 126, 106, 127, 0,
		101, 102, 121, 100, 102, 120, 106, 107, 127, 0,
		121, 102, 122, 120, 102, 122, 127, 107, 128, 0,
		102, 103, 122, 102, 103, 122, 107, 108, 128, 0,
		122, 103, 123, 122, 103, 123, 128, 108, 129, 0,
		103, 104, 123, 103, 104, 123, 108, 109, 129, 0,
		123, 104, 124, 123, 104, 124, 129, 109, 130, 0,
		104, 105, 124, 104, 105, 124, 109, 110, 130, 0,
		124, 105, 125, 124, 105, 125, 130, 110, 131, 0,
		105, 106, 125, 105, 106, 125, 110, 111, 131, 0,
		125, 106, 126, 125, 106, 126, 131, 111, 132, 0,
		106, 107, 126, 106, 107, 126, 111, 112, 132, 0,
		126, 107, 127, 126, 107, 127, 132, 112, 133, 0,
		107, 108, 127, 107, 108, 127, 112, 113, 133, 0,
		127, 108, 128, 127, 108, 128, 133, 113, 134, 0,
		108, 109, 128, 108, 109, 128, 113, 114, 134, 0,
		128, 109, 129, 128, 109, 129, 134, 114, 135, 0,
		109, 110, 129, 109, 110, 129, 114, 115, 135, 0,
		129, 110, 130, 129, 110, 130, 135, 115, 136, 0,
		110, 111, 130, 110, 111, 130, 115, 116, 136, 0,
		130, 111, 131, 130, 111, 131, 136, 116, 137, 0,
		111, 112, 131, 111, 112, 131, 116, 117, 137, 0,
		131, 112, 132, 131, 112, 132, 137, 117, 138, 0,
		112, 113, 132, 112, 113, 132, 117, 118, 138, 0,
		132, 113, 133, 132, 113, 133, 138, 118, 139, 0,
		113, 114, 133, 113, 114, 133, 118, 119, 139, 0,
		133, 114, 134, 133, 114, 134, 139, 119, 140, 0,
		114, 115, 134, 114, 115, 134, 119, 120, 140, 0,
		134, 115, 135, 134, 115, 135, 140, 120, 141, 0,
		115, 116, 135, 115, 116, 135, 120, 121, 141, 0,
		135, 116, 136, 135, 116, 136, 141, 121, 142, 0,
		116, 117, 136, 116, 117, 136, 121, 122, 142, 0,
		136, 117, 137, 136, 117, 137, 142, 122, 143, 0,
		117, 118, 137, 117, 118, 137, 122, 123, 143, 0,
		137, 118, 138, 137, 118, 138, 143, 123, 144, 0,
		118, 119, 138, 118, 119, 138, 123, 124, 144, 0,
		138, 119, 139, 138, 119, 139, 144, 124, 145, 0,
		119, 100, 139, 119, 101, 139, 124, 125, 145, 0,
		139, 100, 120, 139, 101, 121, 145, 125, 146, 0,
		120, 121, 140, 121, 120, 141, 126, 127, 147, 0,
		140, 121, 141, 141, 120, 140, 147, 127, 148, 0,
		121, 122, 141, 120, 122, 140, 127, 128, 148, 0,
		141, 122, 142, 140, 122, 142, 148, 128, 149, 0,
		122, 123, 142, 122, 123, 142, 128, 129, 149, 0,
		142, 123, 143, 142, 123, 143, 149, 129, 150, 0,
		123, 124, 143, 123, 124, 143, 129, 130, 150, 0,
		143, 124, 144, 143, 124, 144, 150, 130, 151, 0,
		124, 125, 144, 124, 125, 144, 130, 131, 151, 0,
		144, 125, 145, 144, 125, 145, 151, 131, 152, 0,
		125, 126, 145, 125, 126, 145, 131, 132, 152, 0,
		145, 126, 146, 145, 126, 146, 152, 132, 153, 0,
		126, 127, 146, 126, 127, 146, 132, 133, 153, 0,
		146, 127, 147, 146, 127, 147, 153, 133, 154, 0,
		127, 128, 147, 127, 128, 147, 133, 134, 154, 0,
		147, 128, 148, 147, 128, 148, 154, 134, 155, 0,
		128, 129, 148, 128, 129, 148, 134, 135, 155, 0,
		148, 129, 149, 148, 129, 149, 155, 135, 156, 0,
		129, 130, 149, 129, 130, 149, 135, 136, 156, 0,
		149, 130, 150, 149, 130, 150, 156, 136, 157, 0,
		130, 131, 150, 130, 131, 150, 136, 137, 157, 0,
		150, 131, 151, 150, 131, 151, 157, 137, 158, 0,
		131, 132, 151, 131, 132, 151, 137, 138, 158, 0,
		151, 132, 152, 151, 132, 152, 158, 138, 159, 0,
		132, 133, 152, 132, 133, 152, 138, 139, 159, 0,
		152, 133, 153, 152, 133, 153, 159, 139, 160, 0,
		133, 134, 153, 133, 134, 153, 139, 140, 160, 0,
		153, 134, 154, 153, 134, 154, 160, 140, 161, 0,
		134, 135, 154, 134, 135, 154, 140, 141, 161, 0,
		154, 135, 155, 154, 135, 155, 161, 141, 162, 0,
		135, 136, 155, 135, 136, 155, 141, 142, 162, 0,
		155, 136, 156, 155, 136, 156, 162, 142, 163, 0,
		136, 137, 156, 136, 137, 156, 142, 143, 163, 0,
		156, 137, 157, 156, 137, 157, 163, 143, 164, 0,
		137, 138, 157, 137, 138, 157, 143, 144, 164, 0,
		157, 138, 158, 157, 138, 158, 164, 144, 165, 0,
		138, 139, 158, 138, 139, 158, 144, 145, 165, 0,
		158, 139, 159, 158, 139, 159, 165, 145, 166, 0,
		139, 120, 159, 139, 121, 159, 145, 146, 166, 0,
		159, 120, 140, 159, 121, 141, 166, 146, 167, 0,
		140, 141, 160, 141, 140, 161, 147, 148, 168, 0,
		160, 141, 161, 161, 140, 160, 168, 148, 169, 0,
		141, 142, 161, 140, 142, 160, 148, 149, 169, 0,
		161, 142, 162, 160, 142, 162, 169, 149, 170, 0,
		142, 143, 162, 142, 143, 162, 149, 150, 170, 0,
		162, 143, 163, 162, 143, 163, 170, 150, 171, 0,
		143, 144, 163, 143, 144, 163, 150, 151, 171, 0,
		163, 144, 164, 163, 144, 164, 171, 151, 172, 0,
		144, 145, 164, 144, 145, 164, 151, 152, 172, 0,
		164, 145, 165, 164, 145, 165, 172, 152, 173, 0,
		145, 146, 165, 145, 146, 165, 152, 153, 173, 0,
		165, 146, 166, 165, 146, 166, 173, 153, 174, 0,
		146, 147, 166, 146, 147, 166, 153, 154, 174, 0,
		166, 147, 167, 166, 147, 167, 174, 154, 175, 0,
		147, 148, 167, 147, 148, 167, 154, 155, 175, 0,
		167, 148, 168, 167, 148, 168, 175, 155, 176, 0,
		148, 149, 168, 148, 149, 168, 155, 156, 176, 0,
		168, 149, 169, 168, 149, 169, 176, 156, 177, 0,
		149, 150, 169, 149, 150, 169, 156, 157, 177, 0,
		169, 150, 170, 169, 150, 170, 177, 157, 178, 0,
		150, 151, 170, 150, 151, 170, 157, 158, 178, 0,
		170, 151, 171, 170, 151, 171, 178, 158, 179, 0,
		151, 152, 171, 151, 152, 171, 158, 159, 179, 0,
		171, 152, 172, 171, 152, 172, 179, 159, 180, 0,
		152, 153, 172, 152, 153, 172, 159, 160, 180, 0,
		172, 153, 173, 172, 153, 173, 180, 160, 181, 0,
		153, 154, 173, 153, 154, 173, 160, 161, 181, 0,
		173, 154, 174, 173, 154, 174, 181, 161, 182, 0,
		154, 155, 174, 154, 155, 174, 161, 162, 182, 0,
		174, 155, 175, 174, 155, 175, 182, 162, 183, 0,
		155, 156, 175, 155, 156, 175, 162, 163, 183, 0,
		175, 156, 176, 175, 156, 176, 183, 163, 184, 0,
		156, 157, 176, 156, 157, 176, 163, 164, 184, 0,
		176, 157, 177, 176, 157, 177, 184, 164, 185, 0,
		157, 158, 177, 157, 158, 177, 164, 165, 185, 0,
		177, 158, 178, 177, 158, 178, 185, 165, 186, 0,
		158, 159, 178, 158, 159, 178, 165, 166, 186, 0,
		178, 159, 179, 178, 159, 179, 186, 166, 187, 0,
		159, 140, 179, 159, 141, 179, 166, 167, 187, 0,
		179, 140, 160, 179, 141, 161, 187, 167, 188, 0,
		160, 161, 180, 161, 160, 181, 168, 169, 189, 0,
		180, 161, 181, 181, 160, 180, 189, 169, 190, 0,
		161, 162, 181, 160, 162, 180, 169, 170, 190, 0,
		181, 162, 182, 180, 162, 182, 190, 170, 191, 0,
		162, 163, 182, 162, 163, 182, 170, 171, 191, 0,
		182, 163, 183, 182, 163, 183, 191, 171, 192, 0,
		163, 164, 183, 163, 164, 183, 171, 172, 192, 0,
		183, 164, 184, 183, 164, 184, 192, 172, 193, 0,
		164, 165, 184, 164, 165, 184, 172, 173, 193, 0,
		184, 165, 185, 184, 165, 185, 193, 173, 194, 0,
		165, 166, 185, 165, 166, 185, 173, 174, 194, 0,
		185, 166, 186, 185, 166, 186, 194, 174, 195, 0,
		166, 167, 186, 166, 167, 186, 174, 175, 195, 0,
		186, 167, 187, 186, 167, 187, 195, 175, 196, 0,
		167, 168, 187, 167, 168, 187, 175, 176, 196, 0,
		187, 168, 188, 187, 168, 188, 196, 176, 197, 0,
		168, 169, 188, 168, 169, 188, 176, 177, 197, 0,
		188, 169, 189, 188, 169, 189, 197, 177, 198, 0,
		169, 170, 189, 169, 170, 189, 177, 178, 198, 0,
		189, 170, 190, 189, 170, 190, 198, 178, 199, 0,
		170, 171, 190, 170, 171, 190, 178, 179, 199, 0,
		190, 171, 191, 190, 171, 191, 199, 179, 200, 0,
		171, 172, 191, 171, 172, 191, 179, 180, 200, 0,
		191, 172, 192, 191, 172, 192, 200, 180, 201, 0,
		172, 173, 192, 172, 173, 192, 180, 181, 201, 0,
		192, 173, 193, 192, 173, 193, 201, 181, 202, 0,
		173, 174, 193, 173, 174, 193, 181, 182, 202, 0,
		193, 174, 194, 193, 174, 194, 202, 182, 203, 0,
		174, 175, 194, 174, 175, 194, 182, 183, 203, 0,
		194, 175, 195, 194, 175, 195, 203, 183, 204, 0,
		175, 176, 195, 175, 176, 195, 183, 184, 204, 0,
		195, 176, 196, 195, 176, 196, 204, 184, 205, 0,
		176, 177, 196, 176, 177, 196, 184, 185, 205, 0,
		196, 177, 197, 196, 177, 197, 205, 185, 206, 0,
		177, 178, 197, 177, 178, 197, 185, 186, 206, 0,
		197, 178, 198, 197, 178, 198, 206, 186, 207, 0,
		178, 179, 198, 178, 179, 198, 186, 187, 207, 0,
		198, 179, 199, 198, 179, 199, 207, 187, 208, 0,
		179, 160, 199, 179, 161, 199, 187, 188, 208, 0,
		199, 160, 180, 199, 161, 181, 208, 188, 209, 0,
		180, 181, 200, 181, 180, 201, 189, 190, 210, 0,
		200, 181, 201, 201, 180, 200, 210, 190, 211, 0,
		181, 182, 201, 180, 182, 200, 190, 191, 211, 0,
		201, 182, 202, 200, 182, 202, 211, 191, 212, 0,
		182, 183, 202, 182, 183, 202, 191, 192, 212, 0,
		202, 183, 203, 202, 183, 203, 212, 192, 213, 0,
		183, 184, 203, 183, 184, 203, 192, 193, 213, 0,
		203, 184, 204, 203, 184, 204, 213, 193, 214, 0,
		184, 185, 204, 184, 185, 204, 193, 194, 214, 0,
		204, 185, 205, 204, 185, 205, 214, 194, 215, 0,
		185, 186, 205, 185, 186, 205, 194, 195, 215, 0,
		205, 186, 206, 205, 186, 206, 215, 195, 216, 0,
		186, 187, 206, 186, 187, 206, 195, 196, 216, 0,
		206, 187, 207, 206, 187, 207, 216, 196, 217, 0,
		187, 188, 207, 187, 188, 207, 196, 197, 217, 0,
		207, 188, 208, 207, 188, 208, 217, 197, 218, 0,
		188, 189, 208, 188, 189, 208, 197, 198, 218, 0,
		208, 189, 209, 208, 189, 209, 218, 198, 219, 0,
		189, 190, 209, 189, 190, 209, 198, 199, 219, 0,
		209, 190, 210, 209, 190, 210, 219, 199, 220, 0,
		190, 191, 210, 190, 191, 210, 199, 200, 220, 0,
		210, 191, 211, 210, 191, 211, 220, 200, 221, 0,
		191, 192, 211, 191, 192, 211, 200, 201, 221, 0,
		211, 192, 212, 211, 192, 212, 221, 201, 222, 0,
		192, 193, 212, 192, 193, 212, 201, 202, 222, 0,
		212, 193, 213, 212, 193, 213, 222, 202, 223, 0,
		193, 194, 213, 193, 194, 213, 202, 203, 223, 0,
		213, 194, 214, 213, 194, 214, 223, 203, 224, 0,
		194, 195, 214, 194, 195, 214, 203, 204, 224, 0,
		214, 195, 215, 214, 195, 215, 224, 204, 225, 0,
		195, 196, 215, 195, 196, 215, 204, 205, 225, 0,
		215, 196, 216, 215, 196, 216, 225, 205, 226, 0,
		196, 197, 216, 196, 197, 216, 205, 206, 226, 0,
		216, 197, 217, 216, 197, 217, 226, 206, 227, 0,
		197, 198, 217, 197, 198, 217, 206, 207, 227, 0,
		217, 198, 218, 217, 198, 218, 227, 207, 228, 0,
		198, 199, 218, 198, 199, 218, 207, 208, 228, 0,
		218, 199, 219, 218, 199, 219, 228, 208, 229, 0,
		199, 180, 219, 199, 181, 219, 208, 209, 229, 0,
		219, 180, 200, 219, 181, 201, 229, 209, 230, 0,
		200, 201, 220, 201, 200, 221, 210, 211, 231, 0,
		220, 201, 221, 221, 200, 220, 231, 211, 232, 0,
		201, 202, 221, 200, 202, 220, 211, 212, 232, 0,
		221, 202, 222, 220, 202, 222, 232, 212, 233, 0,
		202, 203, 222, 202, 203, 222, 212, 213, 233, 0,
		222, 203, 223, 222, 203, 223, 233, 213, 234, 0,
		203, 204, 223, 203, 204, 223, 213, 214, 234, 0,
		223, 204, 224, 223, 204, 224, 234, 214, 235, 0,
		204, 205, 224, 204, 205, 224, 214, 215, 235, 0,
		224, 205, 225, 224, 205, 225, 235, 215, 236, 0,
		205, 206, 225, 205, 206, 225, 215, 216, 236, 0,
		225, 206, 226, 225, 206, 226, 236, 216, 237, 0,
		206, 207, 226, 206, 207, 226, 216, 217, 237, 0,
		226, 207, 227, 226, 207, 227, 237, 217, 238, 0,
		207, 208, 227, 207, 208, 227, 217, 218, 238, 0,
		227, 208, 228, 227, 208, 228, 238, 218, 239, 0,
		208, 209, 228, 208, 209, 228, 218, 219, 239, 0,
		228, 209, 229, 228, 209, 229, 239, 219, 240, 0,
		209, 210, 229, 209, 210, 229, 219, 220, 240, 0,
		229, 210, 230, 229, 210, 230, 240, 220, 241, 0,
		210, 211, 230, 210, 211, 230, 220, 221, 241, 0,
		230, 211, 231, 230, 211, 231, 241, 221, 242, 0,
		211, 212, 231, 211, 212, 231, 221, 222, 242, 0,
		231, 212, 232, 231, 212, 232, 242, 222, 243, 0,
		212, 213, 232, 212, 213, 232, 222, 223, 243, 0,
		232, 213, 233, 232, 213, 233, 243, 223, 244, 0,
		213, 214, 233, 213, 214, 233, 223, 224, 244, 0,
		233, 214, 234, 233, 214, 234, 244, 224, 245, 0,
		214, 215, 234, 214, 215, 234, 224, 225, 245, 0,
		234, 215, 235, 234, 215, 235, 245, 225, 246, 0,
		215, 216, 235, 215, 216, 235, 225, 226, 246, 0,
		235, 216, 236, 235, 216, 236, 246, 226, 247, 0,
		216, 217, 236, 216, 217, 236, 226, 227, 247, 0,
		236, 217, 237, 236, 217, 237, 247, 227, 248, 0,
		217, 218, 237, 217, 218, 237, 227, 228, 248, 0,
		237, 218, 238, 237, 218, 238, 248, 228, 249, 0,
		218, 219, 238, 218, 219, 238, 228, 229, 249, 0,
		238, 219, 239, 238, 219, 239, 249, 229, 250, 0,
		219, 200, 239, 219, 201, 239, 229, 230, 250, 0,
		239, 200, 220, 239, 201, 221, 250, 230, 251, 0,
		220, 221, 240, 221, 220, 241, 231, 232, 252, 0,
		240, 221, 241, 241, 220, 240, 252, 232, 253, 0,
		221, 222, 241, 220, 222, 240, 232, 233, 253, 0,
		241, 222, 242, 240, 222, 242, 253, 233, 254, 0,
		222, 223, 242, 222, 223, 242, 233, 234, 254, 0,
		242, 223, 243, 242, 223, 243, 254, 234, 255, 0,
		223, 224, 243, 223, 224, 243, 234, 235, 255, 0,
		243, 224, 244, 243, 224, 244, 255, 235, 256, 0,
		224, 225, 244, 224, 225, 244, 235, 236, 256, 0,
		244, 225, 245, 244, 225, 245, 256, 236, 257, 0,
		225, 226, 245, 225, 226, 245, 236, 237, 257, 0,
		245, 226, 246, 245, 226, 246, 257, 237, 258, 0,
		226, 227, 246, 226, 227, 246, 237, 238, 258, 0,
		246, 227, 247, 246, 227, 247, 258, 238, 259, 0,
		227, 228, 247, 227, 228, 247, 238, 239, 259, 0,
		247, 228, 248, 247, 228, 248, 259, 239, 260, 0,
		228, 229, 248, 228, 229, 248, 239, 240, 260, 0,
		248, 229, 249, 248, 229, 249, 260, 240, 261, 0,
		229, 230, 249, 229, 230, 249, 240, 241, 261, 0,
		249, 230, 250, 249, 230, 250, 261, 241, 262, 0,
		230, 231, 250, 230, 231, 250, 241, 242, 262, 0,
		250, 231, 251, 250, 231, 251, 262, 242, 263, 0,
		231, 232, 251, 231, 232, 251, 242, 243, 263, 0,
		251, 232, 252, 251, 232, 252, 263, 243, 264, 0,
		232, 233, 252, 232, 233, 252, 243, 244, 264, 0,
		252, 233, 253, 252, 233, 253, 264, 244, 265, 0,
		233, 234, 253, 233, 234, 253, 244, 245, 265, 0,
		253, 234, 254, 253, 234, 254, 265, 245, 266, 0,
		234, 235, 254, 234, 235, 254, 245, 246, 266, 0,
		254, 235, 255, 254, 235, 255, 266, 246, 267, 0,
		235, 236, 255, 235, 236, 255, 246, 247, 267, 0,
		255, 236, 256, 255, 236, 256, 267, 247, 268, 0,
		236, 237, 256, 236, 237, 256, 247, 248, 268, 0,
		256, 237, 257, 256, 237, 257, 268, 248, 269, 0,
		237, 238, 257, 237, 238, 257, 248, 249, 269, 0,
		257, 238, 258, 257, 238, 258, 269, 249, 270, 0,
		238, 239, 258, 238, 239, 258, 249, 250, 270, 0,
		258, 239, 259, 258, 239, 259, 270, 250, 271, 0,
		239, 220, 259, 239, 221, 259, 250, 251, 271, 0,
		259, 220, 240, 259, 221, 241, 271, 251, 272, 0,
		240, 241, 260, 241, 240, 261, 252, 253, 273, 0,
		260, 241, 261, 261, 240, 260, 273, 253, 274, 0,
		241, 242, 261, 240, 242, 260, 253, 254, 274, 0,
		261, 242, 262, 260, 242, 262, 274, 254, 275, 0,
		242, 243, 262, 242, 243, 262, 254, 255, 275, 0,
		262, 243, 263, 262, 243, 263, 275, 255, 276, 0,
		243, 244, 263, 243, 244, 263, 255, 256, 276, 0,
		263, 244, 264, 263, 244, 264, 276, 256, 277, 0,
		244, 245, 264, 244, 245, 264, 256, 257, 277, 0,
		264, 245, 265, 264, 245, 265, 277, 257, 278, 0,
		245, 246, 265, 245, 246, 265, 257, 258, 278, 0,
		265, 246, 266, 265, 246, 266, 278, 258, 279, 0,
		246, 247, 266, 246, 247, 266, 258, 259, 279, 0,
		266, 247, 267, 266, 247, 267, 279, 259, 280, 0,
		247, 248, 267, 247, 248, 267, 259, 260, 280, 0,
		267, 248, 268, 267, 248, 268, 280, 260, 281, 0,
		248, 249, 268, 248, 249, 268, 260, 261, 281, 0,
		268, 249, 269, 268, 249, 269, 281, 261, 282, 0,
		249, 250, 269, 249, 250, 269, 261, 262, 282, 0,
		269, 250, 270, 269, 250, 270, 282, 262, 283, 0,
		250, 251, 270, 250, 251, 270, 262, 263, 283, 0,
		270, 251, 271, 270, 251, 271, 283, 263, 284, 0,
		251, 252, 271, 251, 252, 271, 263, 264, 284, 0,
		271, 252, 272, 271, 252, 272, 284, 264, 285, 0,
		252, 253, 272, 252, 253, 272, 264, 265, 285, 0,
		272, 253, 273, 272, 253, 273, 285, 265, 286, 0,
		253, 254, 273, 253, 254, 273, 265, 266, 286, 0,
		273, 254, 274, 273, 254, 274, 286, 266, 287, 0,
		254, 255, 274, 254, 255, 274, 266, 267, 287, 0,
		274, 255, 275, 274, 255, 275, 287, 267, 288, 0,
		255, 256, 275, 255, 256, 275, 267, 268, 288, 0,
		275, 256, 276, 275, 256, 276, 288, 268, 289, 0,
		256, 257, 276, 256, 257, 276, 268, 269, 289, 0,
		276, 257, 277, 276, 257, 277, 289, 269, 290, 0,
		257, 258, 277, 257, 258, 277, 269, 270, 290, 0,
		277, 258, 278, 277, 258, 278, 290, 270, 291, 0,
		258, 259, 278, 258, 259, 278, 270, 271, 291, 0,
		278, 259, 279, 278, 259, 279, 291, 271, 292, 0,
		259, 240, 279, 259, 241, 279, 271, 272, 292, 0,
		279, 240, 260, 279, 241, 261, 292, 272, 293, 0,
		260, 261, 280, 261, 260, 281, 273, 274, 294, 0,
		280, 261, 281, 281, 260, 280, 294, 274, 295, 0,
		261, 262, 281, 260, 262, 280, 274, 275, 295, 0,
		281, 262, 282, 280, 262, 282, 295, 275, 296, 0,
		262, 263, 282, 262, 263, 282, 275, 276, 296, 0,
		282, 263, 283, 282, 263, 283, 296, 276, 297, 0,
		263, 264, 283, 263, 264, 283, 276, 277, 297, 0,
		283, 264, 284, 283, 264, 284, 297, 277, 298, 0,
		264, 265, 284, 264, 265, 284, 277, 278, 298, 0,
		284, 265, 285, 284, 265, 285, 298, 278, 299, 0,
		265, 266, 285, 265, 266, 285, 278, 279, 299, 0,
		285, 266, 286, 285, 266, 286, 299, 279, 300, 0,
		266, 267, 286, 266, 267, 286, 279, 280, 300, 0,
		286, 267, 287, 286, 267, 287, 300, 280, 301, 0,
		267, 268, 287, 267, 268, 287, 280, 281, 301, 0,
		287, 268, 288, 287, 268, 288, 301, 281, 302, 0,
		268, 269, 288, 268, 269, 288, 281, 282, 302, 0,
		288, 269, 289, 288, 269, 289, 302, 282, 303, 0,
		269, 270, 289, 269, 270, 289, 282, 283, 303, 0,
		289, 270, 290, 289, 270, 290, 303, 283, 304, 0,
		270, 271, 290, 270, 271, 290, 283, 284, 304, 0,
		290, 271, 291, 290, 271, 291, 304, 284, 305, 0,
		271, 272, 291, 271, 272, 291, 284, 285, 305, 0,
		291, 272, 292, 291, 272, 292, 305, 285, 306, 0,
		272, 273, 292, 272, 273, 292, 285, 286, 306, 0,
		292, 273, 293, 292, 273, 293, 306, 286, 307, 0,
		273, 274, 293, 273, 274, 293, 286, 287, 307, 0,
		293, 274, 294, 293, 274, 294, 307, 287, 308, 0,
		274, 275, 294, 274, 275, 294, 287, 288, 308, 0,
		294, 275, 295, 294, 275, 295, 308, 288, 309, 0,
		275, 276, 295, 275, 276, 295, 288, 289, 309, 0,
		295, 276, 296, 295, 276, 296, 309, 289, 310, 0,
		276, 277, 296, 276, 277, 296, 289, 290, 310, 0,
		296, 277, 297, 296, 277, 297, 310, 290, 311, 0,
		277, 278, 297, 277, 278, 297, 290, 291, 311, 0,
		297, 278, 298, 297, 278, 298, 311, 291, 312, 0,
		278, 279, 298, 278, 279, 298, 291, 292, 312, 0,
		298, 279, 299, 298, 279, 299, 312, 292, 313, 0,
		279, 260, 299, 279, 261, 299, 292, 293, 313, 0,
		299, 260, 280, 299, 261, 281, 313, 293, 314, 0,
		280, 281, 300, 281, 280, 301, 294, 295, 315, 0,
		300, 281, 301, 301, 280, 300, 315, 295, 316, 0,
		281, 282, 301, 280, 282, 300, 295, 296, 316, 0,
		301, 282, 302, 300, 282, 302, 316, 296, 317, 0,
		282, 283, 302, 282, 283, 302, 296, 297, 317, 0,
		302, 283, 303, 302, 283, 303, 317, 297, 318, 0,
		283, 284, 303, 283, 284, 303, 297, 298, 318, 0,
		303, 284, 304, 303, 284, 304, 318, 298, 319, 0,
		284, 285, 304, 284, 285, 304, 298, 299, 319, 0,
		304, 285, 305, 304, 285, 305, 319, 299, 320, 0,
		285, 286, 305, 285, 286, 305, 299, 300, 320, 0,
		305, 286, 306, 305, 286, 306, 320, 300, 321, 0,
		286, 287, 306, 286, 287, 306, 300, 301, 321, 0,
		306, 287, 307, 306, 287, 307, 321, 301, 322, 0,
		287, 288, 307, 287, 288, 307, 301, 302, 322, 0,
		307, 288, 308, 307, 288, 308, 322, 302, 323, 0,
		288, 289, 308, 288, 289, 308, 302, 303, 323, 0,
		308, 289, 309, 308, 289, 309, 323, 303, 324, 0,
		289, 290, 309, 289, 290, 309, 303, 304, 324, 0,
		309, 290, 310, 309, 290, 310, 324, 304, 325, 0,
		290, 291, 310, 290, 291, 310, 304, 305, 325, 0,
		310, 291, 311, 310, 291, 311, 325, 305, 326, 0,
		291, 292, 311, 291, 292, 311, 305, 306, 326, 0,
		311, 292, 312, 311, 292, 312, 326, 306, 327, 0,
		292, 293, 312, 292, 293, 312, 306, 307, 327, 0,
		312, 293, 313, 312, 293, 313, 327, 307, 328, 0,
		293, 294, 313, 293, 294, 313, 307, 308, 328, 0,
		313, 294, 314, 313, 294, 314, 328, 308, 329, 0,
		294, 295, 314, 294, 295, 314, 308, 309, 329, 0,
		314, 295, 315, 314, 295, 315, 329, 309, 330, 0,
		295, 296, 315, 295, 296, 315, 309, 310, 330, 0,
		315, 296, 316, 315, 296, 316, 330, 310, 331, 0,
		296, 297, 316, 296, 297, 316, 310, 311, 331, 0,
		316, 297, 317, 316, 297, 317, 331, 311, 332, 0,
		297, 298, 317, 297, 298, 317, 311, 312, 332, 0,
		317, 298, 318, 317, 298, 318, 332, 312, 333, 0,
		298, 299, 318, 298, 299, 318, 312, 313, 333, 0,
		318, 299, 319, 318, 299, 319, 333, 313, 334, 0,
		299, 280, 319, 299, 281, 319, 313, 314, 334, 0,
		319, 280, 300, 319, 281, 301, 334, 314, 335, 0,
		300, 301, 320, 301, 300, 321, 315, 316, 336, 0,
		320, 301, 321, 321, 300, 320, 336, 316, 337, 0,
		301, 302, 321, 300, 302, 320, 316, 317, 337, 0,
		321, 302, 322, 320, 302, 322, 337, 317, 338, 0,
		302, 303, 322, 302, 303, 322, 317, 318, 338, 0,
		322, 303, 323, 322, 303, 323, 338, 318, 339, 0,
		303, 304, 323, 303, 304, 323, 318, 319, 339, 0,
		323, 304, 324, 323, 304, 324, 339, 319, 340, 0,
		304, 305, 324, 304, 305, 324, 319, 320, 340, 0,
		324, 305, 325, 324, 305, 325, 340, 320, 341, 0,
		305, 306, 325, 305, 306, 325, 320, 321, 341, 0,
		325, 306, 326, 325, 306, 326, 341, 321, 342, 0,
		306, 307, 326, 306, 307, 326, 321, 322, 342, 0,
		326, 307, 327, 326, 307, 327, 342, 322, 343, 0,
		307, 308, 327, 307, 308, 327, 322, 323, 343, 0,
		327, 308, 328, 327, 308, 328, 343, 323, 344, 0,
		308, 309, 328, 308, 309, 328, 323, 324, 344, 0,
		328, 309, 329, 328, 309, 329, 344, 324, 345, 0,
		309, 310, 329, 309, 310, 329, 324, 325, 345, 0,
		329, 310, 330, 329, 310, 330, 345, 325, 346, 0,
		310, 311, 330, 310, 311, 330, 325, 326, 346, 0,
		330, 311, 331, 330, 311, 331, 346, 326, 347, 0,
		311, 312, 331, 311, 312, 331, 326, 327, 347, 0,
		331, 312, 332, 331, 312, 332, 347, 327, 348, 0,
		312, 313, 332, 312, 313, 332, 327, 328, 348, 0,
		332, 313, 333, 332, 313, 333, 348, 328, 349, 0,
		313, 314, 333, 313, 314, 333, 328, 329, 349, 0,
		333, 314, 334, 333, 314, 334, 349, 329, 350, 0,
		314, 315, 334, 314, 315, 334, 329, 330, 350, 0,
		334, 315, 335, 334, 315, 335, 350, 330, 351, 0,
		315, 316, 335, 315, 316, 335, 330, 331, 351, 0,
		335, 316, 336, 335, 316, 336, 351, 331, 352, 0,
		316, 317, 336, 316, 317, 336, 331, 332, 352, 0,
		336, 317, 337, 336, 317, 337, 352, 332, 353, 0,
		317, 318, 337, 317, 318, 337, 332, 333, 353, 0,
		337, 318, 338, 337, 318, 338, 353, 333, 354, 0,
		318, 319, 338, 318, 319, 338, 333, 334, 354, 0,
		338, 319, 339, 338, 319, 339, 354, 334, 355, 0,
		319, 300, 339, 319, 301, 339, 334, 335, 355, 0,
		339, 300, 320, 339, 301, 321, 355, 335, 356, 0,
		320, 321, 340, 321, 320, 341, 336, 337, 357, 0,
		340, 321, 341, 341, 320, 340, 357, 337, 358, 0,
		321, 322, 341, 320, 322, 340, 337, 338, 358, 0,
		341, 322, 342, 340, 322, 342, 358, 338, 359, 0,
		322, 323, 342, 322, 323, 342, 338, 339, 359, 0,
		342, 323, 343, 342, 323, 343, 359, 339, 360, 0,
		323, 324, 343, 323, 324, 343, 339, 340, 360, 0,
		343, 324, 344, 343, 324, 344, 360, 340, 361, 0,
		324, 325, 344, 324, 325, 344, 340, 341, 361, 0,
		344, 325, 345, 344, 325, 345, 361, 341, 362, 0,
		325, 326, 345, 325, 326, 345, 341, 342, 362, 0,
		345, 326, 346, 345, 326, 346, 362, 342, 363, 0,
		326, 327, 346, 326, 327, 346, 342, 343, 363, 0,
		346, 327, 347, 346, 327, 347, 363, 343, 364, 0,
		327, 328, 347, 327, 328, 347, 343, 344, 364, 0,
		347, 328, 348, 347, 328, 348, 364, 344, 365, 0,
		328, 329, 348, 328, 329, 348, 344, 345, 365, 0,
		348, 329, 349, 348, 329, 349, 365, 345, 366, 0,
		329, 330, 349, 329, 330, 349, 345, 346, 366, 0,
		349, 330, 350, 349, 330, 350, 366, 346, 367, 0,
		330, 331, 350, 330, 331, 350, 346, 347, 367, 0,
		350, 331, 351, 350, 331, 351, 367, 347, 368, 0,
		331, 332, 351, 331, 332, 351, 347, 348, 368, 0,
		351, 332, 352, 351, 332, 352, 368, 348, 369, 0,
		332, 333, 352, 332, 333, 352, 348, 349, 369, 0,
		352, 333, 353, 352, 333, 353, 369, 349, 370, 0,
		333, 334, 353, 333, 334, 353, 349, 350, 370, 0,
		353, 334, 354, 353, 334, 354, 370, 350, 371, 0,
		334, 335, 354, 334, 335, 354, 350, 351, 371, 0,
		354, 335, 355, 354, 335, 355, 371, 351, 372, 0,
		335, 336, 355, 335, 336, 355, 351, 352, 372, 0,
		355, 336, 356, 355, 336, 356, 372, 352, 373, 0,
		336, 337, 356, 336, 337, 356, 352, 353, 373, 0,
		356, 337, 357, 356, 337, 357, 373, 353, 374, 0,
		337, 338, 357, 337, 338, 357, 353, 354, 374, 0,
		357, 338, 358, 357, 338, 358, 374, 354, 375, 0,
		338, 339, 358, 338, 339, 358, 354, 355, 375, 0,
		358, 339, 359, 358, 339, 359, 375, 355, 376, 0,
		339, 320, 359, 339, 321, 359, 355, 356, 376, 0,
		359, 320, 340, 359, 321, 341, 376, 356, 377, 0,
		340, 341, 360, 341, 340, 361, 357, 358, 378, 0,
		360, 341, 361, 361, 340, 360, 378, 358, 379, 0,
		341, 342, 361, 340, 342, 360, 358, 359, 379, 0,
		361, 342, 362, 360, 342, 362, 379, 359, 380, 0,
		342, 343, 362, 342, 343, 362, 359, 360, 380, 0,
		362, 343, 363, 362, 343, 363, 380, 360, 381, 0,
		343, 344, 363, 343, 344, 363, 360, 361, 381, 0,
		363, 344, 364, 363, 344, 364, 381, 361, 382, 0,
		344, 345, 364, 344, 345, 364, 361, 362, 382, 0,
		364, 345, 365, 364, 345, 365, 382, 362, 383, 0,
		345, 346, 365, 345, 346, 365, 362, 363, 383, 0,
		365, 346, 366, 365, 346, 366, 383, 363, 384, 0,
		346, 347, 366, 346, 347, 366, 363, 364, 384, 0,
		366, 347, 367, 366, 347, 367, 384, 364, 385, 0,
		347, 348, 367, 347, 348, 367, 364, 365, 385, 0,
		367, 348, 368, 367, 348, 368, 385, 365, 386, 0,
		348, 349, 368, 348, 349, 368, 365, 366, 386, 0,
		368, 349, 369, 368, 349, 369, 386, 366, 387, 0,
		349, 350, 369, 349, 350, 369, 366, 367, 387, 0,
		369, 350, 370, 369, 350, 370, 387, 367, 388, 0,
		350, 351, 370, 350, 351, 370, 367, 368, 388, 0,
		370, 351, 371, 370, 351, 371, 388, 368, 389, 0,
		351, 352, 371, 351, 352, 371, 368, 369, 389, 0,
		371, 352, 372, 371, 352, 372, 389, 369, 390, 0,
		352, 353, 372, 352, 353, 372, 369, 370, 390, 0,
		372, 353, 373, 372, 353, 373, 390, 370, 391, 0,
		353, 354, 373, 353, 354, 373, 370, 371, 391, 0,
		373, 354, 374, 373, 354, 374, 391, 371, 392, 0,
		354, 355, 374, 354, 355, 374, 371, 372, 392, 0,
		374, 355, 375, 374, 355, 375, 392, 372, 393, 0,
		355, 356, 375, 355, 356, 375, 372, 373, 393, 0,
		375, 356, 376, 375, 356, 376, 393, 373, 394, 0,
		356, 357, 376, 356, 357, 376, 373, 374, 394, 0,
		376, 357, 377, 376, 357, 377, 394, 374, 395, 0,
		357, 358, 377, 357, 358, 377, 374, 375, 395, 0,
		377, 358, 378, 377, 358, 378, 395, 375, 396, 0,
		358, 359, 378, 358, 359, 378, 375, 376, 396, 0,
		378, 359, 379, 378, 359, 379, 396, 376, 397, 0,
		359, 340, 379, 359, 341, 379, 376, 377, 397, 0,
		379, 340, 360, 379, 341, 361, 397, 377, 398, 0,
		1, 0, 380, 1, 0, 380, 1, 0, 399, 0,
		2, 1, 380, 4, 1, 380, 2, 1, 400, 0,
		3, 2, 380, 6, 4, 380, 3, 2, 401, 0,
		4, 3, 380, 8, 6, 380, 4, 3, 402, 0,
		5, 4, 380, 10, 8, 380, 5, 4, 403, 0,
		6, 5, 380, 12, 10, 380, 6, 5, 404, 0,
		7, 6, 380, 14, 12, 380, 7, 6, 405, 0,
		8, 7, 380, 16, 14, 380, 8, 7, 406, 0,
		9, 8, 380, 18, 16, 380, 9, 8, 407, 0,
		10, 9, 380, 20, 18, 380, 10, 9, 408, 0,
		11, 10, 380, 22, 20, 380, 11, 10, 409, 0,
		12, 11, 380, 24, 22, 380, 12, 11, 410, 0,
		13, 12, 380, 26, 24, 380, 13, 12, 411, 0,
		14, 13, 380, 28, 26, 380, 14, 13, 412, 0,
		15, 14, 380, 30, 28, 380, 15, 14, 413, 0,
		16, 15, 380, 32, 30, 380, 16, 15, 414, 0,
		17, 16, 380, 34, 32, 380, 17, 16, 415, 0,
		18, 17, 380, 36, 34, 380, 18, 17, 416, 0,
		19, 18, 380, 38, 36, 380, 19, 18, 417, 0,
		0, 19, 380, 0, 38, 380, 20, 19, 418, 0,
		360, 361, 381, 361, 360, 381, 378, 379, 419, 0,
		361, 362, 381, 360, 362, 381, 379, 380, 420, 0,
		362, 363, 381, 362, 363, 381, 380, 381, 421, 0,
		363, 364, 381, 363, 364, 381, 381, 382, 422, 0,
		364, 365, 381, 364, 365, 381, 382, 383, 423, 0,
		365, 366, 381, 365, 366, 381, 383, 384, 424, 0,
		366, 367, 381, 366, 367, 381, 384, 385, 425, 0,
		367, 368, 381, 367, 368, 381, 385, 386, 426, 0,
		368, 369, 381, 368, 369, 381, 386, 387, 427, 0,
		369, 370, 381, 369, 370, 381, 387, 388, 428, 0,
		370, 371, 381, 370, 371, 381, 388, 389, 429, 0,
		371, 372, 381, 371, 372, 381, 389, 390, 430, 0,
		372, 373, 381, 372, 373, 381, 390, 391, 431, 0,
		373, 374, 381, 373, 374, 381, 391, 392, 432, 0,
		374, 375, 381, 374, 375, 381, 392, 393, 433, 0,
		375, 376, 381, 375, 376, 381, 393, 394, 434, 0,
		376, 377, 381, 376, 377, 381, 394, 395, 435, 0,
		377, 378, 381, 377, 378, 381, 395, 396, 436, 0,
		378, 379, 381, 378, 379, 381, 396, 397, 437, 0,
		379, 360, 381, 379, 361, 381, 397, 398, 438, 0
	};



	//MIntArray triData(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup
	int meshLightShapetriData[] = {
		0, 1, 2, 0, 1, 3, 0, 1, 2, 0,
		2, 1, 3, 3, 1, 2, 2, 1, 3, 0
	};

	//MIntArray triData(vtxId0, vtxId1, vtxId2,  normalId0, normalId1, normalId2, uvId0, uvId1, uvId2, perFaceShadingGroup
	int BgShapetriData[] = {
		0, 63, 64, 0, 1, 3, 0, 1, 3, 0,
		64, 63, 73, 3, 1, 2, 3, 1, 2, 0,
		63, 1, 73, 1, 4, 2, 1, 5, 2, 0,
		73, 1, 65, 2, 4, 5, 2, 5, 6, 0,
		73, 65, 66, 2, 5, 7, 2, 6, 11, 0,
		66, 65, 5, 7, 5, 6, 11, 6, 10, 0,
		64, 73, 4, 3, 2, 8, 3, 2, 15, 0,
		4, 73, 66, 8, 2, 7, 15, 2, 11, 0,
		1, 67, 65, 4, 9, 5, 5, 12, 6, 0,
		65, 67, 74, 5, 9, 10, 6, 12, 18, 0,
		67, 2, 74, 9, 11, 10, 12, 13, 18, 0,
		74, 2, 68, 10, 11, 12, 18, 13, 22, 0,
		74, 68, 69, 10, 12, 14, 18, 22, 27, 0,
		69, 68, 6, 14, 12, 13, 27, 22, 26, 0,
		65, 74, 5, 5, 10, 6, 6, 18, 10, 0,
		5, 74, 69, 6, 10, 14, 10, 18, 27, 0,
		2, 70, 68, 11, 15, 12, 13, 4, 22, 0,
		68, 70, 75, 12, 15, 16, 22, 4, 34, 0,
		70, 3, 75, 15, 17, 16, 4, 7, 34, 0,
		75, 3, 71, 16, 17, 18, 34, 7, 38, 0,
		75, 71, 72, 16, 18, 20, 34, 38, 43, 0,
		72, 71, 7, 20, 18, 19, 43, 38, 42, 0,
		68, 75, 6, 12, 16, 13, 22, 34, 26, 0,
		6, 75, 72, 13, 16, 20, 26, 34, 43, 0,
		4, 66, 28, 8, 7, 22, 15, 11, 51, 0,
		28, 66, 76, 22, 7, 21, 51, 11, 50, 0,
		66, 5, 76, 7, 6, 21, 11, 10, 50, 0,
		76, 5, 29, 21, 6, 23, 50, 10, 54, 0,
		76, 29, 30, 21, 23, 25, 50, 54, 59, 0,
		30, 29, 9, 25, 23, 24, 59, 54, 58, 0,
		28, 76, 8, 22, 21, 26, 51, 50, 63, 0,
		8, 76, 30, 26, 21, 25, 63, 50, 59, 0,
		5, 69, 29, 6, 14, 23, 10, 27, 54, 0,
		29, 69, 77, 23, 14, 27, 54, 27, 66, 0,
		69, 6, 77, 14, 13, 27, 27, 26, 66, 0,
		77, 6, 31, 27, 13, 28, 66, 26, 70, 0,
		77, 31, 32, 27, 28, 30, 66, 70, 75, 0,
		32, 31, 10, 30, 28, 29, 75, 70, 74, 0,
		29, 77, 9, 23, 27, 24, 54, 66, 58, 0,
		9, 77, 32, 24, 27, 30, 58, 66, 75, 0,
		6, 72, 31, 13, 20, 28, 26, 43, 70, 0,
		31, 72, 78, 28, 20, 31, 70, 43, 82, 0,
		72, 7, 78, 20, 19, 31, 43, 42, 82, 0,
		78, 7, 33, 31, 19, 32, 82, 42, 86, 0,
		78, 33, 34, 31, 32, 34, 82, 86, 37, 0,
		34, 33, 11, 34, 32, 33, 37, 86, 90, 0,
		31, 78, 10, 28, 31, 29, 70, 82, 74, 0,
		10, 78, 34, 29, 31, 34, 74, 82, 37, 0,
		8, 30, 35, 26, 25, 36, 63, 59, 19, 0,
		35, 30, 79, 36, 25, 35, 19, 59, 17, 0,
		30, 9, 79, 25, 24, 35, 59, 58, 17, 0,
		79, 9, 36, 35, 24, 37, 17, 58, 20, 0,
		79, 36, 37, 35, 37, 39, 17, 20, 23, 0,
		37, 36, 13, 39, 37, 38, 23, 20, 21, 0,
		35, 79, 12, 36, 35, 40, 19, 17, 33, 0,
		12, 79, 37, 40, 35, 39, 33, 17, 23, 0,
		9, 32, 36, 24, 30, 37, 58, 75, 20, 0,
		36, 32, 80, 37, 30, 41, 20, 75, 89, 0,
		32, 10, 80, 30, 29, 41, 75, 74, 89, 0,
		80, 10, 38, 41, 29, 42, 89, 74, 88, 0,
		80, 38, 39, 41, 42, 44, 89, 88, 85, 0,
		39, 38, 14, 44, 42, 43, 85, 88, 87, 0,
		36, 80, 13, 37, 41, 38, 20, 89, 21, 0,
		13, 80, 39, 38, 41, 44, 21, 89, 85, 0,
		10, 34, 38, 29, 34, 42, 74, 37, 88, 0,
		38, 34, 81, 42, 34, 45, 88, 37, 84, 0,
		34, 11, 81, 34, 33, 45, 37, 90, 84, 0,
		81, 11, 40, 45, 33, 46, 84, 90, 83, 0,
		81, 40, 41, 45, 46, 48, 84, 83, 80, 0,
		41, 40, 15, 48, 46, 47, 80, 83, 81, 0,
		38, 81, 14, 42, 45, 43, 88, 84, 87, 0,
		14, 81, 41, 43, 45, 48, 87, 84, 80, 0,
		16, 42, 43, 49, 50, 52, 79, 78, 76, 0,
		43, 42, 82, 52, 50, 51, 76, 78, 77, 0,
		42, 17, 82, 50, 53, 51, 78, 73, 77, 0,
		82, 17, 44, 51, 53, 54, 77, 73, 72, 0,
		82, 44, 45, 51, 54, 56, 77, 72, 69, 0,
		45, 44, 21, 56, 54, 55, 69, 72, 71, 0,
		43, 82, 20, 52, 51, 57, 76, 77, 68, 0,
		20, 82, 45, 57, 51, 56, 68, 77, 69, 0,
		17, 46, 44, 53, 58, 54, 73, 67, 72, 0,
		44, 46, 83, 54, 58, 59, 72, 67, 65, 0,
		46, 18, 83, 58, 60, 59, 67, 64, 65, 0,
		83, 18, 47, 59, 60, 61, 65, 64, 62, 0,
		83, 47, 48, 59, 61, 63, 65, 62, 60, 0,
		48, 47, 22, 63, 61, 62, 60, 62, 61, 0,
		44, 83, 21, 54, 59, 55, 72, 65, 71, 0,
		21, 83, 48, 55, 59, 63, 71, 65, 60, 0,
		18, 49, 47, 60, 64, 61, 64, 57, 62, 0,
		47, 49, 84, 61, 64, 65, 62, 57, 56, 0,
		49, 19, 84, 64, 66, 65, 57, 55, 56, 0,
		84, 19, 50, 65, 66, 67, 56, 55, 53, 0,
		84, 50, 51, 65, 67, 69, 56, 53, 49, 0,
		51, 50, 23, 69, 67, 68, 49, 53, 52, 0,
		47, 84, 22, 61, 65, 62, 62, 56, 61, 0,
		22, 84, 51, 62, 65, 69, 61, 56, 49, 0,
		20, 45, 52, 57, 56, 71, 68, 69, 47, 0,
		52, 45, 85, 71, 56, 70, 47, 69, 48, 0,
		45, 21, 85, 56, 55, 70, 69, 71, 48, 0,
		85, 21, 53, 70, 55, 72, 48, 71, 46, 0,
		85, 53, 54, 70, 72, 74, 48, 46, 44, 0,
		54, 53, 25, 74, 72, 73, 44, 46, 45, 0,
		52, 85, 24, 71, 70, 75, 47, 48, 41, 0,
		24, 85, 54, 75, 70, 74, 41, 48, 44, 0,
		21, 48, 53, 55, 63, 72, 71, 60, 46, 0,
		53, 48, 86, 72, 63, 76, 46, 60, 40, 0,
		48, 22, 86, 63, 62, 76, 60, 61, 40, 0,
		86, 22, 55, 76, 62, 77, 40, 61, 39, 0,
		86, 55, 56, 76, 77, 79, 40, 39, 35, 0,
		56, 55, 26, 79, 77, 78, 35, 39, 36, 0,
		53, 86, 25, 72, 76, 73, 46, 40, 45, 0,
		25, 86, 56, 73, 76, 79, 45, 40, 35, 0,
		22, 51, 55, 62, 69, 77, 61, 49, 39, 0,
		55, 51, 87, 77, 69, 80, 39, 49, 32, 0,
		51, 23, 87, 69, 68, 80, 49, 52, 32, 0,
		87, 23, 57, 80, 68, 81, 32, 52, 31, 0,
		87, 57, 58, 80, 81, 83, 32, 31, 29, 0,
		58, 57, 27, 83, 81, 82, 29, 31, 30, 0,
		55, 87, 26, 77, 80, 78, 39, 32, 36, 0,
		26, 87, 58, 78, 80, 83, 36, 32, 29, 0,
		24, 54, 59, 75, 74, 85, 41, 44, 25, 0,
		59, 54, 88, 85, 74, 84, 25, 44, 28, 0,
		54, 25, 88, 74, 73, 84, 44, 45, 28, 0,
		88, 25, 60, 84, 73, 86, 28, 45, 24, 0,
		88, 60, 63, 84, 86, 1, 28, 24, 1, 0,
		63, 60, 1, 1, 86, 4, 1, 24, 5, 0,
		59, 88, 0, 85, 84, 0, 25, 28, 0, 0,
		0, 88, 63, 0, 84, 1, 0, 28, 1, 0,
		25, 56, 60, 73, 79, 86, 45, 35, 24, 0,
		60, 56, 89, 86, 79, 87, 24, 35, 16, 0,
		56, 26, 89, 79, 78, 87, 35, 36, 16, 0,
		89, 26, 61, 87, 78, 88, 16, 36, 14, 0,
		89, 61, 67, 87, 88, 9, 16, 14, 12, 0,
		67, 61, 2, 9, 88, 11, 12, 14, 13, 0,
		60, 89, 1, 86, 87, 4, 24, 16, 5, 0,
		1, 89, 67, 4, 87, 9, 5, 16, 12, 0,
		26, 58, 61, 78, 83, 88, 36, 29, 14, 0,
		61, 58, 90, 88, 83, 89, 14, 29, 9, 0,
		58, 27, 90, 83, 82, 89, 29, 30, 9, 0,
		90, 27, 62, 89, 82, 90, 9, 30, 8, 0,
		90, 62, 70, 89, 90, 15, 9, 8, 4, 0,
		70, 62, 3, 15, 90, 17, 4, 8, 7, 0,
		61, 90, 2, 88, 89, 11, 14, 9, 13, 0,
		2, 90, 70, 11, 89, 15, 13, 9, 4, 0
	};

}
#endif
