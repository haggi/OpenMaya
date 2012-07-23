#include "hsff.h"
#include "pystring.h"
#include <fstream>
#include <stdio.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPart.h>
#include <GEO/GEO_AttributeHandle.h>

typedef unsigned int uint;

class AttrStruct
{
public:
	GEO_AttributeHandle handle;
	GA_RWAttributeRef ref;

	int type;
	std::string attName;
	std::string realName;
	void *data;
	uint numParticles;
	AttrStruct(GU_Detail& gdp, uint nParticles, Hsff& inFile)
	{
		data = NULL;
		numParticles = nParticles;
		cout << "AttrStruct reading att\n"; 
		inFile.readParticleAttr(attName, type);
		cout << "AttName " << attName << "\n";
		allocMem(type);
		cout << "Reading data \n";
		inFile.readData(data, numParticles, type);
		// position do not need attribute
		if( attName != "position")
		{
			cout << "Create GDP Attribute: " << attName.c_str() << " type " << type << "\n";
			createGDPAttr(gdp);
		}
	};
	void createGDPAttr(GU_Detail& gdp)
	{
		realName = attName;
		if( attName == "rgbPP")
			realName = "Cd";
		if( attName == "velocity")
			realName = "v";
		if( attName == "radiusPP")
			realName = "pscale";
		if( (attName != "rgbPP") && (attName != "velocity") && (attName != "radiusPP") )
		{
			realName = std::string("p_");
			realName += attName;
		}

		if( type == INT)
		{
			ref = gdp.addIntTuple(GA_ATTRIB_POINT, realName.c_str(), sizeof(int));
			handle = gdp.getPointAttribute(realName.c_str());
			return;
		}
		if( type == FLOAT)
		{
			ref = gdp.addFloatTuple(GA_ATTRIB_POINT, realName.c_str(), sizeof(float));
			handle = gdp.getPointAttribute(realName.c_str());
			return;
		}
		if( type == DOUBLE)
		{
			ref = gdp.addFloatTuple(GA_ATTRIB_POINT, realName.c_str(), sizeof(float));
			handle = gdp.getPointAttribute(realName.c_str());
			return;
		}
		if( type == VECTOR)
		{
			ref = gdp.addFloatTuple(GA_ATTRIB_POINT, realName.c_str(), sizeof(UT_Vector3));
			if (ref.isValid()) ref.getAttribute()->setTypeInfo(GA_TYPE_VECTOR);
			handle = gdp.getPointAttribute(realName.c_str());
			return;
		}
		if( type == COLOR)
		{
			ref = gdp.addFloatTuple(GA_ATTRIB_POINT, realName.c_str(), sizeof(UT_Vector3));
			if (ref.isValid()) ref.getAttribute()->setTypeInfo(GA_TYPE_COLOR);
			handle = gdp.getPointAttribute(realName.c_str());
			return;
		}
	};
	void allocMem(int type)
	{
		if( type == INT)
		{
			data = malloc(sizeof(int) * numParticles);
			return;
		}
		if( type == FLOAT)
		{
			data = malloc(sizeof(float) * numParticles);
			return;
		}
		if( type == DOUBLE)
		{
			data = malloc(sizeof(double) * numParticles);
			return;
		}
		if( type == VECTOR)
		{
			data = malloc(sizeof(double) * 3 * numParticles);
			return;
		}
		if( type == COLOR)
		{
			data = malloc(sizeof(double) * 3 * numParticles);
			return;
		}
	};
	~AttrStruct()
	{
		if( data != NULL)
			free(data);
	};
};

void createParticleGeo(Hsff& inFile)
{
	 GU_Detail	gdp;
	 GU_PrimParticle *particle;

	//read number of particles uint
	uint numParticles = inFile.readUInt();

	cout << "Particle file contains " << numParticles << " particles\n";

	// a paricle hsff file contains at least the attributes:
	// position pos
	// velocity vel  --> v
	// size		radiusPP --> pscale
	// others are optional
	// if rgbPP --> Cd ?

	std::vector<AttrStruct *> attStructList;

	while( !inFile.inFile->eof() )
	{
		AttrStruct *att = new AttrStruct(gdp, numParticles, inFile);
		attStructList.push_back(att);
	}

	particle = (GU_PrimParticle *)GU_PrimParticle::build(&gdp, 0);
	
	for( uint i = 0; i < numParticles; i++)
	{
		GEO_Point *newPoint = gdp.appendPoint();
		for( uint attId = 0; attId < attStructList.size(); attId++)
		{
			AttrStruct *att = attStructList[attId];
			double *ddata = (double *)att->data;
			int *idata = (int *)att->data;
			if( att->attName == "position")
			{
				uint index = i * 3;
				newPoint->setPos(ddata[index],ddata[index+1],ddata[index+2]);
			}else{
				uint index = i; 
				att->handle.setElement(newPoint);
				if( att->type == VECTOR ||  att->type == COLOR)
				{
					index = i * 3; 
					att->handle.setV3(UT_Vector3(ddata[index],ddata[index+1],ddata[index+2]));
				}
				if( att->type == FLOAT ||  att->type == DOUBLE)
				{
					att->handle.setF(ddata[index]);
				}
				if( att->type == INT)
				{
					att->handle.setI(idata[index]);
				}
			}
		}
		particle->appendParticle(newPoint);
	}

        //partsys->getRenderAttribs().setType(GEO_PARTICLE_TUBES);
        //partsys->getRenderAttribs().setMotionBlur(1)
        //partsys->getRenderAttribs().setBlurTime(0.03);
        //partsys->getRenderAttribs().setSize(0.05);

	for( uint attId = 0; attId < attStructList.size(); attId++)
	{
		delete attStructList[attId];
	}
	attStructList.clear();

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
