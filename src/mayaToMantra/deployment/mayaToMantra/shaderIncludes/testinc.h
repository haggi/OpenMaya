
#define VOP_SHADING

#include <voptype.h>
#include <voplib.h>
#include "utils.h"

vector blubb( export int __blabla)
{
	vector clr = vop_lighting("specular", N, I, {0.0, 0.0, 0.0}, { 0.5, 0.5, 0.5 }, Cf, { 0.5, 0.5, 0.5 }, 0.050000000000000003, 0.050000000000000003, 0);	
	__blabla = 0;
	return clr;
}
