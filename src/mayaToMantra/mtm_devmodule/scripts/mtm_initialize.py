import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import mtm_mantraAttributes as mantraAttributes

reload(Renderer)

log = logging.getLogger("mtmLogger")

class MantraRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg=None):
        if not MantraRenderer.theRendererInstance:
            MantraRenderer.theRendererInstance = MantraRenderer("Mantra", "mtm_initialize")
        return MantraRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        if not pm.about(batch=True):
            mantraMainMenu()

    
    def getEnumList(self, attr):
        return [(i, v) for i, v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy=None):
        print "UpdateTest", dummy             
        
    def MantraRendererCreateTab(self):
        log.debug("MantraRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True):
                mantraAttributes.mantraGlobalsATList.createUi(self.renderGlobalsNodeName)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.MantraRendererUpdateTab()

    def MantraRendererUpdateTab(self, dummy=None):
        log.debug("MantraRendererUpdateTab()")
        if self.renderGlobalsNode.adaptiveSampling.get():
            self.rendererTabUiDict['minSamples'].setEnable(True)
            self.rendererTabUiDict['maxError'].setEnable(True)
        else:
            self.rendererTabUiDict['minSamples'].setEnable(False)
            self.rendererTabUiDict['maxError'].setEnable(False)


    def MantraTranslatorCreateTab(self):
        log.debug("MantraTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn=True):
                mantraAttributes.mantraTranslatorATList.createUi(self.renderGlobalsNodeName)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def MantraTranslatorUpdateTab(self):
        log.debug("MantraTranslatorUpdateTab()")

    def createImageFormats(self):
        self.imageFormats.append('exr')
        self.imageFormats.append('png')


    def registerNodeExtensions(self):
        """Register Mantra specific node extensions. e.g. reflection blur, area light type etc.
        """
        pm.addExtension(nodeType="spotLight", longName="mtm_deepShadowMap", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="spotLight", longName="mtm_spread", attributeType="double", defaultValue=1.0)
        pm.addExtension(nodeType="spotLight", longName="mtm_bias", attributeType="double", defaultValue=0.05)
        pm.addExtension(nodeType="spotLight", longName="mtm_blur", attributeType="double", defaultValue=0.0)
        pm.addExtension(nodeType="spotLight", longName="mtm_shadowQuality", attributeType="double", defaultValue=1.0)
        pm.addExtension(nodeType="spotLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
        pm.addExtension(nodeType="spotLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
    
        # area light
        pm.addExtension(nodeType="areaLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
        pm.addExtension(nodeType="areaLight", longName="mtm_areaType", attributeType="enum", enumName="line:grid:disk:sphere")
        pm.addExtension(nodeType="areaLight", longName="mtm_normalizeArea", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="mtm_useLightGeometry", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="areaLight", longName="mtm_lightGeometry", attributeType="message")
        pm.addExtension(nodeType="areaLight", longName="mtm_renderLightGeometry", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="areaLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
        
        #point light
        pm.addExtension(nodeType="pointLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
        pm.addExtension(nodeType="pointLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
    
        #directional light
        pm.addExtension(nodeType="directionalLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
        pm.addExtension(nodeType="directionalLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
    
        pm.addExtension(nodeType="spotLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="areaLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="pointLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="directionalLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="volumeLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
        
        pm.addExtension(nodeType="light", longName="mtm_fastShadows", attributeType="bool", defaultValue=False)
    
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtm_renderAsSubdiv", attributeType="bool", defaultValue=False)
        # nurbsCurve
        pm.addExtension(nodeType="nurbsCurve", longName="primaryVisibility", attributeType="bool", defaultValue=True)
    
        # displacement shader
        pm.addExtension(nodeType="displacementShader", longName="mtm_displacebound", attributeType="double", defaultValue=1.0)
        
        # phong, blinn, phongE, lambert ...
        pm.addExtension(nodeType="phong", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="phongE", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="blinn", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="lambert", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="lambert", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min=0.0, max=1.0)
        pm.addExtension(nodeType="phong", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min=0.0, max=1.0)
        pm.addExtension(nodeType="blinn", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min=0.0, max=1.0)
        
        pm.addExtension(nodeType="phong", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
        pm.addExtension(nodeType="phongE", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
        pm.addExtension(nodeType="blinn", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
        pm.addExtension(nodeType="lambert", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
    
        pm.addExtension(nodeType="phong", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="phongE", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
        pm.addExtension(nodeType="blinn", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min=0.0, softMaxValue=1.0)
    
        pm.addExtension(nodeType="phong", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
        pm.addExtension(nodeType="phongE", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
        pm.addExtension(nodeType="blinn", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
        
        # orenNayar
        pm.addExtension(nodeType="lambert", longName="mtm_orenNayar", attributeType="long", defaultValue=0)
    
    def renderProcedure(self):
        log.debug("mtmRenderProcedure")
    
        pm.mel.trace("================ Start MayaToMantra Rendering ======================")
        # TODO: get directorys and filenames
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.renderGlobalsNode.basePath.set(pm.workspace.path)
        self.renderGlobalsNode.imagePath.set(pm.workspace.path + pm.workspace.fileRules['images'])
        imageName = pm.sceneName().basename().replace(".ma", "").replace(".mb", "")
        # check for mayabatch name like sceneName_number 
        numberPart = imageName.split("__")[-1]
        try:
            number = int(numberPart)
            if number > 999:
                imageName = imageName.replace("__" + numberPart, "")
        except:
            pass
        self.renderGlobalsNode.imageName.set(imageName)        

        outputPath = pm.workspace.path + "/mantra/" + realSceneName()
        imagePath = pm.workspace.path + "/images"
        
        if not os.path.exists(outputPath):
            os.makedirs(outputPath)
        if not os.path.exists(imagePath):
            os.makedirs(imagePath)
    
        shaderOutputPath = outputPath + "/shaders"
        if not os.path.exists(shaderOutputPath):
            os.makedirs(shaderOutputPath)
        geoOutputPath = outputPath + "/geo"
        if not os.path.exists(geoOutputPath):
            os.makedirs(geoOutputPath)
        
        pm.mayatomantra()
        
        

def theRenderer():
    return MantraRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Mantra")
        theRenderer().registerRenderer()
    except Exception:
        traceback.print_exc(file=sys.__stdout__)
        sys.__stdout__.write("\nInit renderer Mantra FAILED\n")
        log.error("Init renderer Mantra FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")


class StandinGUI(pm.ui.Window):
    _singleton = True
    def __new__(cls, *args):
        return super(StandinGUI, cls).__new__(cls)
    
    def createStandin(self):
        log.debug("StandinGUI: create standin")
        print self.fileName, self.showPoints, self.density, self.pointSize
        standin = pm.createNode("mtm_standin")
        fn = os.path.basename(self.fileName).replace(".geo", "").replace(".bgeo", "").replace(".", "_")
        standin.rename(fn + "Shape")
        standin.getParent().rename(fn)
        
        standin.geoFile.set(self.fileName)
        standin.showPoints.set(self.showPoints)
        standin.pointPercent.set(self.density)
        pm.select(standin.getParent())
        self.delete()
        
        
    def selectGeoFile(self):
        print "Select file"
        startDir = r"C:\daten\3dprojects\mantra\data\fluidGeo"
        # fileFilter="*.geo;;*.bgeo" seems not to work correctly...
        self.fileName = pm.fileDialog2(cap="Select Geo File", dir=startDir, fm=1)
        if len(self.fileName) > 0:
            self.fileName = self.fileName[0]
            pm.textFieldButtonGrp("stdingfilename", edit=True, text=self.fileName)
                
    def setValue(self, value):
        if value == "stdingfilename":
            self.fileName = pm.textFieldButtonGrp("stdingfilename", query=True, text=True)
        if value == "stdinshowpoints":
            self.showPoints = pm.checkBoxGrp("stdinshowpoints", query=True, value1=True)
        if value == "stdinptdensity":
            self.density = pm.floatFieldGrp("stdinptdensity", query=True, value1=True)          
        if value == "stdinptsize":
            self.pointSize = pm.intFieldGrp("stdinptsize", query=True, value1=True)
            
    def __init__(self):
        self.title = "Mantra Standin"
        self.fileName = ""
        self.density = .3
        self.pointSize = 1
        self.showPoints = False
        self.boundingBox = [-.5, -.5, -.5, .5, .5, .5]
        
        pm.setUITemplate("DefaultTemplate", pushTemplate=True)
        self.height = 500
        with pm.frameLayout(collapsable=False, labelVisible=False) as frame:
            pm.textFieldButtonGrp("stdingfilename", label="GeoFileName", text="", buttonLabel="Select", bc=pm.Callback(self.selectGeoFile), cc=pm.Callback(self.setValue, "stdingfilename"))
            #string $b = `symbolButton -image "navButtonBrowse.xpm" browser`;
            with pm.frameLayout(collapsable=False, label="Points"):
                pm.checkBoxGrp("stdinshowpoints", label="Show Points", value1=False, cc=pm.Callback(self.setValue, "stdinshowpoints"))
                pm.floatFieldGrp("stdinptdensity", label="PointDensity", value1=0.3, cc=pm.Callback(self.setValue, "stdinptdensity"))
                pm.intFieldGrp("stdinptsize", label="PointSize", value1=1, cc=pm.Callback(self.setValue, "stdinptsize"))
            with pm.rowLayout(nc=2):
                pm.button(label="Create Standin", c=pm.Callback(self.createStandin))            
                pm.button(label="Cancel", c=pm.Callback(self.delete))        
        self.show()

RENDERERNAME = "Mantra"
GLOBALSNAME = "mayaToMantraGlobals"


def realSceneName(doDot=False):
    snel = pm.sceneName().basename().split(".")
    sn = snel[0]
    if len(sn.split("__")) > 1:
        elements = sn.split("__")
        num = elements[1]
        if num.isdigit():
            log.debug("Scene has pattern name__number.xx seems to be a batchrender scene, deleting number.")
            sn = elements[0]
    if doDot:
        sn += "." + snel[1]
    return sn


def prepareEnv():
    
    log.debug("prepareEnv")
    if os.environ.has_key("MTM_ENVDONE"):
        if os.environ['MTM_ENVDONE'] == "TRUE":
            log.debug("Env already initialized, skipping.")
            return True

    if not os.environ.has_key('H'):
        if pm.about(batch=True):
            log.error("Environment variable 'H' not set. Cannot find houdini. Exiting.")
        else:
            pm.displayWarning("Environment variable 'H' not set. Cannot find houdini. Exiting.")
        return False
    
    if not os.environ.has_key("MTM_HOME"):
        if pm.about(batch=True):
            log.error("Environment variable 'MTM_HOME' not set. Cannot find installdir. Exiting.")
        else:
            pm.displayWarning("Environment variable 'MTM_HOME' not set. Cannot find installdir. Exiting.")
        return False

    tmpdir = ""
    if not os.environ.has_key('TMP'):
        if not os.environ.has_key('tmp'):
            # TODO: correct tmpdir
            pass
        else:
            tmpdir = os.environ['tmp']
    else:
        tmpdir = os.environ['TMP']
    basePath = os.environ['H']

    os.environ['HTB'] = basePath + "/toolkit/bin"     
    username = getpass.getuser()

    currentDir = pm.workspace.path
    sn = realSceneName()
        
    os.environ['HB'] = basePath + "/bin" 
    os.environ['HD'] = basePath + "/demo" 
    os.environ['HFS'] = basePath
    os.environ['HH'] = basePath + "/houdini" 
    os.environ['HHC'] = basePath + "/houdini/config" 
    os.environ['HIP'] = currentDir + "/mantra/" + sn
    os.environ['HIPNAME'] = "untitled.hip" 
    os.environ['HOUDINI_TEMP_DIR'] = tmpdir + "/houdini" 
    os.environ['HSITE'] = basePath + "/site" 
    os.environ['HT'] = basePath + "/toolkit" 
    os.environ['HTB'] = basePath + "/toolkit/bin" 
    #os.environ['HOUDINI_OTL_PATH'] = os.environ['HOUDINI_OTL_PATH'] + ";" + currentDir + "/mantra/" + sn + "/shaders"
    os.environ['HOUDINI_VEX_PATH'] = currentDir + "/mantra/" + sn + "/shaders" + ";&;"
    path = os.environ['PATH']
    if not (basePath + "/bin") in path:
        os.environ['PATH'] = basePath + "/bin;" + path     
    os.environ['MTM_ENVDONE'] = "TRUE"     
    log.debug("prepareEnv done")

    return True

def startRenderProc(ifdFile):
    log.info("Starting render procedure")
    prepareEnv()
    IDLE_PRIORITY_CLASS = 64
    verbosity = 4
    try:
        renderGlobals = ls(type="mayaToMantraGlobals")
        verbosity = renderGlobals.verbosity.get()
    except:
        pass
    
    cwd = os.getcwd()
    os.chdir(os.path.dirname(ifdFile).replace("/ifd", "/shaders"))
    
    cmd = "mantra -V " + str(verbosity) + " -f " + ifdFile
    log.info("Starting cmd %s" % cmd)
    process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)

    while 1:
        line = process.stdout.readline()
        if not line: break
        
        # log.info(line.strip())
        pm.mel.trace(line.strip())
        #log.debug(line.strip())
        if "Render Time:" in line:
            break
    os.chdir(cwd)    
    log.info("Rendering done.")

def startHoudiniObjectTools(geoFile="", ptc=False, density=0.1, ptcBBox=True):
    #HoudiniObjectTools.exe C:\daten\3dprojects\mantra\mantra\mayaToMantra_fluid\geo
    if geoFile == "":
        log.error("startHoudiniObjectTools: no file to operate on")
        return False
    prepareEnv()
    converterPath = os.environ["MTM_HOME"] + "/bin/HoudiniObjectTools.exe -v 5 "

    cmd = ""    
    if ptc:
        cmd = converterPath + " -ptc " + geoFile + " -ptcd " + str(density)
    else:
        cmd = converterPath + " -bbox " + geoFile

    if not os.path.exists(geoFile):
        log.error("geoFile " + geoFile + " does not exist.")
        return
    ptcFile = geoFile.replace(".geo", ".hsffp").replace(".bgeo", ".hsffp")
    doConvert = True
    if os.path.exists(ptcFile):
        mtimeg = pm.Path(geoFile).mtime
        mtimep = pm.Path(ptcFile).mtime
        if mtimep > mtimeg:
            log.debug("ptcFile is younger than geo file, no conversion necessary")
            doConvert = False
        
    # no conversion, only read bbox
    if not doConvert:
        cmd = converterPath + " -bbox " + geoFile
        
    log.info("Starting HoudiniObjectTools cmd: %s" % cmd)
    IDLE_PRIORITY_CLASS = 64
    process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
    
    output = []
    bbox = []
    while 1:
        line = process.stdout.readline()
        #BBox: -0.110311 -1.26636 -1.17787 2.24247 2.5 1.17811
        if "BBox: " in line:
            bbox = line.strip().replace("BBox: ", "").split(" ")
            bbox = map(float, bbox)
        output.append(line)
        if not line: break        
        pm.mel.trace(line.strip())
        #log.debug(line.strip())
    #print "Bounding box", bbox
    return bbox
        

def startGeoConverter():
    #HoudiniObjectSaver.exe C:\daten\3dprojects\mantra\mantra\mayaToMantra_fluid\geo
    prepareEnv()
    converterPath = os.environ["MTM_HOME"] + "/bin/HoudiniObjectTools.exe "
    geoType = pm.SCENE.mayaToMantraGlobals.geoFileType.get()
    geoPath = pm.SCENE.mayaToMantraGlobals.basePath.get() + "/geo"
    
    # 0 == binary, 1 == ascii
    if geoType == 0:
        geoType = " -b "
    else:
        geoType = ""
    cmd = converterPath + " -cd " + geoPath + " " + geoType
    log.info("Starting HoudiniObjectSaver cmd: %s" % cmd)
    IDLE_PRIORITY_CLASS = 64
    process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
    
    while 1:
        line = process.stdout.readline()
        if not line: break        
        pm.mel.trace(line.strip())
        #log.debug(line.strip())
    
    log.info("GeoConversion done.")
    
def startShaderCompiler(shaderLib, shaderFile):
    log.info("Starting vcc compiler procedure")
    prepareEnv()
    IDLE_PRIORITY_CLASS = 64
    baseIncludePath = os.environ['H'] + "/houdini/vex/include"
    includePath = os.environ["MTM_HOME"] + "/shaderIncludes"
    includePathList = [baseIncludePath, includePath]
    includePathString = ""
    for ip in includePathList:
        includePathString += " -I \"" + ip + "\"" 
    cwd = os.getcwd()

    # go into shader directory because the -x option is needed to create .vex files and they are
    # placed into the current directory
    if not os.path.exists(shaderFile):
        log.error("Shader file " + shaderFile + " does not exist, noting to compile.")
        return False
    
    os.chdir(os.path.dirname(shaderFile))
    cmd = "vcc " + includePathString + " -x " + shaderFile
    #cmd = "vcc " + includePathString + " -x -m " + shaderLib + " " + shaderFile #--- h11
    log.info("Starting vcc cmd: %s" % cmd)
    process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
    
    while 1:
        line = process.stdout.readline()
        if not line: break        
        pm.mel.trace(line.strip())
        #log.debug(line.strip())
    log.info("Compiling done.")
    os.chdir(cwd)

def menuCallback(arg=None):
    if arg == "GeoStandin":
        StandinGUI()
        
def mantraMainMenu():
    log.debug("Creating mantra main menu")
    menuName = "Mantra"
    if pm.menu(menuName, query=True, exists=True):
        pm.deleteUI(menuName)
    gMainWindow = pm.mel.eval('$tmpVar=$gMainWindow')
    mantraMenu = pm.menu(menuName, label=menuName, parent=gMainWindow, tearOff=True)
    pm.menuItem(label='Create Geo Standin', command=pm.Callback(menuCallback, "GeoStandin"))
    pm.setParent("..", menu=True)

#import pymel.core as pm 
#import pymel.core.uitypes as pui
#from pymel.core import Callback
#import maya.cmds as cmds
#import logging
#import os
#import getpass
#import subprocess
#import mtm_mantraAttributes as mantraAttributes
#import mtm_attributeManager
#import shutil
#import sys
##import AETemplates.AEdagNodeTemplate as dgTmpl
#
#reload(mantraAttributes)
#log = logging.getLogger("mantraLogger")
#
#    
#
##def attributeManager():
##    reload(mtm_attributeManager)
#    #mtm_attributeManager.attributeManager()
##    mtm_attributeManager.GUI()
#
#    
#def createGeoStandin():
#    log.debug("Creating geo standin.")
#    
#
#    
#def renderProcedure():
#    log.debug("RenderProcedure")
#    
#    pm.mel.trace("================ Start MayaToMantra Rendering ======================")
#    # TODO: get directorys and filenames
#    currentTime = pm.currentTime(query = True)
#    outputPath = pm.workspace.path + "/mantra/" + realSceneName()
#    imagePath = pm.workspace.path + "/images"
#    imageName = realSceneName()
#    try:
#        mantraGlobals = pm.ls(type="mayaToMantraGlobals")[0]
#    except:
#        log.error("mayaToMantraGlobals not found")
#        return 
#    
#    mantraGlobals.basePath.set(outputPath)
#    mantraGlobals.imagePath.set(imagePath)
#    mantraGlobals.imageName.set(imageName)
#    if not os.path.exists(outputPath):
#        os.makedirs(outputPath)
#    if not os.path.exists(imagePath):
#        os.makedirs(imagePath)
#
#    shaderOutputPath = outputPath + "/shaders"
#    if not os.path.exists(shaderOutputPath):
#        os.makedirs(shaderOutputPath)
#    geoOutputPath = outputPath + "/geo"
#    if not os.path.exists(geoOutputPath):
#        os.makedirs(geoOutputPath)
#    
#    pm.mayatomantra()
#    
#    mantraGlobals.basePath.set("")
#    mantraGlobals.imagePath.set("")
#    mantraGlobals.imageName.set("")
#    pm.currentTime(currentTime)
#    
#def renderOptionsProcedure():
#    log.debug("RenderOptionsProcedure")
#
#def commandRenderProcedure():
#    log.debug("commandRenderProcedure")
#    renderProcedure()
# #   reload(mantraRender)
# #   mar = mantraRender.mantraRenderer()
# #   mar.batchRendering = True
# #   mar.render()
#
#def batchRenderProcedure():
#    log.debug("batchRenderProcedure")
#    renderProcedure()
#    
#def batchRenderOptionsProcedure():
#    log.debug("batchRenderOptionsProcedure")
#
#def batchRenderOptionsStringProcedure():
#    log.debug("batchRenderOptionsStringProcedure")
#
#def cancelBatchRenderProcedure():
#    log.debug("cancelBatchRenderProcedure")
#
#def showBatchRenderProcedure():
#    log.debug("showBatchRenderProcedure")
#
#def showRenderLogProcedure():
#    log.debug("showRenderLogProcedure")
#
#def showBatchRenderLogProcedure():
#    log.debug("showBatchRenderLogProcedure")
#
#def renderRegionProcedure():
#    log.debug("renderRegionProcedure")
#
#def textureBakingProcedure():
#    log.debug("textureBakingProcedure")
#
#def renderingEditorsSubMenuProcedure():
#    log.debug("renderingEditorsSubMenuProcedure")
#
#def renderMenuProcedure():
#    log.debug("renderMenuProcedure")
#
#def removeRenderer():
#    pm.renderer(RENDERERNAME,  edit = True, unregisterRenderer = True)
#    menuName = RENDERERNAME
#    if pm.menu(menuName, query=True, exists=True):
#        pm.deleteUI(menuName)
#
#def dimConnections(element):
#    print "DimConnections"
#    if element['name'] in ['motionblur']:
#        value = pm.checkBoxGrp( element['name'], query = True, v1 = True)
#        print element['uielement']
#        valueInv = value
#        #groups = mantraGlobalsAttributes.mantraGlobalsAttributes
#        #if el['name'] in ['xftimesamples', 'geotimesamples', 'motionfactor', 'mbtype']:
#        if value:
#            print "enabling ctrls"            
#        else:
#            print "disabling ctrls"
#        pm.intFieldGrp('xftimesamples', edit = True, enable = valueInv)
#        pm.intFieldGrp('geotimesamples', edit = True, enable = valueInv)
#        pm.floatFieldGrp('motionfactor', edit = True, enable = valueInv)
#        pm.attrEnumOptionMenu('mbtype', edit = True, enable = valueInv)
#        pm.checkBoxGrp('imagemotionblur',  edit = True, enable = valueInv)
#
#def MantraTabCreateFromAtt():
#    log.debug("MantraTabCreate from Attribute List")
#    reload(mantraAttributes)
#
#    if len(pm.ls(type = GLOBALSNAME)) == 0:
#        mrg = pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
#    else:
#        mrg = pm.ls(type = GLOBALSNAME)[0]
#        
#    parentForm = pm.setParent(query = True)
#    pm.setUITemplate( "attributeEditorTemplate", pushTemplate = True)
#    pm.scrollLayout( "MantraScrollLayout", horizontalScrollBarThickness = 0)
#    pm.columnLayout("MantraColumnLayout", adjustableColumn = True)
#
#    mantraAttributes.mantraGlobalsATList.createUi(mrg)
#    
#    pm.setUITemplate( "attributeEditorTemplate", popTemplate = True)
#    pm.formLayout(parentForm, edit = True, attachForm = [ ("MantraScrollLayout", "top", 0),
#                                                          ("MantraScrollLayout", "bottom", 0),
#                                                          ("MantraScrollLayout", "left", 0),
#                                                          ("MantraScrollLayout", "right", 0)
#                                                        ])
#
#
#def MantraTabUpdate():
#    log.debug("MantraTabUpdate")
#
#def MantraTranslatorTabCreate():
#    log.debug("MantraTabCreate from Attribute List")
#    reload(mantraAttributes)
#
#    if len(pm.ls(type = GLOBALSNAME)) == 0:
#        mrg = pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
#    else:
#        mrg = pm.ls(type = GLOBALSNAME)[0]
#        
#    parentForm = pm.setParent(query = True)
#    pm.setUITemplate( "attributeEditorTemplate", pushTemplate = True)
#    pm.scrollLayout( "MantraTrScrollLayout", horizontalScrollBarThickness = 0)
#    pm.columnLayout("MantraTrColumnLayout", adjustableColumn = True)
#
#    mantraAttributes.mantraTranslatorATList.createUi(mrg)
#    
#    pm.setUITemplate( "attributeEditorTemplate", popTemplate = True)
#    pm.formLayout(parentForm, edit = True, attachForm = [ ("MantraTrScrollLayout", "top", 0),
#                                                          ("MantraTrScrollLayout", "bottom", 0),
#                                                          ("MantraTrScrollLayout", "left", 0),
#                                                          ("MantraTrScrollLayout", "right", 0)
#                                                        ])
#
#
#def MantraTranslatorTabUpdate():
#    log.debug("MantraTranslatorTabUpdate")
#
#def melGlobals():
#    log.debug("melGlobals")
#    createMantraTab = "global proc createRendererTab()\
#    {\
#     print(\"createMelTab\");\n\
#     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTabCreateFromAtt()\");\n\
#    };"
#    updateMantraTab = "global proc updateRendererTab()\
#    {\
#     print(\"updateMelTab\");\n\
#     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTabUpdate()\");\n\
#    };"
#    pm.mel.eval(createMantraTab)
#    pm.mel.eval(updateMantraTab)
#
#    createMantraTab = "global proc createTranslatorTab()\
#    {\
#     print(\"createMelTab\");\n\
#     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTranslatorTabCreate()\");\n\
#    };"
#    updateMantraTab = "global proc updateTranslatorTab()\
#    {\
#     print(\"updateMelTab\");\n\
#     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTranslatorTabUpdate()\");\n\
#    };"
#    pm.mel.eval(createMantraTab)
#    pm.mel.eval(updateMantraTab)
#    
#    log.debug("melGlobals done")
#
#def registerRenderer():
#    log.debug("registerRenderer")
#    
#    baseCmd = "import mtm_initialize as mtmi;reload(mtmi);"
#    renderProc = baseCmd + "mtmi.renderProcedure()"    
#    batchRenderProc = baseCmd + "mtmi.batchRenderProcedure()"    
#    commandRenderProc = baseCmd + "mtmi.commandRenderProcedure()"    
#    batchRenderOptionsProc = baseCmd + "mtmi.batchRenderOptionsProcedure()"    
#    batchRenderOptionsStringProc = baseCmd + "mtmi.batchRenderOptionsStringProcedure()"    
#    pm.renderer(RENDERERNAME, rendererUIName = RENDERERNAME)
#    pm.renderer(RENDERERNAME, edit = True, renderProcedure = "python(\"" + renderProc +"\")")
#    pm.renderer(RENDERERNAME, edit = True, batchRenderProcedure =  "python(\"" + batchRenderProc +"\")")
#    pm.renderer(RENDERERNAME, edit = True, commandRenderProcedure =  "python(\"" + commandRenderProc +"\")")
#    pm.renderer(RENDERERNAME, edit = True, batchRenderOptionsProcedure =  "python(\"" + batchRenderOptionsProc +"\")")
#    pm.renderer(RENDERERNAME, edit = True, batchRenderOptionsStringProcedure =  "python(\"" + batchRenderOptionsStringProc +"\")")
#    pm.renderer(RENDERERNAME, edit = True, addGlobalsNode = "defaultRenderGlobals")
#    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = ('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"))
#    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = (RENDERERNAME, "createRendererTab", "updateRendererTab"))    
#    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = (RENDERERNAME + " Translator", "createTranslatorTab", "updateTranslatorTab")) 
#    log.debug("registerRenderer done")   
#    print pm.renderer(RENDERERNAME, query = True, batchRenderProcedure =  True)
#    
#def extendMayaNodes():
#    
#    # spot light
#    pm.addExtension(nodeType="spotLight", longName="mtm_deepShadowMap", attributeType="bool", defaultValue=True)
#    pm.addExtension(nodeType="spotLight", longName="mtm_spread", attributeType="double", defaultValue=1.0)
#    pm.addExtension(nodeType="spotLight", longName="mtm_bias", attributeType="double", defaultValue=0.05)
#    pm.addExtension(nodeType="spotLight", longName="mtm_blur", attributeType="double", defaultValue=0.0)
#    pm.addExtension(nodeType="spotLight", longName="mtm_shadowQuality", attributeType="double", defaultValue=1.0)
#    pm.addExtension(nodeType="spotLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
#    pm.addExtension(nodeType="spotLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
#
#    # area light
#    pm.addExtension(nodeType="areaLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
#    pm.addExtension(nodeType="areaLight", longName="mtm_areaType", attributeType="enum", enumName="line:grid:disk:sphere")
#    pm.addExtension(nodeType="areaLight", longName="mtm_normalizeArea",  attributeType="bool", defaultValue=True)
#    pm.addExtension(nodeType="areaLight", longName="mtm_useLightGeometry", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="areaLight", longName="mtm_lightGeometry", attributeType="message")
#    pm.addExtension(nodeType="areaLight", longName="mtm_renderLightGeometry", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="areaLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
#    
#    #point light
#    pm.addExtension(nodeType="pointLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
#    pm.addExtension(nodeType="pointLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
#
#    #directional light
#    pm.addExtension(nodeType="directionalLight", longName="mtm_samplingQuality", attributeType="double", defaultValue=1.0)
#    pm.addExtension(nodeType="directionalLight", longName="mtm_activeRadius", attributeType="double", defaultValue=0.0)
#
#    pm.addExtension(nodeType="spotLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="areaLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="pointLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="directionalLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
#    pm.addExtension(nodeType="volumeLight", longName="mtm_surfaceShaders", attributeType="bool", defaultValue=False)
#    
#    pm.addExtension(nodeType="light", longName="mtm_fastShadows", attributeType="bool", defaultValue=False)
#
#    # mesh
#    pm.addExtension(nodeType="mesh", longName="mtm_renderAsSubdiv", attributeType="bool", defaultValue=False)
#    # nurbsCurve
#    pm.addExtension(nodeType="nurbsCurve", longName="primaryVisibility", attributeType="bool", defaultValue=True)
#
#    # displacement shader
#    pm.addExtension(nodeType="displacementShader", longName="mtm_displacebound", attributeType="double", defaultValue=1.0)
#    
#    # phong, blinn, phongE, lambert ...
#    pm.addExtension(nodeType="phong", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="phongE", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="blinn", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="lambert", longName="mtm_refrBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="lambert", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min = 0.0, max = 1.0)
#    pm.addExtension(nodeType="phong", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min = 0.0, max = 1.0)
#    pm.addExtension(nodeType="blinn", longName="mtm_fresnel", attributeType="double", defaultValue=0.0, min = 0.0, max = 1.0)
#    
#    pm.addExtension(nodeType="phong", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
#    pm.addExtension(nodeType="phongE", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
#    pm.addExtension(nodeType="blinn", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
#    pm.addExtension(nodeType="lambert", longName="mtm_refrSamples", attributeType="long", defaultValue=1)
#
#    pm.addExtension(nodeType="phong", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="phongE", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#    pm.addExtension(nodeType="blinn", longName="mtm_reflBlur", attributeType="double", defaultValue=0.0, min = 0.0, softMaxValue = 1.0)
#
#    pm.addExtension(nodeType="phong", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
#    pm.addExtension(nodeType="phongE", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
#    pm.addExtension(nodeType="blinn", longName="mtm_reflSamples", attributeType="long", defaultValue=1)
#    
#    # orenNayar
#    pm.addExtension(nodeType="lambert", longName="mtm_orenNayar", attributeType="long", defaultValue=0)
#    
#def unloadAndUnregister():
#    
#    if pm.pluginInfo('mayatomantra', q = True, l=True):
#        pm.unloadPlugin('mayatomantra', force = True)
#        if pm.renderer(RENDERERNAME, q = True, exists = True):
#            pm.renderer(RENDERERNAME, unregisterRenderer = True)
#            
#    pm.loadPlugin( 'mayatomantra' )
#    
#    if len(pm.ls(type = GLOBALSNAME)) == 0:
#        pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
#    else:
#        log.info("mayaToMantraGlobals node exists, using old one")
#
#def unregister():
#    if pm.renderer(RENDERERNAME, q = True, exists = True):
#        pm.renderer(RENDERERNAME, unregisterRenderer = True)
#        
#def initRenderer():
#    """call this function during after initialisation of the mantra render plugin.
#    From within the plugin.
#    """
#    log.debug("Init Mantra Renderer script.")
#    #if prepareEnv() == False:
#    #    pm.displayError("Env Variables not found. Unable to continue.")
#    #    return
#    #unloadAndUnregister()
#    log.debug("Call mel globals")
#    melGlobals()
#    log.debug("Call registerRenderer")
#    registerRenderer()
#    
#    if not pm.about(batch=True):
#        mantraMainMenu()
#        
#    log.debug("Call extendmayanodes")        
#    extendMayaNodes()
#    
## 192.168.199.136
