import pymel.core as pm
import maya.OpenMaya as om

def unload_plugin():
    pm.newFile(force=True)
    if pm.pluginInfo("mayatoappleseed.mll", query=True, loaded=True):
        pm.unloadPlugin("mayatoappleseed.mll")    
    pm.newFile(force=True)
    
def load_plugin():
    if not pm.pluginInfo("mayatoappleseed.mll", query=True, loaded=True):
        pm.loadPlugin("mayatoappleseed.mll")
    
def reloadPlugin():
    unload_plugin()
    load_plugin()
    

class assembly:
    def __init__(self, dp, dn, name, parentAss = None):
        self.geoList = []
        self.dagPath = dp
        self.dagNode = dn
        self.name = name
        self.parentAss = parentAss
        self.assemblies = []
        self.assembly_instances = []
        self.transformMatrix = om.MTransformationMatrix().identity.asMatrix()
        self.geometry = []
    
class mobject:
    def __init__(self, dagPath):
        self.dagPath = dagPath
        self.dagNode = om.MFnDagNode(self.dagPath.node())
        self.node = pm.PyNode(dagPath.fullPathName())
        self.assembly = None

def getWorld():
    di = om.MItDag(om.MItDag.kDepthFirst, om.MFn.kInvalid)
    while not di.isDone():
        dp = om.MDagPath()
        di.getPath(dp)
        if dp.node().apiType() == om.MFn.kWorld:
            return dp
        di.next()

def needsAssembly(node):
    if node.parentCount() > 1:
        return True
    try:
        pnode = pm.PyNode(node.fullPathName())
    except:
        return False
    if pnode.type() == "transform":
        if len(pnode.inputs()) > 0:
            return True
    return False    

def isGeo(mobj):
    if mobj.hasFn(om.MFn.kMesh):
        return True
    return False
    
def isTransform(mobj):
    if mobj.hasFn(om.MFn.kTransform):
        return True
    return False


def parseChildren(parentNode, pAssembly, assemblyList, tMatrix):
    
    numChildren = parentNode.childCount()
    dagPath = om.MDagPath()
    parentNode.getPath(dagPath)
    parentObj = dagPath.node()
#    if dagPath.instanceNumber() > 0:
#        print "is inst", dagPath.fullPathName()
#        return
    
    #print "Parse", parentNode.fullPathName()
    for cId in range(numChildren):
        childObj = parentNode.child(cId)
        childNode = om.MFnDagNode(childObj)
        childDagPath = om.MDagPath()
        childNode.getPath(childDagPath)
        
        childParent = childNode.parent(0)
        childParentNode = om.MFnDagNode(childParent)

        if childNode.parent(0) != parentObj:
            print "parent von ", childNode.fullPathName(), "==", childParentNode.fullPathName(), "current parent:", parentNode.fullPathName()
            continue

        cMatrix = tMatrix
        if isTransform(childObj):
            cMatrix *= childNode.transformationMatrix()
            print "ParentAss Tranform", pAssembly.name
            print om.MScriptUtil.getDoubleArrayItem(cMatrix[3], 0)
            print om.MScriptUtil.getDoubleArrayItem(cMatrix[3], 1)
            print om.MScriptUtil.getDoubleArrayItem(cMatrix[3], 2)
                                
        if isGeo(childObj):
            print "Put geo", childNode.partialPathName(), "into parent assembly", pAssembly.name
            pAssembly.geometry.append(childNode.partialPathName())
                        
        parentAssembly = pAssembly               
        if needsAssembly(childNode):
            print "Child node", childNode.fullPathName(), "needs assembly"
            parentAssembly = assembly(childDagPath, childNode, childNode.fullPathName(), parentAss=pAssembly)
            pAssembly.assembly_instances.append(parentAssembly)
            assemblyList.append(parentAssembly)
            #parentAssembly = pAssembly
            
        parseChildren(childNode, parentAssembly, assemblyList, cMatrix)
    
def printAss(level, ass = None):
    sp = ""
    for i in range(level):
        sp += " "
    print sp+"AName: ", ass.name
    for i in ass.geometry:
        print sp, "geo:", i.partialPathName() 
    for i in ass.assembly_instances:
        print sp, "Ainstance", i.name
    for i in ass.assemblies:
        printAss(level+4, i)
    
        
def hierarchyA():
    worldDag = getWorld()
    worldDagNode = om.MFnDagNode(worldDag)
    assemblyList = []
    worldAssembly = assembly(worldDag, worldDagNode, "root")
    assemblyList.append(worldAssembly)
    #tMatrix = om.MMatrix()
    parseChildren(worldDagNode, worldAssembly, assemblyList, tMatrix)
    #printAss(0, worldAssembly)
    
    for ass in assemblyList:
        parentAss = ass.parentAss
        if parentAss:
            print "Assembly List: ", ass.name, "parent", ass.parentAss.name
        else:
            print "Assembly List: ", ass.name, "parent", "world"
        for ai in ass.assembly_instances:
            print ass.name, "Instance:", ai.name
        for ai in ass.geometry:
            print "->Geo:", ai
#            print om.MScriptUtil.getDoubleArrayItem(ass.transformMatrix[3], 0)
#            print om.MScriptUtil.getDoubleArrayItem(ass.transformMatrix[3], 1)
#            print om.MScriptUtil.getDoubleArrayItem(ass.transformMatrix[3], 2)
    
def hierarchy():
    
    assDagDict = {}
    liste = []
    di = om.MItDag(om.MItDag.kDepthFirst, om.MFn.kInvalid)
    while not di.isDone():
        dp = om.MDagPath()
        di.getPath(dp)
        obj = dp.node()
        print "Object", dp.fullPathName()
        if obj.apiType() == om.MFn.kWorld:
            "Object", dp.fullPathName(), "==kWorld"
        if dp.instanceNumber() > 0:
            di.next()
            continue
        dn = om.MFnDagNode(dp.node())
        
        if dp.node().hasFn(om.MFn.kMesh) or dp.node().hasFn(om.MFn.kTransform):
            node = None
            try: 
                node = pm.PyNode(dp.fullPathName())
            except:
                pass
            if node:
                #print dp.fullPathName()
                liste.append(mobject(dp))

        di.next()   
    
    for mo in liste:
        if mo.node.type() == "transform":
            print "Check T", mo.node
            if mo.node.inputs():
                print "Has input"
                mo.assembly = assembly()
                assDagDict[mo.dagNode.fullPathName()] = mo 
        if mo.node.type() == "mesh":
            print "Check M", mo.node
        if mo.dagNode.parentCount() > 1:
            print "Has multi parents"
            mo.assembly = assembly()
            assDagDict[mo.dagNode.fullPathName()] = mo 
    
    print assDagDict
    
    for mo in liste:
        if mo.node.type() == "mesh":
            print "Check Mesh Path", mo.node
            parentNode = mo.dagNode
            while parentNode:
                print "ParentPath", parentNode.fullPathName()
                if assDagDict.has_key(parentNode.fullPathName()):
                    mobj = assDagDict[parentNode.fullPathName()]
                    print "Put", mo.node, "into assembly", mobj.dagNode.fullPathName()
                    break
                pobj = parentNode.parent(0)
                #print pobj, pobj.apiTypeStr()
                if pobj.apiType() == om.MFn.kWorld:
                    print "Found world", "put node", mo.node, "into scene"
                    break
                parentNode = om.MFnDagNode(pobj)
                #print parentNode, parentNode.fullPathName()
                #if parentNode.dagPath().apiTypeStr() == om.MFn.kWorld:
                #    print "World!"
                #    break
    
        
            
    

