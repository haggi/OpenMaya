import pymel.core as pm
import maya.OpenMayaMPx as OpenMayaMPx
import sys

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
            pm.textFieldGrp("MSH_OPTS_PATH", label="ExportDir:", text=exportDir) 
        
    if action == "query":
        resultOptions = ""
        print "Query action"
        oneFilePerMesh = pm.checkBox("MSH_OPTS_ONEFILE", query=True, v=True)
        resultOptions += "oneFilePerMesh={0}".format(int(oneFilePerMesh))
        doProx = pm.checkBox("MSH_OPTS_DOPROX", query=True, v=True)
        resultOptions += ";createProxies={0}".format(int(doProx))
        proxyRes = pm.floatFieldGrp("MSH_OPTS_PROXPERC", query=True, v1 = True)
        resultOptions += ";proxyRes={0}".format(proxyRes)
        exportDir = pm.textFieldGrp("MSH_OPTS_PATH", query=True, text = True)
        resultOptions += ";exportDir='{0}'".format(exportDir)
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
        pm.binMeshTranslatorCmd(selection, path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, oneFilePerMesh=oneFilePerMesh)

    if accessMode == "all":
        print "binMeshTranslatorWrite all"
        pm.binMeshTranslatorCmd(path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, all=True, oneFilePerMesh=oneFilePerMesh)
    
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
        
    if accessMode == "selected":
        pass
        #pm.binMeshTranslatorCmd(selection, path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, oneFilePerMesh=oneFilePerMesh)

    if accessMode == "all":
        pass
        #pm.binMeshTranslatorCmd(path=exportPath, doProxy=createProxies, percentage=proxyRes, doTransform=useTransform, all=True, oneFilePerMesh=oneFilePerMesh)
    
    return True

BinMeshTranslatorName = "appleseedBinaryMesh"

# Node definition
class BinMeshTranslator(OpenMayaMPx.MPxFileTranslator):
    def __init__(self):
        OpenMayaMPx.MPxFileTranslator.__init__(self)
        
    def haveWriteMethod(self):
        return True
    
    def haveReadMethod(self):
        return True

    def filter(self):
        return "*.binarymesh"
    
    def defaultExtension(self):
        return "binarymesh"

    def writer( self, fileObject, optionString, accessMode ):
        print "BinMeshTranslator:Writer"
        if accessMode == OpenMayaMPx.MPxFileTranslator.kExportActiveAccessMode:
            binMeshTranslatorWrite(fileObject.fullName(), optionString, "selected")
        if accessMode == OpenMayaMPx.MPxFileTranslator.kExportAccessMode:
            binMeshTranslatorWrite(fileObject.fullName(), optionString, "all")

    def reader( self, fileObject, optionString, accessMode ):
        print "BinMeshTranslator:Reader"
        binMeshTranslatorRead(fileObject.fullName(), optionString, accessMode)

def translatorCreator():
    return OpenMayaMPx.asMPxPtr( BinMeshTranslator() )

def initializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.registerFileTranslator( BinMeshTranslatorName, None, translatorCreator, "binMeshTranslatorOpts", "", True)
        melToPythonCmd = """global proc binMeshTranslatorOpts(string $a, string $b, string $c, string $d)\n
{
    python(\"import binMeshTranslator; binMeshTranslator.binMeshTranslatorOpts('\" + $a + \"','\" + $b + \"','\" + $c + \"','\" + $d + \"')\");
}"""
        print "melToPythonCmd", melToPythonCmd
        pm.mel.eval(melToPythonCmd)
    
        toolsName = "appleseedTools_maya2014"
        if not pm.pluginInfo(toolsName, query=True, loaded=True):
            try:
                pm.loadPlugin(toolsName)
            except:
                sys.stderr.write( "Failed to load appleseed tools: %s" % toolsName )
                raise
                
    except:
        sys.stderr.write( "Failed to register translator: %s" % BinMeshTranslatorName )
        raise
    
    

def uninitializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.deregisterFileTranslator( BinMeshTranslatorName )
    except:
        sys.stderr.write( "Failed to deregister translator: %s" % BinMeshTranslatorName )
        raise
    

