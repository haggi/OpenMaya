import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import path

reload(Renderer)

log = logging.getLogger("mtLuxLogger")


RENDERER_NAME="Lux"

class LuxRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg = None):
        if not LuxRenderer.theRendererInstance:
            LuxRenderer.theRendererInstance = LuxRenderer(RENDERER_NAME , __name__)
            #LuxRenderer.theRendererInstance = LuxRenderer(RENDERER_NAME , "mtlu_initialize")
        return LuxRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             

    def updateEnvironment(self, dummy=None):
        pass
        #envDict = self.rendererTabUiDict['environment']
            
    def LuxRendererCreateTab(self):
        log.debug("LuxRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label='Renderer', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'renderer', uiType = 'enum', displayName = 'Renderer', default='0', data='sampler:hybridsampler:hybridsppm', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsConfigFile', uiType = 'string', displayName = 'HSConfig', default='""', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclPlatformIndex', uiType = 'int', displayName = 'OCL Platform Index', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclGpuUse', uiType = 'bool', displayName = 'OCL Gpu Use', default='1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclWGroupSize', uiType = 'int', displayName = 'OCL WorkgroupSize', default='0', uiDict=uiDict)
                
                with pm.frameLayout(label='Sampler', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'sampler', uiType = 'enum', displayName = 'Sampler', default='0', data='random:low discrepancy:metropolis:rrpt', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pixelSampler', uiType = 'enum', displayName = 'Pixel Sampler', default='2', data='hilbert:linear:vegas:lowdiscrepancy:tile:random', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'numSamples', uiType = 'int', displayName = 'Samples', default='4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxconsecrejects', uiType = 'int', displayName = 'Max Consec Rejects', default='512', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'largemutationprob', uiType = 'float', displayName = 'Large Mutation Probability', default='0.4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mutationrange', uiType = 'float', displayName = 'Mutation Range', default='0.1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'usevariance', uiType = 'bool', displayName = 'Use Variance', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'initSamples', uiType = 'int', displayName = 'Init Samples', default='100000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'chainLength', uiType = 'int', displayName = 'Chain Length', default='2000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mutationRange', uiType = 'float', displayName = 'Mutation Range', default='0.1', uiDict=uiDict)
                
                with pm.frameLayout(label='image', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'imageFormat', uiType = 'enum', displayName = 'Image Format', default='1', data='Exr:Png:Tga', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'premultiplyAlpha', uiType = 'bool', displayName = 'Premultiply Alpha', default='true', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'pixelfilter', uiType = 'enum', displayName = 'Pixel Filter', default='3', data='box:trianlge:gauss:mitchell:sinc', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterWidth', uiType = 'float', displayName = 'Width', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterHeight', uiType = 'float', displayName = 'Height', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterAlpha', uiType = 'float', displayName = 'Alpha', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'B', uiType = 'float', displayName = 'MitchellB', default='.33', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'C', uiType = 'float', displayName = 'MitchellC', default='.33', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mSupersample', uiType = 'bool', displayName = 'Supersample', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sincTau', uiType = 'float', displayName = 'Tau', default='3.0', uiDict=uiDict)
                
                with pm.frameLayout(label='accelerator', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'accelerator', uiType = 'enum', displayName = 'Accelerator', default='0', data='kdtree:qbvh:grid (not thread-safe):unsafekdtree (not thread-safe):bvh (not thread-safe):none', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'kdIntersectcost', uiType = 'int', displayName = 'IntersectorCost', default='80', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'kdTraversalcost', uiType = 'int', displayName = 'TraversalCost', default='1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'kdEmptybonus', uiType = 'float', displayName = 'Empty Bonus', default='0.5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'kdMaxprims', uiType = 'int', displayName = 'Max Prims', default='1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'kdMaxdepth', uiType = 'int', displayName = 'Max Depth', default='-1', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'maxprimsperleaf', uiType = 'int', displayName = 'Max Prims Per Leaf', default='4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'fullsweepthreshold', uiType = 'int', displayName = 'Full Sweep Thresh', default='4 * 4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'skipfactor', uiType = 'int', displayName = 'Skip Factor', default='1', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'refineimmediately', uiType = 'bool', displayName = 'Refine Immediately', default='false', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'treetype', uiType = 'enum', displayName = 'Tree Type', default='0', data='binary:quadtree:octree', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'costsamples', uiType = 'int', displayName = 'Cost Samples', default='0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'surfaceIntegrator', uiType = 'enum', displayName = 'Surface Integrator', default='0', data='bidirectional (default):path:exphotonmap:directlighting:distributedpath', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightStrategy', uiType = 'enum', displayName = 'Light Strategy', default='2', data='one:all:auto:importance:powerimp:allpowerimp:logpowerimp', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'shadowraycount', uiType = 'int', displayName = 'Shadow Rays', default='1', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'eyedepth', uiType = 'int', displayName = 'Eye Depth', default='8', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightdepth', uiType = 'int', displayName = 'Light Depth', default='8', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'eyerrthreshold', uiType = 'float', displayName = 'Eye RR Thresh', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightrrthreshold', uiType = 'float', displayName = 'Light RR Thresh', default='0.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'pathMaxdepth', uiType = 'int', displayName = 'Max Depth', default='16', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'rrstrategy', uiType = 'enum', displayName = 'RR Strategy', default='2', data='none:probability:efficiency', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'rrcontinueprob', uiType = 'float', displayName = 'RR Continue', default='.65', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'dlightMaxdepth', uiType = 'int', displayName = 'Max Depth', default='5', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'phRenderingmode', uiType = 'enum', displayName = 'Rendering Mode', default='0', data='directlighting:path', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phCausticphotons', uiType = 'int', displayName = 'Caustic Photons', default='20000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phIndirectphotons', uiType = 'int', displayName = 'Indirect Photons', default='200000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phDirectphotons', uiType = 'int', displayName = 'Direct Photons', default='200000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phRadiancephotons', uiType = 'int', displayName = 'Radiance Photons', default='200000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phNphotonsused', uiType = 'int', displayName = 'Irradiance Photons', default='50', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phMaxphotondist', uiType = 'float', displayName = 'Max Photon Dist', default='0.5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phMaxdepth', uiType = 'int', displayName = 'Max Specular Bounces', default='5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phMaxphotondepth', uiType = 'int', displayName = 'Max Trace Depth', default='10', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phFinalgather', uiType = 'bool', displayName = 'Use Direct Lighing', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phFinalgathersamples', uiType = 'int', displayName = 'Final Gather Samples', default='32', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phGatherangle', uiType = 'float', displayName = 'Gather Angle', default='10.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phRrstrategy', uiType = 'enum', displayName = 'RR Strategy', default='2', data='none:probability:efficiency', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phRrcontinueprob', uiType = 'float', displayName = 'Cont Prob RR', default='0.65', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phDistancethreshold', uiType = 'float', displayName = 'Dist Thresold', default='1.25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phPhotonmapsfile', uiType = 'string', displayName = 'Photon File', default='""', uiDict=uiDict)

                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.LuxRendererUpdateTab()

    def LuxRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        self.updateEnvironment()
        log.debug("LuxRendererUpdateTab()")
        pass

    def LuxEnvironemntCreateTab(self, dummy = None):
        pass

    def LuxEnvironemntUpdateTab(self, dummy = None):
        pass
    
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
        
    def LuxTranslatorCreateTab(self):
        log.debug("LuxTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
                with pm.frameLayout(label="Lux XML export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="XMLFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Lux"
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

    def LuxTranslatorUpdateTab(self):
        log.debug("LuxTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)

    def registerNodeExtensions(self):
        """Register Lux specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtlu_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtlu_diaphragm_blades", attributeType="long", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtlu_diaphragm_tilt_angle", attributeType="float", defaultValue = 0.0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_useassembly", attributeType="bool", defaultValue = False)

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
            pm.mayatoLux()
        else:
            pm.mayatoLux(width=width, height=height, camera=camera)
            
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
        pm.mayatoLux(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoLux(stopIpr=True)
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

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    aeDir = path.path(__file__).dirname() + "/AETemplates/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {0}".format(templateName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            pm.mel.eval(melCommand)
            log.debug("load aeTemplate: " + templateName)

def theRenderer():
    return LuxRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Lux")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Lux FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

