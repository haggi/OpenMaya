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
        pass
            
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
                        self.addRenderGlobalsUIElement(attName = 'num_threads', uiType = 'int', displayName = 'Num Threads', default='0', uiDict=uiDict)

#                with pm.frameLayout(label='Image', collapsable = True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
#                        self.addRenderGlobalsUIElement(attName = 'aperture_diffraction', uiType = 'bool', displayName = 'Aperture Diffraction', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'post_process_diffraction', uiType = 'bool', displayName = 'Post Aperture Diffraction', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'render_foreground_alpha', uiType = 'bool', displayName = 'Alpha Image', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'vignetting', uiType = 'bool', displayName = 'Vignetting', default='true', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'watermark', uiType = 'bool', displayName = 'Watermark', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName = 'info_overlay', uiType = 'bool', displayName = 'Info Overlay', default='false', uiDict=uiDict)
                with pm.frameLayout(label='Environment', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'environmentType', uiType = 'enum', displayName = 'Environment Type', default='0', data='Color/Map:Sun', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'environmentColor', uiType = 'color', displayName = 'Environment Color', default='0.4:0.4:1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'environmentSun', uiType = 'message', displayName = 'Environment Sun', uiDict=uiDict)

                
                    
                    
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
        filename = pm.fileDialog2(fileMode=0, caption="XML Export File Name")
        if len(filename) > 0:
            print "Got filename", filename
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename[0])
    
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
                with pm.frameLayout(label="Indigo XML export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="XMLFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Indigo"
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

    def IndigoTranslatorUpdateTab(self):
        log.debug("IndigoTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register Indigo specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtin_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtin_diaphragm_blades", attributeType="long", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtin_diaphragm_tilt_angle", attributeType="float", defaultValue = 0.0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtin_mesh_useassembly", attributeType="bool", defaultValue = False)

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
        optimizeTextures.preRenderOptimizeTextures(optimizedFilePath = self.renderGlobalsNode.optimizedTexturePath.get())

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

