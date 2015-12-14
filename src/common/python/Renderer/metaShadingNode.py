import math, sys

import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx

NODENAMES = ["miniNodeA", "miniNodeB"]

sineNodeId = OpenMaya.MTypeId(0x87000)

class MetaClass(OpenMayaMPx.MPxNode):
    @classmethod
    def nodeCreator(cls):
        return OpenMayaMPx.asMPxPtr( cls() )
    @classmethod
    def nodeInitializer(cls):
        print "nodeInitializer", cls
        nAttr = OpenMaya.MFnNumericAttribute()
        cls.input = nAttr.create( "input", "in", OpenMaya.MFnNumericData.kFloat, 0.0 )
        nAttr.setStorable(1)
        nAttr = OpenMaya.MFnNumericAttribute()
        cls.output = nAttr.create( "output", "out", OpenMaya.MFnNumericData.kFloat, 0.0 )
        nAttr.setStorable(1)
        nAttr.setWritable(1)
        # add attributes
        cls.addAttribute( cls.input )
        cls.addAttribute( cls.output )
        cls.attributeAffects( cls.input, cls.output )
    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)
        
    
# # Node definition
# class sineNode(OpenMayaMPx.MPxNode):
#     # class variables
#     input = OpenMaya.MObject()
#     output = OpenMaya.MObject()
#     def __init__(self):
#         OpenMayaMPx.MPxNode.__init__(self)
#     def compute(self,plug,dataBlock):
#         if ( plug == sineNode.output ):
#             dataHandle = dataBlock.inputValue( sineNode.input )
#             
#             inputFloat = dataHandle.asFloat()
#             result = math.sin( inputFloat ) * 10.0
#             outputHandle = dataBlock.outputValue( sineNode.output )
#             outputHandle.setFloat( result )
#             dataBlock.setClean( plug )
# 
#         return OpenMaya.kUnknownParameter
# 
# # creator
# def nodeCreator():
#     return OpenMayaMPx.asMPxPtr( sineNode() )
# 
# # initializer
# def nodeInitializer():
#     # input
#     nAttr = OpenMaya.MFnNumericAttribute()
#     sineNode.input = nAttr.create( "input", "in", OpenMaya.MFnNumericData.kFloat, 0.0 )
#     nAttr.setStorable(1)
#     # output
#     nAttr = OpenMaya.MFnNumericAttribute()
#     sineNode.output = nAttr.create( "output", "out", OpenMaya.MFnNumericData.kFloat, 0.0 )
#     nAttr.setStorable(1)
#     nAttr.setWritable(1)
#     # add attributes
#     sineNode.addAttribute( sineNode.input )
#     sineNode.addAttribute( sineNode.output )
#     sineNode.attributeAffects( sineNode.input, sineNode.output )
    
# initialize the script plug-in
def initializePlugin(mobject):
    
    for index, nodeName in enumerate(NODENAMES):    
        X = type(nodeName, (MetaClass,), {})
        
        mplugin = OpenMayaMPx.MFnPlugin(mobject)
        try:
            mplugin.registerNode( X.__name__, OpenMaya.MTypeId(0x87000 + index), X.nodeCreator, X.nodeInitializer )
        except:
            sys.stderr.write( "Failed to register node: %s" % X.__name__ )
            raise

# uninitialize the script plug-in
def uninitializePlugin(mobject):
    X = type(NODENAMES[0], (MetaClass,), {})
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.deregisterNode( sineNodeId )
    except:
        sys.stderr.write( "Failed to deregister node: %s" % X.__name__ )
        raise
    
