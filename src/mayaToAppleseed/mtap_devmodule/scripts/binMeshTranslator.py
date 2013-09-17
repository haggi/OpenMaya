import pymel.core as pm
import maya.OpenMayaMPx as OpenMayaMPx
import sys
import logging

log = logging.getLogger("mtapLogger")

def binMeshTranslatorOpts(parent, action, initialSettings, resultCallback):
    print "Parent", parent
    print "Action", action
    print "InitialSettings", initialSettings
    print "ResultCallback", resultCallback
    
    useTransform = True
    oneFilePerMesh = False
    createProxies = True
    proxyRes = 0.1
    exportDir = ""
    
    if initialSettings is not None and len(initialSettings) > 0:
        #oneFilePerMesh=0;createProxies=1;proxyRes=0.1;exportDir=;createProxies=1
        opts = initialSettings.split(";")
        for opt in opts:
            name, value = opt.split("=")
            if name == "oneFilePerMesh":
                oneFilePerMesh = int(value)
            if name == "createProxies":
                createProxies = int(value)
            if name == "useTransform":
                useTransform = int(value)
            if name == "proxyRes":
                proxyRes = float(proxyRes)
                
    if action == "post":
        print "post action"
        
        pm.setParent(parent)
        with pm.columnLayout(adj = True):
            pm.checkBox("MSH_OPTS_DOTRANSFORM", label = "Use Transform", v=useTransform)
            pm.checkBox("MSH_OPTS_ONEFILE", label = "One File Per Mesh", v=oneFilePerMesh)
            pm.checkBox("MSH_OPTS_DOPROX", label = "Create ProxyFiles", v=createProxies)
            pm.floatFieldGrp("MSH_OPTS_PROXPERC", label="Proxy Resolution", v1 = proxyRes)
            #pm.textFieldGrp("MSH_OPTS_PATH", label="ExportDir:", text=exportDir) 
        
    if action == "query":
        resultOptions = ""
        print "Query action"
        oneFilePerMesh = pm.checkBox("MSH_OPTS_ONEFILE", query=True, v=True)
        resultOptions += "oneFilePerMesh={0}".format(int(oneFilePerMesh))
        doProx = pm.checkBox("MSH_OPTS_DOPROX", query=True, v=True)
        resultOptions += ";createProxies={0}".format(int(doProx))
        proxyRes = pm.floatFieldGrp("MSH_OPTS_PROXPERC", query=True, v1 = True)
        resultOptions += ";proxyRes={0}".format(proxyRes)
        #exportDir = pm.textFieldGrp("MSH_OPTS_PATH", query=True, text = True)
        #resultOptions += ";exportDir='{0}'".format(exportDir)
        doTransform = pm.checkBox("MSH_OPTS_DOTRANSFORM", query=True, v=True)
        resultOptions += ";useTransform={0}".format(int(doTransform))
        
        melCmd = '{0} "{1}"'.format(resultCallback,resultOptions)
        pm.mel.eval(melCmd)
        
    return 1

def binMeshTranslatorWrite(fileName, optionString, accessMode):
    

    exportPath = fileName
    createProxies = False
    proxyRes = 0.1
    all = False
    oneFilePerMesh = False
    useTransform = False

    #print "OptionString", optionString
    # the very first sign is a ; so the first element cannot be split again, no idea why
    opts = optionString.split(";")
    for opt in opts:
        print "Opt", opt
        try:
            name, value = opt.split("=")
            if name == "oneFilePerMesh":
                oneFilePerMesh = int(value)
            if name == "createProxies":
                createProxies = int(value)
            if name == "useTransform":
                useTransform = int(value)
            if name == "proxyRes":
                proxyRes = float(proxyRes)
        except:
            pass
        
    if accessMode == "selected":
        print "binMeshTranslatorWrite selected"
        selection = []
        for object in pm.ls(sl=True):
            selection.extend(object.getChildren(ad=True, type="mesh"))
        if len(selection) == 0:
            print "Fehler: No mesh objects found in selection."
            raise
        pm.binMeshWriterCmd(selection, path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, oneFilePerMesh=oneFilePerMesh)

    if accessMode == "all":
        print "binMeshTranslatorWrite all"
        pm.binMeshWriterCmd(path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, all=True, oneFilePerMesh=oneFilePerMesh)
    
    return True

def binMeshTranslatorRead(fileName, optionString, accessMode):
    print "binMeshTranslatorRead"

    importPath = fileName
    createProxies = False
    proxyRes = 0.1

    #print "OptionString", optionString
    # the very first sign is a ; so the first element cannot be split again, no idea why
    opts = optionString.split(";")
    for opt in opts:
        print "Opt", opt
        try:
            name, value = opt.split("=")
            if name == "createProxies":
                createProxies = int(value)
            if name == "proxyRes":
                proxyRes = float(proxyRes)
        except:
            pass
        
    pm.binMeshReaderCmd(path=importPath)
    
    return True

def binMeshCheckAndCreateShadingGroup(shadingGroup):
    try:
        print "check ", shadingGroup
        shadingGroup = pm.PyNode(shadingGroup)
    except:
        log.debug("Shading group {0} does not exist, creating one.".format(shadingGroup)) 
        shader = pm.shadingNode("appleseedSurfaceShader", asShader=True)
        shadingGroup = pm.sets(renderable=True, noSurfaceShader=True, empty=True, name=shadingGroup)
        shader.outColor >> shadingGroup.surfaceShader

# I'd prefer to use the connected polyshape directly, but because of the connections in a multi shader assignenment
# I do not get a directo connection from the creator node to the polymesh but it goes to some groupId nodes what makes
# it a bit complicated to search for the first connected mesh in the API. So I simply call the script with the creator node
# and let the python script search for the mesh.

def listConnections(node, allList, meshList):
    for con in node.outputs(sh=True):
        if not con in allList:
            log.debug("Check {0}".format(con))
            allList.append(con)
            if con.type() == 'mesh':
                meshList.append(con)
            else:
                listConnections(con, allList, meshList)
        else:
            log.debug("Found existing shape, skipping.")
            
def getConnectedPolyShape(creatorShape):
    try:
        creatorShape = pm.PyNode(creatorShape)
    except:
        return None
    
    meshList = []
    allList = []
    listConnections(creatorShape, allList, meshList)

    return meshList
    
        
# perFaceAssingments contains a double list, one face id list for every shading group        
def binMeshAssignShader(creatorShape = None, shadingGroupList=[], perFaceAssingments=[]):
    if not creatorShape:
        log.error("binMeshAssignShader: No creator shape")
        return
    
    if pm.PyNode(creatorShape).type() != "mtap_standinMeshNode":
        log.error("binMeshAssignShader: Node {0} is not a creator shape".format(creatorShape))
        return

    log.debug("binMeshAssignShader: creatorShape {0}".format(creatorShape))        
    polyShapeList = getConnectedPolyShape(creatorShape)
        
    for polyShape in polyShapeList:    
        log.debug("binMeshAssignShader: polyShape {0}".format(polyShape))        
        for index, shadingGroup in enumerate(shadingGroupList):
            binMeshCheckAndCreateShadingGroup(shadingGroup)
            
            assignments = perFaceAssingments[index]
            print assignments
            faceSelection = []
            
            for polyId in assignments:
                faceSelection.append(polyId)
            
            print "meshShape has", polyShape.numFaces(), "faces. Edeff."
            fs = polyShape.f[faceSelection]
            print "pm.sets('{0}', e=True, forceElement={1})".format(shadingGroup,str(fs))
            pm.sets(shadingGroup, e=True, forceElement=str(fs))
        
        
        
        