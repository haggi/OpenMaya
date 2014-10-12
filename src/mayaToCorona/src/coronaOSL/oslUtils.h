#ifndef OSL_UTILS_H
#define OSL_UTILS_H

/*
	OSL and maya projection nodes have a problem. 
	A projection node normally works this way:
		- Calculate the projection of a point
		- Calculate the new uv's
		- Get the image based on these new uv's
	In OSL this is not possible. Modifying the uv's and evaluate
	an image based on these new uv's is not possible, the uv's have to
	be manipulated before they are read by the projection node what is a circle, 
	because the projection node itself calculates the uvs....
	My solution is to split the projection node into the color part and the projection part.
	The projection part has to be connected to the very first node in the chain, normally a place2dnode.
	So what is do is:
		- from an attribute/plug I search the node history for a projection node
		- if one is found, follow the chain to the leaf nodes and place the projection part before the leaf
	So I create a structure which saves the projection nodes for every leaf node
*/
#include <maya/MObject.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <vector>

namespace OSL{

	struct ProjectionUtil{
		MObjectArray leafNodes;
		MObjectArray projectionNodes;
	};
	void listProjectionHistory(MObject& mobject, ProjectionUtil& util);

}
#endif