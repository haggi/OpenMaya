// Houdini object saver.
// This tool reads object definitions form a hsff file and writes geo/bgeo files for 
// later usage in mantra rendering.


//#include <stdio.h>
#include <iostream>
#include <CMD/CMD_Args.h>
#include <GU/GU_Detail.h>

#include "HoudiniObjectSaver.h"
#include "hsff.h"
#include "particleGeo.h"
#include "fluidGeo.h"
#include "meshGeo.h"
#include "nurbsGeo.h"
#include "readBBox.h"
#include "readPtc.h"
#include "dirhandler.h"
#include "logging.h"

Logging logger;

static void usage(const char *program)
{
	std::cerr << "Usage: " << program << " sourcedir [binary = 0| ascii = 1]\n";
	std::cerr << "The tool will scan the given directory for .hsff files and writes geo/bgeo files depending on binary second argument." << std::endl;
    std::cerr << "This conversion is done only for files where the .hsff files are younger than the .geo files." << std::endl;
}

// HoudiniObjectTools main
// Args:
//		-cd  ==  HoudiniObjectTools.exe -cd c:/data/hsffFileDir --> convert all hsff files in this dir to geo/bgeo
//		-c  == HoudiniObjectTools.exe -c c:/data/sphere.hsff --> convert exact one file1.hsff to file1.geo/bgeo
//		-b  == HoudiniObjectTools.exe -c c:/data/sphere.hsff -b -> binary  file1.hsff to file1.bgeo
//		-bbox == HoudiniObjectTools.exe -bbox c:/data/sphere.bgeo getbounding box from file1.geo/bgeo
//		-pts == HoudiniObjectTools.exe -pts c:/data/sphere.bgeo extract points from geo file save as readable format

struct args{
	bool cd;
	bool c;
	bool b;
	bool bbox;
	bool ptc;
	bool ptcd;
	bool v;
	int cdp;
	int cp;
	int bp;
	int bboxp;
	int ptcp;
	int ptcdp;
	int vp;
	float ptcdv;
	std::string bboxfile;
	std::string ptcfile;
	std::string cdDir;
	std::string cFile;
	args(){
		cd = c = b = bbox = ptc = v = ptcd = false;
	};
};
void setArgs( struct args& a, int argc, char *argv[])
{
	for( int ac = 0; ac < argc; ac++)
	{
		std::cout << "Checking " << argv[ac] << "\n";
		if( ac == 0)
			continue;
		std::string arg(argv[ac]);
		if( arg == "-v" )
		{
			a.v = true;
			a.vp = ac;
		}
		if( arg == "-b" )
		{
			a.b = true;
			a.bp = ac;
		}
		if( arg == "-bbox" )
		{
			a.bbox = true;
			a.bboxp = ac;
		}
		if( arg == "-c" )
		{
			a.c = true;
			a.cp = ac;
		}
		if( arg == "-cd" )
		{
			a.cd = true;
			a.cdp = ac;
		}
		if( arg == "-ptc" )
		{
			a.ptc = true;
			a.ptcp = ac;
		}
		if( arg == "-ptcd" )
		{
			a.ptcd = true;
			a.ptcdp = ac;
		}
		if( a.v )
		{
			if( ac == (a.vp + 1))
			{
				UT_String intarg(argv[ac]);
				std::cout << "Verbosity argument: " << intarg.toInt() << "\n";
				logger.setLogLevel((Logging::LogLevel)intarg.toInt());
			}
		}
		if( a.bbox )
		{
			if( ac == (a.bboxp + 1))
			{
				a.bboxfile = std::string(argv[ac]);
				std::cout << "bbox file " << a.bboxfile << "\n";
			}
		}	
		if( a.ptc )
		{
			if( ac == (a.ptcp + 1))
			{
				a.ptcfile = std::string(argv[ac]);
				std::cout << "pointcloud file " << a.ptcfile << "\n";
			}
		}	
		if( a.ptcd )
		{
			if( ac == (a.ptcdp + 1))
			{
				UT_String farg(argv[ac]);
				a.ptcdv = farg.toFloat();
				std::cout << "pointcloud file density " << a.ptcdv << "\n";
			}
		}	
		if( a.c )
		{
			if( ac == (a.cp + 1))
			{
				a.cFile = std::string(argv[ac]);
				std::cout << "convert file " << a.cFile << "\n";
			}
		}	
		if( a.cd )
		{
			if( ac == (a.cdp + 1))
			{
				a.cdDir = std::string(argv[ac]);
				std::cout << "convert dir " << a.cdDir << "\n";
			}
		}	
	}
}

int main(int argc, char *argv[])
{
	
	logger.setLogLevel(Logging::Debug);
	struct args a;
	setArgs(a, argc, argv);

    GU_Detail		 gdp;
	bool binary = false;
	bool bbox = false;		

	if( a.b )
		binary = true;

	// convert all files in directory from hsff to geo or bgeo
	if (a.cd || a.c)
	{
		logger.debug("Found argument -cd or -c, trying to convert files in a directory.");
		std::vector<std::string> filesToConvert;
		if( a.cd )
		{
			logger.debug("Found argument -cd trying to convert files in a directory.");
			std::string info("Directory: ");
			info += a.cdDir;
			logger.debug(info);

			if( binary )
				logger.debug("Exporting binary bgeo files");
			else
				logger.debug("Exporting ascii geo files");

			//dirname.harden(argv[1]); // what does this mean, harden???

			fs::path p(a.cdDir);
			getHsffFiles(p, filesToConvert, binary);
		}else{
			logger.debug("Found argument -c trying to convert exactly one file.");
			//filename = UT_String(args.argp('c'));
			std::string info("Filename.");
			info += a.cFile;
			logger.debug(info);
			filesToConvert.push_back(a.cFile);
		}

		// export geo loop
		for( uint fId = 0; fId < filesToConvert.size(); fId++)
		{
			Hsff inFile(filesToConvert[fId]);
			if(!inFile.good)
			{
				logger.error(std::string("Error: problems reading file ") + filesToConvert[fId]);
				//cerr << "Error: problems reading file " << filesToConvert[fId] << "\n";
				inFile.close();
				continue;		
			}
			logger.debug(std::string("Converting file ") + filesToConvert[fId]);
			//cout << "Converting " << filesToConvert[fId] << "\n";
			inFile.doBinary = binary;

			unsigned int geoType = inFile.readGeoType();
			switch(geoType)
			{
			case PARTICLE:
				//cout << "Detected particle file.\n";
				createParticleGeo(inFile);
				break;
			case FLUID:
				//cout << "Detected fluid file.\n";
				createFluidGeo(inFile);
				break;
			case MESH:
				//cout << "Detected mesh file.\n";
				createMeshGeo(inFile);
				break;
			case CURVE:
				//cout << "Detected curve file.\n";
				createCurveGeo(inFile);
				break;
			case NURBS:
				//cout << "Detected nurbs file.\n";
				createNurbsGeo(inFile);
				break;
			}

			inFile.close();
		}
		return 0;
	}


	// get bbox from 1st argument
	if (a.bbox)
	{	
		std::string info("Getting BBox of ");
		info += a.bboxfile;
		logger.debug(info);
		if( !isValid(fs::path(a.bboxfile)))
		{
			logger.error("Could not read file.");
			return 1;
		}
		UT_BoundingBox bbox;
		getBBox(a.bboxfile, bbox);
		cout << "BBox: " << bbox.xmin() << " " << bbox.ymin() << " " << bbox.zmin() << " " << bbox.xmax() << " " << bbox.ymax() << " " << bbox.zmax() << "\n";
	}

	if (a.ptc)
	{
		std::string info("Getting pointcloud of ");
		info += a.ptcfile;
		logger.debug(info);
		std::string outFile = pystring::replace(a.ptcfile, ".geo", ".hsffp"); // add a 'p' to show that it is a point cloud file from this tool
		if( pystring::endswith(a.ptcfile, "bgeo"))
			outFile = pystring::replace(a.ptcfile, ".bgeo", ".hsffp"); // add a 'p' to show that it is a point cloud file from this tool
		UT_BoundingBox bbox;
		readPtc(a.ptcfile, outFile, a.ptcdv, bbox);
		cout << "BBox: " << bbox.xmin() << " " << bbox.ymin() << " " << bbox.zmin() << " " << bbox.xmax() << " " << bbox.ymax() << " " << bbox.zmax() << "\n";
	}

	//cout << "numargs: " << args.argc() << "\n"; 

	
	
 //   if (args.argc() == 3)
	//{
	//	UT_String dobin;
	//	
	//	dobin.harden(argv[2]);
	//	if(dobin.isInteger())
	//	{
	//		if( dobin.toInt() == 0)
	//			binary = true;
	//		else
	//			binary = false;
	//	}
	//}

	//if(binary)
	//	cout << "Writing from source dir " << dirname << " as binary\n";
	//else
	//	cout << "Writing from source dir " << dirname << " as ascii\n";
	


    return 0;

}



//bool
//voxelLoad(UT_IStream &is, GU_Detail *gdp)
//{
//    // Check our magic token
//    if (!is.checkToken("VOXELS"))
//	return false;
//
//    GEO_AttributeHandle		name_gah;
//    int				def = -1;
//
//    gdp->addPrimAttrib("name", sizeof(int), GB_ATTRIB_INDEX, &def);
//    name_gah = gdp->getPrimAttribute("name");
//
//    while (is.checkToken("VOLUME"))
//    {
//	UT_String		name;
//	UT_WorkBuffer		buf;
//
//	is.getWord(buf);
//	name.harden(buf.buffer());
//
//	int			rx, ry, rz;
//
//	is.read(&rx); is.read(&ry); is.read(&rz);
//
//	// Center and size
//	float			tx, ty, tz, sx, sy, sz;
//
//	is.read(&tx); is.read(&ty); is.read(&tz);
//	is.read(&sx); is.read(&sy); is.read(&sz);
//
//	GU_PrimVolume		*vol;
//
//	vol = (GU_PrimVolume *)GU_PrimVolume::build(gdp);
//
//	// Set the name of the primitive
//	name_gah.setElement(vol);
//	name_gah.setString(name);
//
//	// Set the center of the volume
//	vol->getVertex().getPt()->getPos() = UT_Vector3(tx, ty, tz);
//
//	UT_Matrix3		xform;
//
//	// The GEO_PrimVolume treats the voxel array as a -1 to 1 cube
//	// so its size is 2, so we scale by 0.5 here.
//	xform.identity();
//	xform.scale(sx/2, sy/2, sz/2);
//
//	vol->setTransform(xform);
//
//	UT_VoxelArrayWriteHandleF	handle = vol->getVoxelWriteHandle();
//
//	// Resize the array.
//	handle->size(rx, ry, rz);
//
//	if (!is.checkToken("{"))
//	    return false;
//
//	for (int z = 0; z < rz; z++)
//	{
//	    for (int y = 0; y < ry; y++)
//	    {
//		for (int x = 0; x < rx; x++)
//		{
//		    float		v;
//
//		    is.read(&v);
//
//		    handle->setValue(x, y, z, v);
//		}
//	    }
//	}
//
//	if (!is.checkToken("}"))
//	    return false;
//
//	// Proceed to the next volume.
//    }
//
//    // All done successfully
//    return true;
//}
//
//bool
//voxelLoad(const char *fname, GU_Detail *gdp)
//{
//    // The UT_IFStream is a specialization of istream which has a lot
//    // of useful utlity methods and some corrections on the behaviour.
//    UT_IFStream	is(fname, UT_ISTREAM_ASCII);
//
//    return voxelLoad(is, gdp);
//}
//
//bool
//voxelSave(ostream &os, const GU_Detail *gdp)
//{
//    // Write our magic token.
//    os << "VOXELS" << endl;
//
//    // Now, for each volume in our gdp...
//    const GEO_Primitive		*prim;
//    GEO_AttributeHandle			 name_gah;
//    UT_String				 name;
//    UT_WorkBuffer			 buf;
//
//    name_gah = gdp->getPrimAttribute("name");
//    FOR_ALL_PRIMITIVES(gdp, prim)
//    {
//	if (prim->getPrimitiveId() == GEOPRIMVOLUME)
//	{
//	    // Default name
//	    buf.sprintf("volume_%d", prim->getNum());
//	    name.harden(buf.buffer());
//
//	    // Which is overridden by any name attribute.
//	    if (name_gah.isAttributeValid())
//	    {
//		name_gah.setElement(prim);
//		name_gah.getString(name);
//	    }
//
//	    os << "VOLUME " << name << endl;
//	    const GEO_PrimVolume	*vol = (GEO_PrimVolume *) prim;
//
//	    int		resx, resy, resz;
//
//	    // Save resolution
//	    vol->getRes(resx, resy, resz);
//	    os << resx << " " << resy << " " << resz << endl;
//
//	    // Save the center and approximate size.
//	    // Calculating the size is complicated as we could be rotated
//	    // or sheared.  We lose all these because the .voxel format
//	    // only supports aligned arrays.
//	    UT_Vector3		p1, p2;
//
//	    os << vol->getVertex().getPt()->getPos().x() << " "
//	       << vol->getVertex().getPt()->getPos().y() << " "
//	       << vol->getVertex().getPt()->getPos().z() << endl;
//
//	    vol->indexToPos(0, 0, 0, p1);
//	    vol->indexToPos(1, 0, 0, p2);
//	    os << resx * (p1 - p2).length() << " ";
//	    vol->indexToPos(0, 1, 0, p2);
//	    os << resy * (p1 - p2).length() << " ";
//	    vol->indexToPos(0, 0, 1, p2);
//	    os << resz * (p1 - p2).length() << endl;
//
//	    UT_VoxelArrayReadHandleF handle = vol->getVoxelHandle();
//
//	    // Enough of a header, dump the data.
//	    os << "{" << endl;
//	    for (int z = 0; z < resz; z++)
//	    {
//		for (int y = 0; y < resy; y++)
//		{
//		    os << "    ";
//		    for (int x = 0; x < resx; x++)
//		    {
//			os << (*handle)(x, y, z) << " ";
//		    }
//		    os << endl;
//		}
//	    }
//	    os << "}" << endl;
//	    os << endl;
//	}
//    }
//
//    return true;
//}
//
//bool
//voxelSave(const char *fname, const GU_Detail *gdp)
//{
//    ofstream	os(fname);
//
//    // Default output precision of 6 will not reproduce our floats
//    // exactly on load, this define has the value that will ensure
//    // our reads match our writes.
//    os.precision(SYS_FLT_DIG);
//
//    return voxelSave(os, gdp);
//}
//
//
//// Convert a volume into a toy ascii voxel format.
////
//// Build using:
////	hcustom -s geo2voxel.C
////
//// Example usage:
////	geo2voxel input.bgeo output.voxel
////	geo2voxel input.voxel output.bgeo
////
//// You can add support for the .voxel format in Houdini by editing
//// your GEOio table file and adding the line
//// .voxel "geo2voxel %s stdout.bgeo" "geo2voxel stdin.bgeo %s"
////
