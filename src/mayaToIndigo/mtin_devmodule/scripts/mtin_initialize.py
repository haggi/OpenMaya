import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import Indigo.aeNodeTemplates as aet
import path

reload(Renderer)

log = logging.getLogger("mtIndigoLogger")


RENDERER_NAME="Indigo"

class IndigoRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg = None):
        if not IndigoRenderer.theRendererInstance:
            IndigoRenderer.theRendererInstance = IndigoRenderer(RENDERER_NAME , __name__)
            #IndigoRenderer.theRendererInstance = IndigoRenderer(RENDERER_NAME , "mtin_initialize")
        return IndigoRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             

    def updateEnvironment(self, dummy=None):
        
        if not self.rendererTabUiDict.has_key('environment'):
            return
        
        envDict = self.rendererTabUiDict['environment']
        envDict['environmentColor'].setEnable(False)
        envDict['turbidity'].setEnable(False)
        envDict['extra_atmospheric'].setEnable(False)
        envDict['sun_layer'].setEnable(False)
        envDict['sky_layer'].setEnable(False)
        envDict['sky_model'].setEnable(False)
        envDict['environmentMapType'].setEnable(False)
        envDict['environmentMapMultiplier'].setEnable(False)  
      

        etype =  self.renderGlobalsNode.environmentType.get()
        if etype == 1:
            envDict['environmentColor'].setEnable(True)
            envDict['environmentMapType'].setEnable(True)
            envDict['environmentMapMultiplier'].setEnable(True)  
        if etype == 2:
            envDict['turbidity'].setEnable(True)
            envDict['extra_atmospheric'].setEnable(True)
            envDict['sun_layer'].setEnable(True)
            envDict['sky_layer'].setEnable(True)
            envDict['sky_model'].setEnable(True)

    def editSun(self, *args):
        uiDict = self.rendererTabUiDict['environment']    
        suns = pm.ls("IndigoSun")
        if len(suns) > 0:
            pm.delete(suns)
            uiDict['sunButton'].setLabel("Create Sun")
        else:
            sun = pm.createNode("directionalLight")
            sun = sun.getParent()
            sun.rename("IndigoSun")
            sun.message >> self.renderGlobalsNode.sunLightConnection
            uiDict['sunButton'].setLabel("Delete Sun")            
            
        
    def IndigoEnvironmentUpdateTab(self):
        if not self.rendererTabUiDict.has_key('environment'):
            return
        self.updateEnvironment()
        
    def IndigoEnvironmentCreateTab(self):
        log.debug("IndigoEnvironmentCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key('environment'):
            self.rendererTabUiDict.pop('environment')        
        uiDict = {}
        self.rendererTabUiDict['environment'] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label='Environment', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'environmentType', uiType = 'enum', displayName = 'Environment Type', default='0', data='Color/Map:Sun', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'environmentColor', uiType = 'color', displayName = 'Environment Color', default='0.4:0.4:1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'environmentMapMultiplier', uiType = 'float', displayName = 'Emission Multiplier', default='10', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'environmentMapType', uiType = 'enum', displayName = 'Environment Map Type', default='0',  data='Spherical:Lat-Long', uiDict=uiDict)         
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'sky_model', uiType = 'enum', displayName = 'Sky Model', default='0', data='original:captured-simulation', uiDict=uiDict)                        
                        buttonLabel = "Create Sun"
                        suns = pm.ls("IndigoSun")
                        if len(suns) > 0:
                            buttonLabel = "Delete Sun"
                        uiDict['sunButton'] = pm.button(label = buttonLabel, command = self.editSun)
                        self.addRenderGlobalsUIElement(attName = 'turbidity', uiType = 'float', displayName = 'Sky Turbidity', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'extra_atmospheric', uiType = 'bool', displayName = 'Extra Atmospheric', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sun_layer', uiType = 'int', displayName = 'Sun Layer', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sky_layer', uiType = 'int', displayName = 'Sky Layer', default='0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

        pm.scriptJob(attributeChange=[self.renderGlobalsNode.environmentType, pm.Callback(self.uiCallback, tab="environment")])        
        self.updateEnvironment()

    def IndigoRendererUpdateTab(self):
        log.debug("IndigoRendererUpdateTab()")
        if not self.rendererTabUiDict.has_key('common'):
            return
        
        envDict = self.rendererTabUiDict['common']
        
        envDict['threads'].setEnable(True)
        if self.renderGlobalsNode.auto_choose_num_threads.get():
            envDict['threads'].setEnable(False)

        
            
    def IndigoRendererCreateTab(self):
        log.debug("IndigoRendererCreateTab()")
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
                with pm.frameLayout(label='Global Settings', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'doDof', uiType = 'bool', displayName = 'Depth Of Field', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'doMotionBlur', uiType = 'bool', displayName = 'Motion Blur', default=False, uiDict=uiDict)
#                with pm.frameLayout(label='Frame Buffer', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'super_sample_factor', uiType = 'int', displayName = 'Supersampling', default='2', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'splat_filter', uiType = 'enum', displayName = 'Splat Filter', default='0', data='fastbox:radial:mn_cubic', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'splat_filter_cubic_b', uiType = 'float', displayName = 'Cubic Splat B', default='0.6', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'splat_filter_cubic_ring', uiType = 'float', displayName = 'Cubic Splat Ring', default='0.2', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'downsize_filter', uiType = 'enum', displayName = 'Downsize Filter', default='0', data='mn_cubic:gauss:sharp', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'downsize_filter_cubic_radius', uiType = 'float', displayName = 'Cubic DS Radius', default='2', uiDict=uiDict)
#                        
#                with pm.frameLayout(label='Output', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'save_untonemapped_exr', uiType = 'bool', displayName = 'Untonemapped EXR', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'save_tonemapped_exr', uiType = 'bool', displayName = 'Tonemapped EXR', default='True', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'save_igi', uiType = 'bool', displayName = 'Save Indigo ImageFile', default='false', data='', uiDict=uiDict)
#
#                with pm.frameLayout(label='Renderer', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'bih_tri_threshold', uiType = 'int', displayName = 'BIH Threshold', default='1100000', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'metropolis', uiType = 'bool', displayName = 'Metropolis', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'large_mutation_prob', uiType = 'float', displayName = 'Large Mutation Probability', default='0.1', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'max_change', uiType = 'float', displayName = 'Perturb Radius', default='.01', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'max_num_consec_rejections', uiType = 'int', displayName = 'Number of Rejections', default='1000', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'path_tracing', uiType = 'enum', displayName = 'Path Tracing', default='0', data='bidirectional:backwards', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'hybrid', uiType = 'bool', displayName = 'Hybrid Sampling', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'glass_acceleration', uiType = 'bool', displayName = 'Glass Accerlation', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'gpu', uiType = 'bool', displayName = 'GPU Acceleration', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'selected_gpu_device', uiType = 'enum', displayName = 'GPU Device', default='0', data='CUDA:OPEN_CL', uiDict=uiDict)
#
#                with pm.frameLayout(label='Misc', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'logging', uiType = 'bool', displayName = 'Logging', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'image_save_period', uiType = 'float', displayName = 'Img Save Period', default='60', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'cache_trees', uiType = 'bool', displayName = 'Cache Kd-Trees', default='true', uiDict=uiDict)

                with pm.frameLayout(label='Halt conditions', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'halt_time', uiType = 'float', displayName = 'Halt Time', default='-1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'halt_samples_per_pixel', uiType = 'int', displayName = 'Max Samples PP', default='-1', uiDict=uiDict)
                
                with pm.frameLayout(label='Threads', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'auto_choose_num_threads', uiType = 'bool', displayName = 'Auto Choose Threads', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Num Threads', default='0', uiDict=uiDict)

#                with pm.frameLayout(label='Image', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'aperture_diffraction', uiType = 'bool', displayName = 'Aperture Diffraction', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'post_process_diffraction', uiType = 'bool', displayName = 'Post Aperture Diffraction', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'render_foreground_alpha', uiType = 'bool', displayName = 'Alpha Image', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'vignetting', uiType = 'bool', displayName = 'Vignetting', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'watermark', uiType = 'bool', displayName = 'Watermark', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'info_overlay', uiType = 'bool', displayName = 'Info Overlay', default='false', uiDict=uiDict)

                
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.IndigoRendererUpdateTab()

    def IndigoRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        self.updateEnvironment()
        log.debug("IndigoRendererUpdateTab()")
        
    def xmlFileBrowse(self, args=None):
        print "xmlfile", args
        filename = pm.fileDialog2(fileMode=0, caption="Export Indigo File Name")
        if len(filename) > 0:
            filename = filename[0]
            print "Got filename", filename
            if not filename.endswith(".igs"):
                filename += ".igs"
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename)
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])
        
    def IndigoTranslatorCreateTab(self):
        log.debug("IndigoTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
                with pm.frameLayout(label="Indigo Scene File export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export Indigo Scene file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".igs"
                    if not defaultXMLPath.dirname().exists():
                        defaultXMLPath.dirname().makedirs()
                    with pm.rowLayout(nc=3):
                        xmlDict['xmlFileText'] = pm.text(label = "Export to")
                        xmlDict['xmlFile'] = pm.textField(text = defaultXMLPath)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index = 2 )
                with pm.frameLayout(label="Optimize Textures", collapsable = True, collapse=False):
                    optiDict = {}
                    ui = pm.checkBoxGrp(label="Use Optimized Textures:", value1 = False)
                    with pm.rowLayout(nc=3):
                        self.rendererTabUiDict['opti'] = optiDict
                        pm.text(label="OptimizedTex Dir:")
                        optiDict['optiField'] = pm.textField(text = self.renderGlobalsNode.optimizedTexturePath.get())
                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
                        pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index = 2 )
                with pm.frameLayout(label="Additional Settings", collapsable = True, collapse=False):
                    ui = pm.floatFieldGrp(label="Scene scale:", value1 = 1.0, numberOfFields = 1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index = 2 )
                         
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def IndigoTranslatorUpdateTab(self):
        log.debug("IndigoTranslatorUpdateTab()")

    def uiCallback(self, **args):
        log.debug("uiCallback()")
        
        if args['tab'] == "environment":
            self.updateEnvironment()

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register Indigo specific node extensions. e.g. camera type, diaphram_blades and others
        """
        pm.addExtension(nodeType="camera", longName="mtin_lensRadius", attributeType="float", defaultValue = 0.1)
        pm.addExtension(nodeType="camera", longName="mtin_autoFocus", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="camera", longName="mtin_whiteBalance", attributeType="enum", enumName = "D50:D55:D65")
        pm.addExtension(nodeType="camera", longName="mtin_numBlades", attributeType="long", defaultValue = 5)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivUse", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivMaxSubdiv", attributeType="long", defaultValue = 0)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivViewDependent", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivSmooth", attributeType="bool", defaultValue = True)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivPixelThreshold", attributeType="float", defaultValue = 0.4)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivCurvatureThreshold", attributeType="float", defaultValue = 0.1)
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_subdivErrorThreshold", attributeType="float", defaultValue = 0.1)

        # 
    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Environment"))    

        
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
            pm.mayatoIndigo()
        else:
            pm.mayatoIndigo(width=width, height=height, camera=camera)
            
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
        pm.mayatoIndigo(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoIndigo(stopIpr=True)
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
        #optimizeTextures.preRenderOptimizeTextures(optimizedFilePath = self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AEIndigoNodeTemplate(nodeName)
        
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
    rendererName = "Indigo"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

def theRenderer():
    return IndigoRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Indigo")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Indigo FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

