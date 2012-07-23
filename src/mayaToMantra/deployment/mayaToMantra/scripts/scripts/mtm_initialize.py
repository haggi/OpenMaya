import pymel.core as pm 
import pymel.core.uitypes as pui
from pymel.core import Callback
import maya.cmds as cmds
import logging
import os
import getpass
import subprocess
import mtm_mantraAttributes as mantraAttributes
import mtm_attributeManager
import shutil
import sys
#import AETemplates.AEdagNodeTemplate as dgTmpl

reload(mantraAttributes)
log = logging.getLogger("mantraLogger")

RENDERERNAME = "Mantra"
GLOBALSNAME = "mayaToMantraGlobals"

def getHoudiniPath():
    return "C:/Program Files/Side Effects Software/Houdini 11.0.446.7"

def getInstallDir():
    return "C:/Users/haggi/coding/mayaTo/VS/" 

def prepareEnv():    
    username = getpass.getuser()
    
    if not os.environ.has_key('H'):
        os.environ['H'] = getHoudiniPath()
    tmpdir = ""
    if not os.environ.has_key('TMP'):
        if not os.environ.has_key('tmp'):
            # TODO: correct tmpdir
            pass
        else:
            tmpdir =  os.environ['tmp']
    else:
        tmpdir =  os.environ['TMP']
    basePath = os.environ['H']
    currentDir = pm.workspace.path
    sn = pm.sceneName().basename().split(".")[0]
    os.environ['HB'] = basePath + "/bin" 
    os.environ['HD'] = basePath + "/demo" 
    os.environ['HFS'] = basePath
    os.environ['HH'] = basePath + "/houdini" 
    os.environ['HHC'] = basePath + "/houdini/config" 
    os.environ['HIP'] = currentDir + "/mantra/" + sn
    os.environ['HIPNAME'] = "untitled.hip" 
    #os.environ['HOUDINI_DESKTOP_DIR'] = "C:/Users/haggi/Desktop" 
    os.environ['HOUDINI_TEMP_DIR'] = tmpdir + "/houdini" 
    os.environ['HSITE'] = basePath + "/site" 
    os.environ['HT'] = basePath + "/toolkit" 
    os.environ['HTB'] = basePath + "/toolkit/bin" 
    #os.environ['HOUDINI_OTL_PATH'] = os.environ['HOUDINI_OTL_PATH'] + ";" + currentDir + "/mantra/" + sn + "/shaders"
    os.environ['HOUDINI_VEX_PATH'] = currentDir + "/mantra/" + sn + "/shaders" + ";&;"
    path = os.environ['PATH']
    if not (basePath + "/bin") in path:
        os.environ['PATH'] = basePath + "/bin;" + path 
    
    if not os.environ.has_key("MTM_HOME"):
        os.environ["MTM_HOME"] = getInstallDir()    
    #print "VexPath", os.environ['HOUDINI_VEX_PATH']
        
def startRenderProc(ifdFile):
    log.info("Starting render procedure")
    prepareEnv()
    IDLE_PRIORITY_CLASS = 64
    verbosity = 4
    try:
        renderGlobals = ls(type = "mayaToMantraGlobals")
        verbosity = renderGlobals.verbosity.get()
    except:
        pass
    cmd = "mantra -V " + str(verbosity) + " -f " + ifdFile
    log.info("Starting cmd %s" % cmd)
    process = subprocess.Popen( cmd, bufsize = 1, shell = True, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)

    while 1:
        line = process.stdout.readline()
        if not line: break
        
        # log.info(line.strip())
        pm.mel.trace(line.strip())
        if "Render Time:" in line:
            break
    log.info("Rendering done.")

def startShaderCompiler( shaderLib, shaderFile):
    log.info("Starting vcc compiler procedure")
    prepareEnv()
    IDLE_PRIORITY_CLASS = 64
    includePath = os.environ["MTM_HOME"] + "/shaderIncludes"
    cwd = os.getcwd()

    # go into shader directory because the -x option is needed to create .vex files and they are
    # placed into the current directory
    os.chdir(os.path.dirname(shaderFile))
    cmd = "vcc -I " + includePath + " -x -m " + shaderLib + " " + shaderFile
    log.info("Starting vcc cmd: %s" % cmd)
    process = subprocess.Popen( cmd, bufsize = 1, shell = True, stdout = subprocess.PIPE, stderr = subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
    
    while 1:
        line = process.stdout.readline()
        if not line: break
        
        # log.info(line.strip())
        pm.mel.trace(line.strip())
    log.info("Compiling done.")
    os.chdir(cwd)
    

def attributeManager():
    reload(mtm_attributeManager)
    #mtm_attributeManager.attributeManager()
    mtm_attributeManager.GUI()

def menuCallback( arg = None):
    log.debug("Mantra main menu callback: %s" % arg)
    if arg == "AttributeManager":
        attributeManager()
        
def mantraMainMenu():
    log.debug("Creating mantra main menu")
    menuName = "Mantra"
    if pm.menu(menuName, query=True, exists=True):
        pm.deleteUI(menuName)
    gMainWindow = pm.mel.eval('$tmpVar=$gMainWindow')
    mantraMenu = pm.menu(menuName, label = menuName, parent = gMainWindow, tearOff = True )
    pm.menuItem( label = 'AttributeManager', command = Callback(menuCallback, "AttributeManager") )
    pm.menuItem( label = 'DummyMenu' , command = Callback(menuCallback, "Dummy") )
    pm.menuItem( label = '', divider = True )
    pm.menuItem( label = 'DummyMenuA' , command = Callback(menuCallback, "DummyA") )
    pm.setParent("..", menu = True)
    
def renderProcedure():
    log.debug("RenderProcedure")
    
    pm.mel.trace("================ Start MayaToMantra Rendering ======================")
    # TODO: get directorys and filenames
    currentTime = pm.currentTime(query = True)
    outputPath = pm.workspace.path + "/mantra/" + pm.sceneName().basename().split(".")[0]
    imagePath = pm.workspace.path + "/images"
    imageName = pm.sceneName().basename().split(".")[0]
    try:
        mantraGlobals = pm.ls(type="mayaToMantraGlobals")[0]
    except:
        log.error("mayaToMantraGlobals not found")
        return 
    
    mantraGlobals.basePath.set(outputPath)
    mantraGlobals.imagePath.set(imagePath)
    mantraGlobals.imageName.set(imageName)
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
    
    mantraGlobals.basePath.set("")
    mantraGlobals.imagePath.set("")
    mantraGlobals.imageName.set("")
    pm.currentTime(currentTime)
    
def renderOptionsProcedure():
    log.debug("RenderOptionsProcedure")

def commandRenderProcedure():
    log.debug("commandRenderProcedure")
    renderProcedure()
 #   reload(mantraRender)
 #   mar = mantraRender.mantraRenderer()
 #   mar.batchRendering = True
 #   mar.render()

def batchRenderProcedure():
    log.debug("batchRenderProcedure")
    renderProcedure()
    
def batchRenderOptionsProcedure():
    log.debug("batchRenderOptionsProcedure")

def batchRenderOptionsStringProcedure():
    log.debug("batchRenderOptionsStringProcedure")

def cancelBatchRenderProcedure():
    log.debug("cancelBatchRenderProcedure")

def showBatchRenderProcedure():
    log.debug("showBatchRenderProcedure")

def showRenderLogProcedure():
    log.debug("showRenderLogProcedure")

def showBatchRenderLogProcedure():
    log.debug("showBatchRenderLogProcedure")

def renderRegionProcedure():
    log.debug("renderRegionProcedure")

def textureBakingProcedure():
    log.debug("textureBakingProcedure")

def renderingEditorsSubMenuProcedure():
    log.debug("renderingEditorsSubMenuProcedure")

def renderMenuProcedure():
    log.debug("renderMenuProcedure")

def removeRenderer():
    pm.renderer(RENDERERNAME,  edit = True, unregisterRenderer = True)
    menuName = RENDERERNAME
    if pm.menu(menuName, query=True, exists=True):
        pm.deleteUI(menuName)

def dimConnections(element):
    print "DimConnections"
    if element['name'] in ['motionblur']:
        value = pm.checkBoxGrp( element['name'], query = True, v1 = True)
        print element['uielement']
        valueInv = value
        #groups = mantraGlobalsAttributes.mantraGlobalsAttributes
        #if el['name'] in ['xftimesamples', 'geotimesamples', 'motionfactor', 'mbtype']:
        if value:
            print "enabling ctrls"            
        else:
            print "disabling ctrls"
        pm.intFieldGrp('xftimesamples', edit = True, enable = valueInv)
        pm.intFieldGrp('geotimesamples', edit = True, enable = valueInv)
        pm.floatFieldGrp('motionfactor', edit = True, enable = valueInv)
        pm.attrEnumOptionMenu('mbtype', edit = True, enable = valueInv)
        pm.checkBoxGrp('imagemotionblur',  edit = True, enable = valueInv)

def MantraTabCreateFromAtt():
    log.debug("MantraTabCreate from Attribute List")
    reload(mantraAttributes)

    if len(pm.ls(type = GLOBALSNAME)) == 0:
        mrg = pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
    else:
        mrg = pm.ls(type = GLOBALSNAME)[0]
        
    parentForm = pm.setParent(query = True)
    pm.setUITemplate( "attributeEditorTemplate", pushTemplate = True)
    pm.scrollLayout( "MantraScrollLayout", horizontalScrollBarThickness = 0)
    pm.columnLayout("MantraColumnLayout", adjustableColumn = True)

    mantraAttributes.mantraGlobalsATList.createUi(mrg)
    
    pm.setUITemplate( "attributeEditorTemplate", popTemplate = True)
    pm.formLayout(parentForm, edit = True, attachForm = [ ("MantraScrollLayout", "top", 0),
                                                          ("MantraScrollLayout", "bottom", 0),
                                                          ("MantraScrollLayout", "left", 0),
                                                          ("MantraScrollLayout", "right", 0)
                                                        ])


def MantraTabUpdate():
    log.debug("MantraTabUpdate")

def MantraTranslatorTabCreate():
    log.debug("MantraTabCreate from Attribute List")
    reload(mantraAttributes)

    if len(pm.ls(type = GLOBALSNAME)) == 0:
        mrg = pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
    else:
        mrg = pm.ls(type = GLOBALSNAME)[0]
        
    parentForm = pm.setParent(query = True)
    pm.setUITemplate( "attributeEditorTemplate", pushTemplate = True)
    pm.scrollLayout( "MantraTrScrollLayout", horizontalScrollBarThickness = 0)
    pm.columnLayout("MantraTrColumnLayout", adjustableColumn = True)

    #mantraAttributes.mantraGlobalsATList.createUi(mrg)
    
    pm.setUITemplate( "attributeEditorTemplate", popTemplate = True)
    pm.formLayout(parentForm, edit = True, attachForm = [ ("MantraTrScrollLayout", "top", 0),
                                                          ("MantraTrScrollLayout", "bottom", 0),
                                                          ("MantraTrScrollLayout", "left", 0),
                                                          ("MantraTrScrollLayout", "right", 0)
                                                        ])


def MantraTranslatorTabUpdate():
    log.debug("MantraTranslatorTabUpdate")

def melGlobals():
    createMantraTab = "global proc createRendererTab()\
    {\
     print(\"createMelTab\");\n\
     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTabCreateFromAtt()\");\n\
    };"
    updateMantraTab = "global proc updateRendererTab()\
    {\
     print(\"updateMelTab\");\n\
     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTabUpdate()\");\n\
    };"
    pm.mel.eval(createMantraTab)
    pm.mel.eval(updateMantraTab)

    createMantraTab = "global proc createTranslatorTab()\
    {\
     print(\"createMelTab\");\n\
     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTranslatorTabCreate()\");\n\
    };"
    updateMantraTab = "global proc updateTranslatorTab()\
    {\
     print(\"updateMelTab\");\n\
     python(\"import mtm_initialize as tr;reload(tr);tr.MantraTranslatorTabUpdate()\");\n\
    };"
    pm.mel.eval(createMantraTab)
    pm.mel.eval(updateMantraTab)

def registerRenderer():
    log.debug("registerRenderer")
    
    baseCmd = "import mtm_initialize as mtmi;reload(mtmi);"
    renderProc = baseCmd + "mtmi.renderProcedure()"    
    batchRenderProc = baseCmd + "mtmi.batchRenderProcedure()"    
    commandRenderProc = baseCmd + "mtmi.commandRenderProcedure()"    
    batchRenderOptionsProc = baseCmd + "mtmi.batchRenderOptionsProcedure()"    
    batchRenderOptionsStringProc = baseCmd + "mtmi.batchRenderOptionsStringProcedure()"    
    pm.renderer(RENDERERNAME, rendererUIName = RENDERERNAME)
    pm.renderer(RENDERERNAME, edit = True, renderProcedure = "python(\"" + renderProc +"\")")
    pm.renderer(RENDERERNAME, edit = True, batchRenderProcedure =  "python(\"" + batchRenderProc +"\")")
    pm.renderer(RENDERERNAME, edit = True, commandRenderProcedure =  "python(\"" + commandRenderProc +"\")")
    pm.renderer(RENDERERNAME, edit = True, batchRenderOptionsProcedure =  "python(\"" + batchRenderOptionsProc +"\")")
    pm.renderer(RENDERERNAME, edit = True, batchRenderOptionsStringProcedure =  "python(\"" + batchRenderOptionsStringProc +"\")")
    pm.renderer(RENDERERNAME, edit = True, addGlobalsNode = "defaultRenderGlobals")
    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = ('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"))
    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = (RENDERERNAME, "createRendererTab", "updateRendererTab"))    
    pm.renderer(RENDERERNAME, edit = True, addGlobalsTab = (RENDERERNAME + " Translator", "createTranslatorTab", "updateTranslatorTab"))    
        
def unloadAndUnregister():
    
    if pm.pluginInfo('mayatomantra', q = True, l=True):
        pm.unloadPlugin('mayatomantra', force = True)
        if pm.renderer(RENDERERNAME, q = True, exists = True):
            pm.renderer(RENDERERNAME, unregisterRenderer = True)
            
    pm.loadPlugin( 'mayatomantra' )
    
    if len(pm.ls(type = GLOBALSNAME)) == 0:
        pm.createNode(GLOBALSNAME, name = GLOBALSNAME)
    else:
        log.info("mayaToMantraGlobals node exists, using old one")

def unregister():
    if pm.renderer(RENDERERNAME, q = True, exists = True):
        pm.renderer(RENDERERNAME, unregisterRenderer = True)
        
def initRenderer():
    """call this function during after initialisation of the mantra render plugin.
    From within the plugin.
    """
    log.debug("Init Mantra Renderer script.")
    
    #unloadAndUnregister()
    melGlobals()
    registerRenderer()
    
    if not pm.about(batch=True):
        mantraMainMenu()
        
    #log.debug("Adding aetemplates to path.")
    #sys.path.append(os.environ["MTM_HOME"]+"/scripts/AETemplates") 

#192.168.64.133
