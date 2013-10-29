import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import Fuji.aeNodeTemplates as aet
import path

reload(Renderer)

log = logging.getLogger("mtFujiLogger")


RENDERER_NAME="Fuji"

class FujiRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg = None):
        if not FujiRenderer.theRendererInstance:
            FujiRenderer.theRendererInstance = FujiRenderer(RENDERER_NAME , __name__)
            #FujiRenderer.theRendererInstance = FujiRenderer(RENDERER_NAME , "mtfu_initialize")
        return FujiRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             
            
    def FujiRendererCreateTab(self):
        log.debug("FujiRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"

        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')
        
        parentForm = pm.setParent(query = True)
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Sampling", collapsable = True, collapse=False):                
                    self.addRenderGlobalsUIElement(attName = 'sampleJitter', uiType = 'float', displayName = 'SampleJitter', default='1.0', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'sample_time_range_min', uiType = 'float', displayName = 'Sample Time Range Min', default='0.0', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'sample_time_range_max', uiType = 'float', displayName = 'Sample Time Range Max', default='1.0', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'samplesX', uiType = 'int', displayName = 'Pixel Samples x', default=3, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'samplesY', uiType = 'int', displayName = 'Pixel Samples y', default=3, uiDict=uiDict)
                    pm.separator()
                    self.addRenderGlobalsUIElement(attName = 'cast_shadow', uiType = 'bool', displayName = 'Cast Shadows', default='true', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'max_reflect_depth', uiType = 'int', displayName = 'Max Reflection Depth', default='3', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'max_refract_depth', uiType = 'int', displayName = 'Max Refraction Depth', default='3', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'raymarch_step', uiType = 'float', displayName = 'Raymarching Stepsize', default='0.05', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'raymarch_shadow_step', uiType = 'float', displayName = 'Raymarching Shadow Stepsize', default='0.1', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'raymarch_reflect_ste', uiType = 'float', displayName = 'Raymarching Reflection Stepsize', default='0.1', uiDict=uiDict)                    
                    self.addRenderGlobalsUIElement(attName = 'raymarch_refract_ste', uiType = 'float', displayName = 'Raymarching Refraction Stepsize', default='0.1', uiDict=uiDict)                                        
                    pm.separator()
                    self.addRenderGlobalsUIElement(attName = 'doMotionBlur', uiType = 'bool', displayName = 'Motionblur:', default=False, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'doDof', uiType = 'bool', displayName = 'Depth of Field:', default=False, uiDict=uiDict)
                    
                with pm.frameLayout(label="Output", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".imageFormat")
                    ui = pm.attrEnumOptionMenuGrp(label = "Image Format", at=self.renderGlobalsNodeName + ".imageFormat", ei = self.getEnumList(attr)) 
                    
                with pm.frameLayout(label="Filtering", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".filtertype")
                    ui = pm.attrEnumOptionMenuGrp(label = "Filter Type", at=self.renderGlobalsNodeName + ".filtertype", ei = self.getEnumList(attr)) 
                    ui = pm.intFieldGrp(label="Filter Size:", numberOfFields = 1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".filtersize", index = 2 )

                with pm.frameLayout(label="Renderer", collapsable = True, collapse=False):
                    self.addRenderGlobalsUIElement(attName = 'use_max_thread', uiType = 'bool', displayName = 'Use Max Threads', default='true', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Threads', default=8, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'tilesize', uiType = 'int', displayName = 'Tile Size', default=64, uiDict=uiDict)
                    ui = pm.intFieldGrp(label="Verbosity:", numberOfFields = 1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".rendererVerbosity", index = 2 )
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.FujiRendererUpdateTab()

    def FujiRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        log.debug("FujiRendererUpdateTab()")
        uiDict = self.rendererTabUiDict['common']
        
        uiDict['threads'].setEnable(False)
        if not self.renderGlobalsNode.use_max_thread.get():
            uiDict['threads'].setEnable(True)
            
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
        
    def FujiTranslatorCreateTab(self):
        log.debug("FujiTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
#                with pm.frameLayout(label="Fuji XML export", collapsable = True, collapse=False):
#                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
#                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
#                    xmlDict = {}
#                    self.rendererTabUiDict['xml'] = xmlDict
#                    with pm.rowColumnLayout(nc=3, width = 120):
#                        pm.text(label="XMLFileName:", width = 60, align="right")
#                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Fuji"
#                        xmlDict['xmlFile'] = pm.textField(text = defaultXMLPath, width = 60)
#                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
#                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportXMLFileName", index = 2 )
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

    def FujiTranslatorUpdateTab(self):
        log.debug("FujiTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register Fuji specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtfu_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="someFancyCameraAttribute", attributeType="long", defaultValue = 0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="someFancyMeshAttribute", attributeType="bool", defaultValue = False)

        # lights
        pm.addExtension(nodeType="areaLight", longName="sample_count", attributeType="long", defaultValue = 8)
        pm.addExtension(nodeType="areaLight", longName="double_sided", attributeType="bool", defaultValue = False)
        
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
            pm.mayatoFuji()
        else:
            pm.mayatoFuji(width=width, height=height, camera=camera)
            
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
        pm.mayatoFuji(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoFuji(stopIpr=True)
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
        print aet.__name__, aet.__file__
        aet.AEFujiNodeTemplate(nodeName)
        
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
    rendererName = "Fuji"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

def theRenderer():
    return FujiRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Fuji")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Fuji FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

