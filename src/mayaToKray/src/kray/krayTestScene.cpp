#include "krayTestScene.h"

void buildBoxMesh(Kray::Prototyper& prot,Kray::MeshSymbol& msh){
	Kray::VertexMapSymbol vmap(prot,3);		// create vertex map

	vmap.data(0,Kray::Vector(-1,-1,-1));		// fill vertex map with cube vertex positions
	vmap.data(1,Kray::Vector(-1,-1, 1));
	vmap.data(2,Kray::Vector(-1, 1,-1));
	vmap.data(3,Kray::Vector(-1, 1, 1));
	vmap.data(4,Kray::Vector( 1,-1,-1));
	vmap.data(5,Kray::Vector( 1,-1, 1));
	vmap.data(6,Kray::Vector( 1, 1,-1));
	vmap.data(7,Kray::Vector( 1, 1, 1));

	msh.points(vmap);						// connect positions vertex map with a mesh

	Kray::VarLenDoubleDynamic polygon;		// create class for storing polygon data
	polygon.set(0,2,6,4);		msh.addPoly(0,polygon);	// add polygons to mesh
	polygon.set(5,7,3,1);		msh.addPoly(0,polygon);	// all polygons have tag==0
	polygon.set(1,3,2,0);		msh.addPoly(0,polygon); // tag is an index to materials table
	polygon.set(4,6,7,5);		msh.addPoly(1,polygon); // we will connect yellowMat with tag==0
	polygon.set(0,4,5,1);		msh.addPoly(1,polygon); // and redMat with tag==1 later
	polygon.set(3,7,6,2);		msh.addPoly(1,polygon);
}
void setupSimpleMaterialsForMesh(Kray::Prototyper& prot,Kray::MeshSymbol& msh){
	Kray::Symbol yellowMat(prot);									// create material object, create new KraySymbol
	prot.material(yellowMat);									// register it, as material
	prot.materialSet_color(yellowMat,Kray::Vector(0.9,0.9,0.3));	// modify default color

	Kray::Symbol redMat(prot);									// another material
	prot.material(redMat);
	prot.materialSet_color(redMat,Kray::Vector(0.9,0.2,0));

	prot.meshTag_material(msh,0,yellowMat);	// attach yellowMat to mesh tag==0
	prot.meshTag_material(msh,1,redMat);	// attach redMat to mesh tag==1
}

void setupSimpleMeshScene(Kray::Prototyper& prot){

	prot.lightAdd_point(Kray::Vector(-1, 5, -2),Kray::Vector(1,1,1));			// add light source to scene
	
	prot.background_sky(Kray::Vector(0.4,0.8,0.9),Kray::Vector(0,0,0),Kray::Vector(1,1,1),Kray::Vector(0,1,0)); // gradient background
	
	prot.pixelFilter_mitchell();			// Mitchel pixel filter makes antialias looks better then box filter (which is default filter becouse of its simplicity)
	prot.splitScreen_none();				// how render screen is divided between render threads
}