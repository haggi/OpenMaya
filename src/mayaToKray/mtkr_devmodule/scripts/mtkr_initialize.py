import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures

reload(Renderer)

log = logging.getLogger("mtkrLogger")

RENDERER_NAME = "Kray"

class KrayRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg=None):
        if not KrayRenderer.theRendererInstance:
            KrayRenderer.theRendererInstance = KrayRenderer(RENDERER_NAME , __name__)
        return KrayRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i, v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy=None):
        print "UpdateTest", dummy             

    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Photons"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("FinalGathering"))    
        #pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Sampling"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Quality"))    

    def KrayPhotonsCreateTab(self):
        log.debug("KrayPhotonsCreateTab()")
        self.createGlobalsNode()
        photonsDict = {}
        self.rendererTabUiDict['photons'] = photonsDict

        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "PhotonsScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Photons frame", collapsable=True, collapse=False):
                    ui = pm.floatFieldGrp(label="Threshold:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgThreshold", index=2)                     
                    ui = pm.intFieldGrp(label="Min Rays:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgMinRays", index=2) 
                    ui = pm.intFieldGrp(label="Max Rays:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgMaxRays", index=2) 
                    ui = pm.floatFieldGrp(label="Prerender:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgPrerender", index=2)                     
                    ui = pm.intFieldGrp(label="Passes:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgPasses", index=2) 
                    ui = pm.floatFieldGrp(label="Sploth Detect:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgSplotchDetect", index=2)                     
                    ui = pm.floatFieldGrp(label="Sensitivity:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgSensitivity", index=2)  
                    ui = pm.checkBoxGrp(label="FG Reflections:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgReflections", index=2) 
                    ui = pm.checkBoxGrp(label="FG Refractions:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgRefractions", index=2)                   
                    ui = pm.floatFieldGrp(label="Spatial Tolerance:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgSpatialTolerance", index=2)  
                    ui = pm.floatFieldGrp(label="Angular Tolerance:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgAngularTolerance", index=2)  
                    ui = pm.floatFieldGrp(label="FG Min Dist:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgDistMin", index=2)  
                    ui = pm.floatFieldGrp(label="FG Dist Max:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgDistMax", index=2)  
                    ui = pm.floatFieldGrp(label="Density/Brightness:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgBrightness", index=2)  
                    ui = pm.intFieldGrp(label="Path Passes:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgPathPasses", index=2)  
                    ui = pm.floatFieldGrp(label="Corner Dist:", numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgCornerDist", index=2)  
                    ui = pm.checkBoxGrp(label="Show Samples:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".fgShowSamples", index=2) 
                    

        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
                    
    def KrayPhotonsUpdateTab(self):
        log.debug("KrayPhotonsUpdateTab()")

    def KrayFinalGatheringCreateTab(self):
        log.debug("KrayFinalGatheringCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "PhotonsScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Photons frame", collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Dummy:", value1=False)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
                    
    def KrayFinalGatheringUpdateTab(self):
        log.debug("KrayFinalGatheringUpdateTab()")

#    def KraySamplingCreateTab(self):
#        log.debug("KraySamplingCreateTab()")
#        self.createGlobalsNode()
#        parentForm = pm.setParent(query = True)
#        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
#        scLo = self.rendererName + "PhotonsScrollLayout"
#        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
#            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
#                with pm.frameLayout(label="Photons frame", collapsable = True, collapse=False):
#                    ui = pm.checkBoxGrp(label="Dummy:", value1 = False)
#        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
#        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
#                    
#    def KraySamplingUpdateTab(self):
#        log.debug("KraySamplingUpdateTab()")
            

    def KrayQualityCreateTab(self):
        log.debug("KrayFinalGatheringCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "PhotonsScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Photons frame", collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Dummy:", value1=False)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
                    
    def KrayQualityUpdateTab(self):
        log.debug("KrayQualityUpdateTab()")
            
            
    def KrayRendererCreateTab(self):
        log.debug("KrayRendererCreateTab()")
        self.createGlobalsNode()
        sDict = {}
        self.rendererTabUiDict['sampling'] = sDict        
        parentForm = pm.setParent(query=True)

        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Image format", collapsable=True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".imageFormat")
                    ui = pm.attrEnumOptionMenuGrp(label="Image format", at=self.renderGlobalsNodeName + ".imageFormat", ei=self.getEnumList(attr)) 
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".bitdepth")
                    sDict['bitdepth'] = pm.attrEnumOptionMenuGrp(label="Bit depth", at=self.renderGlobalsNodeName + ".bitdepth", ei=self.getEnumList(attr)) 
                    sDict['jpgQuality'] = pm.intFieldGrp(label="Jpg Quality:", numberOfFields=1)
                    pm.connectControl(sDict['jpgQuality'], self.renderGlobalsNodeName + ".jpgQuality", index=2)                     
                
                with pm.frameLayout(label="Sampling", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayoutA", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".samplingType")
                        ui = pm.attrEnumOptionMenuGrp(label="Sampling Type", at=self.renderGlobalsNodeName + ".samplingType", ei=self.getEnumList(attr)) 
                        sDict['fullScreenAA'] = pm.checkBoxGrp(label="Full Screen AA:", value1=False)
                        pm.connectControl(sDict['fullScreenAA'], self.renderGlobalsNodeName + ".fullScreenAA", index=2)
                        pm.separator()
                        sDict['gridSize'] = pm.intFieldGrp(label="Grid Size:", numberOfFields=1)
                        pm.connectControl(sDict['gridSize'], self.renderGlobalsNodeName + ".gridSize", index=2)                     
                        sDict['gridRotate'] = pm.checkBoxGrp(label="Grid Rotate:", value1=False)
                        pm.connectControl(sDict['gridRotate'], self.renderGlobalsNodeName + ".rotateGrid", index=2) 
                        pm.separator()
                        ui = pm.intFieldGrp(label="Rays:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_rays", index=2)                     
                        ui = pm.intFieldGrp(label="Min Rays:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_minRays", index=2)                     
                        ui = pm.intFieldGrp(label="Max Rays:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_maxRays", index=2)                     
                        ui = pm.intFieldGrp(label="Edge Thickness:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_thickness", index=2)                     
                        ui = pm.intFieldGrp(label="Upsample:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_upsample", index=2)      
                        ui = pm.floatFieldGrp(label="Edge Absulut:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_edgeAbsolute", index=2)                     
                        ui = pm.floatFieldGrp(label="Edge Relative:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_relative", index=2)                     
                        ui = pm.floatFieldGrp(label="Normal Sentitivity:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_normal", index=2)                     
                        ui = pm.floatFieldGrp(label="Z Sensitivity:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_z", index=2)                  
                        ui = pm.floatFieldGrp(label="Max Brightness:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_overburn", index=2)                    
                        ui = pm.floatFieldGrp(label="Threshold:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".aa_threshold", index=2)                     
                        ui = pm.floatFieldGrp(label="Mb Subframes:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".mb_subframes", index=2)                     
                        pm.separator()
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".pixelOrder")
                        ui = pm.attrEnumOptionMenuGrp(label="Pixel Order", at=self.renderGlobalsNodeName + ".pixelOrder", ei=self.getEnumList(attr)) 
                    
                with pm.frameLayout(label="Filtering", collapsable=True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".filtertype")
                    ui = pm.attrEnumOptionMenuGrp(label="Filter Type", at=self.renderGlobalsNodeName + ".filtertype", ei=self.getEnumList(attr)) 
                    sDict['filterSize']  = pm.floatFieldGrp(label="Filter Size:", numberOfFields=1)
                    pm.connectControl(sDict['filterSize'], self.renderGlobalsNodeName + ".filtersize", index=2)
                                        
                with pm.frameLayout(label="Features", collapsable=True, collapse=False):
                    sDict['doMb'] = pm.checkBoxGrp(label="Motion Blur:")
                    pm.connectControl(sDict['doMb'], self.renderGlobalsNodeName + ".doMb", index=2)
                    sDict['doDof'] = pm.checkBoxGrp(label="Depth Of Field:")
                    pm.connectControl(sDict['doDof'], self.renderGlobalsNodeName + ".doDof", index=2)
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.KrayRendererUpdateTab()
        
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.samplingType, pm.Callback(self.KrayRendererUIUpdateCallback, "sampling")] )
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.filtertype, pm.Callback(self.KrayRendererUIUpdateCallback, "sampling")] )
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.imageFormat, pm.Callback(self.KrayRendererUIUpdateCallback, "sampling")] )

    def KrayRendererUIUpdateCallback(self, what=None):
        self.createGlobalsNode()
        #self.updateEnvironment()
        log.debug("KrayRendererUIUpdateCallback(): " + str(what))
        if what=="sampling":
            print "Update sampling"
            sDict = self.rendererTabUiDict['sampling']
            sType = self.renderGlobalsNode.samplingType.get()
            
            if sType != 1: #grid
                pm.intFieldGrp(sDict['gridSize'], edit=True, enable=False)
                pm.checkBoxGrp(sDict['gridRotate'], edit=True, enable=False)
            else:
                pm.intFieldGrp(sDict['gridSize'], edit=True, enable=True)
                pm.checkBoxGrp(sDict['gridRotate'], edit=True, enable=True)
                
            fType = self.renderGlobalsNode.filtertype.get()
            if fType in [4,5,6]:
                pm.floatFieldGrp(sDict['filterSize'], edit=True, enable=False)
            else:
                pm.floatFieldGrp(sDict['filterSize'], edit=True, enable=True)
                
            iFormat = self.renderGlobalsNode.imageFormat.get()    
            if iFormat in [2,3]:
                pm.attrEnumOptionMenuGrp(sDict['bitdepth'], edit=True, enable=True) 
            else:
                pm.attrEnumOptionMenuGrp(sDict['bitdepth'], edit=True, enable=False) 
                
            if iFormat == 1:
                pm.intFieldGrp(sDict['jpgQuality'], edit=True, enable=True)
            else:
                pm.intFieldGrp(sDict['jpgQuality'], edit=True, enable=False)

    def KrayRendererUpdateTab(self, dummy=None):
        self.createGlobalsNode()
        #self.updateEnvironment()
        log.debug("KrayRendererUpdateTab()")
        
        sDict = self.rendererTabUiDict['sampling']
        sType = self.renderGlobalsNode.samplingType.get()
        
        if sType != 1: #grid
            pm.intFieldGrp(sDict['gridSize'], edit=True, enable=False)
            pm.checkBoxGrp(sDict['gridRotate'], edit=True, enable=False)
        else:
            pm.intFieldGrp(sDict['gridSize'], edit=True, enable=True)
            pm.checkBoxGrp(sDict['gridRotate'], edit=True, enable=True)

        fType = self.renderGlobalsNode.filtertype.get()
        if fType in [4,5,6]:
            pm.floatFieldGrp(sDict['filterSize'], edit=True, enable=False)
        else:
            pm.floatFieldGrp(sDict['filterSize'], edit=True, enable=True)
            
        iFormat = self.renderGlobalsNode.imageFormat.get()    
        if iFormat in [2,3]:
            pm.attrEnumOptionMenuGrp(sDict['bitdepth'], edit=True, enable=True) 
        else:
            pm.attrEnumOptionMenuGrp(sDict['bitdepth'], edit=True, enable=False) 
            
        if iFormat == 1:
            pm.intFieldGrp(sDict['jpgQuality'], edit=True, enable=True)
        else:
            pm.intFieldGrp(sDict['jpgQuality'], edit=True, enable=False)
            
    def xmlFileBrowse(self, args=None):
        filename = pm.fileDialog2(fileMode=0, caption="Kray Export File Name")
        if len(filename) > 0:
            print "Got filename", filename
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename[0])
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])
        
    def KrayTranslatorCreateTab(self):
        log.debug("KrayTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Translator", collapsable=True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label="Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei=self.getEnumList(attr)) 
                with pm.frameLayout(label="Kray Scene export", collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export Scene File:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index=2)
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width=120):
                        pm.text(label="SceneFileName:", width=60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".kray"
                        xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath, width=60)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index=2)
                with pm.frameLayout(label="Optimize Textures", collapsable=True, collapse=False):
                    with pm.rowColumnLayout(nc=3, width=120):
                        optiDict = {}
#                        pm.text(label="OptimizedTex Dir:", width = 60, align="right")
#                        self.rendererTabUiDict['opti'] = optiDict
#                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
#                        optiDict['optiField'] = pm.textField(text = self.renderGlobalsNode.optimizedTexturePath.get(), width = 60)
#                        pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index = 2 )
                with pm.frameLayout(label="Additional Settings", collapsable=True, collapse=False):
                    ui = pm.floatFieldGrp(label="Scene scale:", value1=1.0, numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index=2)
                         
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def KrayTranslatorUpdateTab(self):
        log.debug("KrayTranslatorUpdateTab()")

    def createImageFormats(self):
        self.imageFormats.append('exr')
        self.imageFormats.append('png')

    def registerNodeExtensions(self):
        """Register Kray specific node extensions. e.g. camera type, diaphram_blades and others
        """
        pass
    
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
                
    def renderProcedure(self, width, height, doShadows, doGlow, camera, options):
        log.debug("renderProcedure")
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        
        if pm.about(batch=True):
            pm.mayatokray()
        else:
            pm.mayatokray(width=width, height=height, camera=camera)
            
        self.postRenderProcedure()
                
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        print "startIprRenderProcedure", editor, resolutionX, resolutionY, camera
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        pm.mayatokray(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatokray(stopIpr=True)
        self.postRenderProcedure()
            
    def preRenderProcedure(self):
        self.createGlobalsNode()
        if self.renderGlobalsNode.threads.get() == 0:
            #TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        
#        if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
#            optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
#            if not os.path.exists(optimizedPath):
#                optimizedPath.makedirs()
#            self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
#
#        #craete optimized exr textures
#        optimizeTextures.preRenderOptimizeTextures(optimizedFilePath = self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        pass
#        optimizeTextures.postRenderOptimizeTextures()


def theRenderer():
    return KrayRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Kray")
        theRenderer().registerRenderer()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Kray FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

def uiCallback(what=None):
    theRenderer().KrayRendererUIUpdateCallback(what)
