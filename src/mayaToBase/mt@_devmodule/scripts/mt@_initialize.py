import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import @.aeNodeTemplates as aet
import path

reload(Renderer)

log = logging.getLogger("mt@Logger")


RENDERER_NAME="@"

class @Renderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg = None):
        if not @Renderer.theRendererInstance:
            @Renderer.theRendererInstance = @Renderer(RENDERER_NAME , __name__)
            #@Renderer.theRendererInstance = @Renderer(RENDERER_NAME , "mt@_initialize")
        return @Renderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             

    def updateEnvironment(self, dummy=None):
        envDict = self.rendererTabUiDict['environment']
        envType = self.renderGlobalsNode.environmentType.get()
        #Constant
            
    def @RendererCreateTab(self):
        log.debug("@RendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')        
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Sampling", collapsable = True, collapse=False):
                    self.addRenderGlobalsUIElement(attName = 'adaptiveSampling', uiType = 'bool', displayName = 'Adaptive Sampling:', default='True', uiDict=uiDict)

                    
                with pm.frameLayout(label="Renderer", collapsable = True, collapse=False):
                    self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Threads', default=8, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'tilesize', uiType = 'int', displayName = 'Tile Size', default=64, uiDict=uiDict)
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.@RendererUpdateTab()

    def @RendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        self.updateEnvironment()
        log.debug("@RendererUpdateTab()")
        if self.renderGlobalsNode.adaptiveSampling.get():
            self.rendererTabUiDict['minSamples'].setEnable(True)
            self.rendererTabUiDict['maxError'].setEnable(True)
        else:
            self.rendererTabUiDict['minSamples'].setEnable(False)
            self.rendererTabUiDict['maxError'].setEnable(False)

    def xmlFileBrowse(self, args=None):
        print "xmlfile", args
        filename = pm.fileDialog2(fileMode=0, caption="XML Export File Name")
        if len(filename) > 0:
            print "Got filename", filename
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename[0])
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])
        
    def @TranslatorCreateTab(self):
        log.debug("@TranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
                with pm.frameLayout(label="@ XML export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="XMLFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".@"
                        xmlDict['xmlFile'] = pm.textField(text = defaultXMLPath, width = 60)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportXMLFileName", index = 2 )
                with pm.frameLayout(label="Optimize Textures", collapsable = True, collapse=False):
                    with pm.rowColumnLayout(nc=3, width = 120):
                        optiDict = {}
                        pm.text(label="OptimizedTex Dir:", width = 60, align="right")
                        self.rendererTabUiDict['opti'] = optiDict
                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
                        optiDict['optiField'] = pm.textField(text = self.renderGlobalsNode.optimizedTexturePath.get(), width = 60)
                        pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index = 2 )
                with pm.frameLayout(label="Additional Settings", collapsable = True, collapse=False):
                    ui = pm.floatFieldGrp(label="Scene scale:", value1 = 1.0, numberOfFields = 1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index = 2 )
                         
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def @TranslatorUpdateTab(self):
        log.debug("@TranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register @ specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mt@_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mt@_diaphragm_blades", attributeType="long", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mt@_diaphragm_tilt_angle", attributeType="float", defaultValue = 0.0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mt@_mesh_useassembly", attributeType="bool", defaultValue = False)

        # 
        
    def setImageName(self):
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
    
    def removeLogFile(self):
        logfile = pm.workspace.path + "/applelog.log"
        try:
            if os.path.exists(logfile):
                os.remove(logfile)
        except:
            pass

    def showLogFile(self):
        logfile = pm.workspace.path + "/applelog.log"
        if os.path.exists(logfile):
            lh = open(logfile, 'r')
            rl = lh.readlines()
            for l in rl:
                sys.__stdout__.write(l)
            
    def renderProcedure(self, width, height, doShadows, doGlow, camera, options):
        log.debug("renderProcedure")
        self.removeLogFile()
        print "renderProcedure", width, height, doShadows, doGlow, camera, options
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        
        if pm.about(batch=True):
            pm.mayato@()
        else:
            pm.mayato@(width=width, height=height, camera=camera)
            
        if not self.ipr_isrunning:
            self.showLogFile()
        self.postRenderProcedure()
            
        
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        print "startIprRenderProcedure", editor, resolutionX, resolutionY, camera
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        pm.mayato@(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayato@(stopIpr=True)
        self.postRenderProcedure()
            
    def preRenderProcedure(self):
        self.createGlobalsNode()
        if self.renderGlobalsNode.threads.get() == 0:
            #TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
            optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
            if not os.path.exists(optimizedPath):
                optimizedPath.makedirs()
            self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))

        #craete optimized exr textures
        optimizeTextures.preRenderOptimizeTextures(optimizedFilePath = self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AE@NodeTemplate(nodeName)
        
    def afterGlobalsNodeReplacement(self):
        log.debug("afterGlobalsNodeReplacement")        
        self.rendererTabUiDict = {}        

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    rendererName = "@"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

def theRenderer():
    return @Renderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer @")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer @ FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

