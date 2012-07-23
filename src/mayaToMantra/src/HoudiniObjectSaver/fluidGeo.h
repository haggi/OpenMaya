#include "hsff.h"
#include "pystring.h"
#include <fstream>
#include <stdio.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>

typedef unsigned int uint;


void createFluidGeo(Hsff& inFile)
{
	 GU_Detail	gdp;
	 GU_PrimVolume *volume;

	//read number of attributes
	int numAttr = inFile.readInt();
	uint gridSize = inFile.readUInt();
	uint xres = inFile.readUInt();
	uint yres = inFile.readUInt();
	uint zres = inFile.readUInt();
	double xdim = inFile.readDouble();
	double ydim = inFile.readDouble();
	double zdim = inFile.readDouble();

	cout << "Fluid file contains " << numAttr << " attributes and has a grid size of " << gridSize << "\n";

	volume = (GU_PrimVolume *)GU_PrimVolume::build(&gdp);
	
    UT_VoxelArrayWriteHandleF handle = volume->getVoxelWriteHandle();

	// now lets read the attributes
	std::string attr;
	inFile.readString(attr);
	cout << "Reading " << attr << " Attribute\n";
	float *density = (float *)malloc(sizeof(float) * gridSize);
	inFile.readFloat(density, gridSize);
	float *d = density;
	uint i, j, k;
	cout << "Reading done filling voxelGrid\n";
	UT_Matrix3              xform;
    xform.identity();
    xform.scale(xdim/2.0, ydim/2.0, zdim/2.0);
	volume->setTransform(xform);

    handle->size(xres, yres, zres);
	uint count = 0;
    for (i = 0; i < xres; i++)
        for (j = 0; j < yres; j++)
            for (k = 0; k < zres; k++)
			{
				uint index = (k * xres * yres) + (j * yres) + i;
				if( index > gridSize)
					cout << "Index " << index << " > " << gridSize << " problem!!!\n";
				//cout << "x: " << i << " y: " << j << " z: " << k << " index " << index << " val " << density[index] << "\n";
                handle->setValue(i, j, k, density[index]);
			}
	//// a paricle hsff file contains at least the attributes:
	//// position pos
	//// velocity vel  --> v
	//// size		radiusPP --> pscale
	//// others are optional
	//// if rgbPP --> Cd ?

	//int defaultId = -1;
	//GB_AttributeRef idAttr = gdp.addPointAttrib("id", sizeof(int), GB_ATTRIB_INT, &defaultId);
	//
	//UT_Vector3 vel(1,2,3);
	//GB_AttributeRef velAttr = gdp.addPointAttrib("v", sizeof(UT_Vector3), GB_ATTRIB_VECTOR, &vel);

	//float pscale = 1.234;
	//GB_AttributeRef scaleAttr = gdp.addPointAttrib("pscale", sizeof(float), GB_ATTRIB_FLOAT, &pscale);

	////Particles as saved as MVectorArray, 3 doubles for every position	
	//particle = (GU_PrimParticle *)GU_PrimParticle::build(&gdp, numParticles);
	//int np = particle->getNumParticles();
	////cout << "Particle object now has " << np << " particles\n";

	//double *posList = (double *)malloc(sizeof(double) * 3 * numParticles);
	//inFile.readDouble(posList, 3 * numParticles);
	//double *velList = (double *)malloc(sizeof(double) * 3 * numParticles);
	//inFile.readDouble(velList, 3 * numParticles);
	//double *sizeList = (double *)malloc(sizeof(double) * numParticles);
	//inFile.readDouble(sizeList, numParticles);

	////gdp.addVelocityAttribute();
	//int sizeId = 0;
	//for( uint i = 0; i < numParticles; i++)
	//{
	//	uint index = i * 3;
	//	particle->getVertex(i).getPt()->setPos(posList[index], posList[index+1], posList[index+2]);
	//	particle->getVertex(i).getPt()->setValue<UT_Vector3>(velAttr, UT_Vector3(velList[index],velList[index+2],velList[index+2]));
	//	particle->getVertex(i).getPt()->setValue<float>(scaleAttr, sizeList[sizeId++]);
	//}
	//
	//free(posList);
	//free(velList);
	//free(sizeList);

	if( inFile.doBinary )
	{
		std::string outFile = pystring::replace(inFile.fileName, ".hsff", ".bgeo");
		cout << "Writing to file " << outFile << "\n";
		gdp.save(outFile.c_str(), NULL, NULL);
	}else{
		std::string outFile = pystring::replace(inFile.fileName, ".hsff", ".geo");
		cout << "Writing to file " << outFile << "\n";
		gdp.save(outFile.c_str(), NULL, NULL);
	}
}
