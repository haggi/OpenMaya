#ifndef MTAP_STANDIN_H
#define MTAP_STANDIN_H

#include "mayarendernodes/standinLocatorNode.h"

#define STANDIN_LOCATOR_NODE_NAME "mtap_standInLocator"

class mtap_StandinLocator : public StandinLocatorNode
{
public:
	mtap_StandinLocator();
	virtual ~mtap_StandinLocator();

    virtual MStatus   compute( const MPlug&, MDataBlock& );
	virtual void      postConstructor();
    static void *     creator();
    static MStatus    initialize();

    static MTypeId    id;


};

#endif
