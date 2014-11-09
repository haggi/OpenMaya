"""Ambient render passes node plugin"""
import sys
import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx

nodeId = OpenMaya.MTypeId(0x0011EF6E)

def addNumAttr(attr, type, default, doc=""):
    numAttr = OpenMaya.MFnNumericAttribute()
    setattr(CoronaPassesNode, attr, numAttr.create(attr, attr, type, default))
    CoronaPassesNode.addAttribute(getattr(CoronaPassesNode, attr))

def addEnumAttr(attr, default, options, doc=""):
    enumAttr = OpenMaya.MFnEnumAttribute()
    setattr(CoronaPassesNode, attr, enumAttr.create(attr, attr, default))
    for option in options:
        enumAttr.addField(option, options[option])
    CoronaPassesNode.addAttribute(getattr(CoronaPassesNode, attr))

def addTypedAttr(attr, type, default, doc=""):
    tAttr = OpenMaya.MFnTypedAttribute()
    setattr(CoronaPassesNode, attr, tAttr.create(attr, attr, type))
    CoronaPassesNode.addAttribute(getattr(CoronaPassesNode, attr))

def addStrAttr(attr):
    tAttr = OpenMaya.MFnTypedAttribute()
    setattr(CoronaPassesNode, attr, tAttr.create(attr, attr, OpenMaya.MFnData.kString))
    CoronaPassesNode.addAttribute(getattr(CoronaPassesNode, attr))

class CoronaPassesNode(OpenMayaMPx.MPxNode):
    pass

def nodeCreator():
    return OpenMayaMPx.asMPxPtr(CoronaPassesNode())

def nodeInitializer():
    bit_options = {"16": 0, "32": 1}
    kBoolean = OpenMaya.MFnNumericData.kBoolean
    kDouble = OpenMaya.MFnNumericData.kDouble
    kLong = OpenMaya.MFnNumericData.kLong
    kInt = OpenMaya.MFnNumericData.kInt
    kFloat = OpenMaya.MFnNumericData.kFloat
    kString = OpenMaya.MFnData.kString

    addNumAttr("passType", kInt, 0)
#     addNumAttr("mapChannel", kInt, 0)
#     addStrAttr("sourceColor")
#     addStrAttr("componentName")
#     typedAttr = OpenMaya.MFnTypedAttribute()
#     CoronaPassesNode.componentList = typedAttr.create("componentList", "componentList", kString)
#     typedAttr.setArray(True)
#     CoronaPassesNode.addAttribute(CoronaPassesNode.componentList)
#     addStrAttr("elementType")
#     addNumAttr("depthMin", kFloat, 0.01)
#     addNumAttr("depthMax", kFloat, 100000.0)

#     addNumAttr("batchOnly", kBoolean, 1, "Passes in batch only")
#     addNumAttr("contrastAll", kBoolean, 0, "Constrast All Buffers")
#     addNumAttr("cleanUp", kBoolean, 1, "Cleanup after render")
#     addEnumAttr("bitDepthTechPasses", 0, bit_options, "Bit depth of tech passes")
    

def initializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.registerNode("coronaPassesNode", nodeId, nodeCreator, nodeInitializer)
    except:
        sys.stderr.write("Failed to register node: coronaPassesNode")
        raise


def uninitializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.deregisterNode(nodeId)
    except:
        sys.stderr.write("Failed to deregister node: coronaPassesNode")
        raise
