import pymel.core as pm
import maya.OpenMaya as om
import math
import logging 
log = logging.getLogger("mtapLogger")

def createPackage():
    pm.version()
    pass


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

instList = []

def parseChildren(parentNode, pAssembly, assemblyList):
    
    numChildren = parentNode.childCount()
    dagPath = om.MDagPath()
    parentNode.getPath(dagPath)
    parentObj = dagPath.node()
#    if dagPath.instanceNumber() > 0:
#        print "is inst", dagPath.fullPathName()
#        return
    
    if dagPath.isInstanced():
        instList.append(dagPath)
        
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

        if isTransform(childObj):
            print "ParentAss Transform", pAssembly.name
                                
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
            
        parseChildren(childNode, parentAssembly, assemblyList)
    
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
    parseChildren(worldDagNode, worldAssembly, assemblyList)
    #printAss(0, worldAssembly)
    
#    for ass in assemblyList:
#        parentAss = ass.parentAss
#        if parentAss:
#            print "Assembly List: ", ass.name, "parent", ass.parentAss.name
#        else:
#            print "Assembly List: ", ass.name, "parent", "world"
#        for ai in ass.assembly_instances:
#            print ass.name, "Instance:", ai.name
#        for ai in ass.geometry:
#            print "->Geo:", ai

    for i in instList:
        print "Instance", i.fullPathName()
        dpa = om.MDagPathArray()
        dn = om.MFnDagNode(i)
        dn.getAllPaths(dpa)
        print "NumPaths", dpa.length()
        for dp in range(dpa.length()):
            print dpa[dp].fullPathName()
    
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
    
def CartesianToPolar(rotation):
    
    v = om.MVector(0,0,1)
    m = om.MTransformationMatrix()
    rotation = map(math.radians, rotation)
    e = om.MEulerRotation()
    e.setValue(rotation[0], rotation[1], rotation[2])
    m.rotateBy(e, om.MSpace.kWorld)
    v = v * m.asMatrix()
#    r = math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
#    print "V", v.x, v.y, v.z, r
#    phi = math.pi/2.0
#    theta = 1.0
#    #if v.x != 0.0:
#    phi = math.atan2(v.y,v.x)
#    #if v.z != 0.0:
#    theta = math.atan2(math.sqrt(v.x * v.x + v.y * v.y),v.z)
    #if v.x != 0.0:
    #    phi = math.atan(v.y/v.x)
    #else:
    #    phi = math.pi/2.0
    #theta = math.acos(v.z/r)
    theta = math.asin(v.y)
    phi = math.asin(v.z)
    t = math.degrees(theta)
    p = math.degrees(phi)
    
    if v.x < 0.0:
        p = 180 - p
    
    polar = (p,t)
    return polar


class aov:
    def __init__(self):
        self.rendererTabUiDict = {}
        self.aovShaders = ["mtap_aoShader", "mtap_aoVoxelShader",  "mtap_diagnosticShader", "mtap_fastSSSShader", "appleseedSurfaceShader"]
        self.renderGlobalsNode = pm.PyNode("appleseedGlobals")
        self.AppleseedAOVsCreateTab()
    
    def AppleseedAOVSelectCommand(self, whichField):
        log.debug("AppleseedAOVSelectCommand")
        aovDict = self.rendererTabUiDict['aovs']
        if whichField == "source":
            pm.button(aovDict['aovButton'], edit=True, enable=True, label="Add selected Shaders")
            pm.textScrollList(aovDict['aovDestField'], edit=True, deselectAll=True) 
        if whichField == "dest":
            pm.button(aovDict['aovButton'], edit=True, enable=True, label="Remove selected Shaders")
            pm.textScrollList(aovDict['aovSourceField'], edit=True, deselectAll=True) 

    def AppleseedAOVUpdateDestList(self):
        aovDict = self.rendererTabUiDict['aovs']
        pm.textScrollList(aovDict['aovDestField'], edit=True, removeAll=True) 
        aovList = self.AppleseedGetAOVConnections()
        pm.textScrollList(aovDict['aovDestField'], edit=True, append=aovList)
        
    def AppleseedAOVButtonCommand(self, args=None):
        log.debug("AppleseedAOVButtonCommand " + str(args))
        aovDict = self.rendererTabUiDict['aovs']
        label = pm.button(aovDict['aovButton'], query=True, label=True)
        if "Add selected Shaders" in label:
            log.debug("AppleseedAOVButtonCommand: adding selected shaders")
            selectedItems = pm.textScrollList(aovDict['aovSourceField'], query=True, selectItem=True) 
            for item in selectedItems:
                log.debug("Adding " + item)
                node = pm.createNode(item)
                indices=self.renderGlobalsNode.AOVs.getArrayIndices()
                index = 0
                if len(indices) > 0:
                    index = indices[-1] + 1
                node.message >> self.renderGlobalsNode.AOVs[index]
            self.AppleseedAOVUpdateDestList()
            
        if "Remove selected Shaders" in label:
            log.debug("AppleseedAOVButtonCommand: removing selected shaders")
            selectedItems = pm.textScrollList(aovDict['aovDestField'], query=True, selectItem=True) 
            for item in selectedItems:
                shaderName = item.split(" ")[0]
                log.debug("Removing " + shaderName)
                shader = pm.PyNode(shaderName)
                attribute = shader.message.outputs(p=True)[0]
                pm.delete(shader)
                attribute.remove()
            self.AppleseedAOVUpdateDestList()
            
    def AppleseedDoubleClickCommand(self):
        log.debug("AppleseedDoubleClickCommand")
        aovDict = self.rendererTabUiDict['aovs']
        selectedItems = pm.textScrollList(aovDict['aovDestField'], query=True, selectItem=True) 
        pm.select(selectedItems[0].split(" ")[0])
            
    def AppleseedGetAOVConnections(self):
        aoList = self.renderGlobalsNode.AOVs.inputs()
        aovList = []
        for aov in aoList:
            aovList.append(aov.name()+" (" + str(aov.type()) + ")")
        return aovList
            
    def AppleseedAOVsCreateTab(self):
        log.debug("AppleseedAOVsCreateTab()")
        aovDict = {}
        self.rendererTabUiDict['aovs'] = aovDict
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        with pm.window() as win:
            scLo = "AOScrollLayout"
            with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
                with pm.columnLayout("ColumnLayout", adjustableColumn = True, width = 400):
                    with pm.frameLayout(label="AOVs", collapsable = True, collapse=False):
                        with pm.columnLayout():
                            with pm.paneLayout(configuration="vertical2", paneSize=(1, 25, 100)):
                                aovDict['aovSourceField'] = pm.textScrollList("AOVSource", ams=True, append=self.aovShaders, selectCommand = pm.Callback(self.AppleseedAOVSelectCommand,"source"))
                                aovList = self.AppleseedGetAOVConnections()
                                aovDict['aovDestField'] = pm.textScrollList("AOVDest", append=aovList, ams=True, dcc=self.AppleseedDoubleClickCommand, selectCommand = pm.Callback(self.AppleseedAOVSelectCommand,"dest"))
                            aovDict['aovButton'] = pm.button(label="Selection", enable=False, c=self.AppleseedAOVButtonCommand)
        
        win.show()
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)





