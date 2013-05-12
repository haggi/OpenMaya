import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import path

import optimizeTextures
import Lux.aeNodeTemplates as LuxTemplates

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

    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Environment"))    

    def updateEnvironment(self, dummy=None):
        if not self.rendererTabUiDict.has_key('environment'):
            return
        envDict = self.rendererTabUiDict['environment']
        
        for key, val in envDict.iteritems():
            val.setEnable(False)
            
        # enable physical sky and enable physical sun becuse we can use physical sun without physical sky
        envDict['pskSkyUse'].setEnable(True)
        envDict['pskUsePhySun'].setEnable(True)
        
        if not self.renderGlobalsNode.usePhysicalSky.get():
            return
        
        try:
            sunConnection = self.renderGlobalsNode.physicalSunConnection.listConnections()[0]
        except:
            log.info("No asSunLight connected, searching for one.")
            try:
                sunConnection = pm.ls("asSunLight")[0]
                sunConnection.message >> self.renderGlobalsNode.physicalSunConnection
            except:
                log.info("No light called asSunLight in scene, creating a new one.")
                lightShape = pm.createNode("directionalLight")
                sunConnection = lightShape.getParent()
                sunConnection.rename("asSunLight")
                sunConnection.message >> self.renderGlobalsNode.physicalSunConnection
        envDict['pskPhySun'].setText(str(sunConnection))
            
        if self.renderGlobalsNode.physicalSun.get():
            envDict['pskRelSize'].setEnable(True)
            
        envDict['pskSunGain'].setEnable(True)
        envDict['pskTurb'].setEnable(True)
        envDict['pskSamples'].setEnable(True)
    
            
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
                        self.addRenderGlobalsUIElement(attName = 'haltspp', uiType = 'int', displayName = 'Halt Samper Per Pixel', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'halttime', uiType = 'int', displayName = 'Halt Time (sec)', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'uiupdateinterval', uiType = 'int', displayName = 'Ui Update Interval (sec)', default='2', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'doMotionBlur', uiType = 'bool', displayName = 'Motionblur:', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'doDof', uiType = 'bool', displayName = 'Depth of Field:', default=False, uiDict=uiDict)
#                        ui = pm.checkBoxGrp(label="Motionblur:", value1=False)
#                        pm.connectControl(ui, self.renderGlobalsNodeName + ".doMotionBlur", index=2)
#                        ui = pm.checkBoxGrp(label="Depth Of Field:", value1=False)
#                        pm.connectControl(ui, self.renderGlobalsNodeName + ".doDof", index=2)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'renderer', uiType = 'enum', displayName = 'Renderer', default='0', data='sampler:hybridsampler:hybridsppm', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'hsConfigFile', uiType = 'string', displayName = 'HSConfig', default='""', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclPlatformIndex', uiType = 'int', displayName = 'OCL Platform Index', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclGpuUse', uiType = 'bool', displayName = 'OCL Gpu Use', default='1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'hsOclWGroupSize', uiType = 'int', displayName = 'OCL WorkgroupSize', default='0', uiDict=uiDict)
                
                with pm.frameLayout(label='Sampler', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'sampler', uiType = 'enum', displayName = 'Sampler', default='0', data='random:low discrepancy:metropolis:rrpt', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'pixelSampler', uiType = 'enum', displayName = 'Pixel Sampler', default='2', data='hilbert:linear:vegas:lowdiscrepancy:tile:random', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'numSamples', uiType = 'int', displayName = 'Samples', default='4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'noiseaware', uiType = 'bool', displayName = 'Noise Aware', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxconsecrejects', uiType = 'int', displayName = 'Max Consec Rejects', default='512', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'largemutationprob', uiType = 'float', displayName = 'Large Mutation Probability', default='0.4', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mutationrange', uiType = 'float', displayName = 'Mutation Range', default='0.1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'usevariance', uiType = 'bool', displayName = 'Use Variance', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'usecooldown', uiType = 'bool', displayName = 'Use Cooldown', default='0', uiDict=uiDict)
                        #self.addRenderGlobalsUIElement(attName = 'initSamples', uiType = 'int', displayName = 'Init Samples', default='100000', uiDict=uiDict)
                        #self.addRenderGlobalsUIElement(attName = 'chainLength', uiType = 'int', displayName = 'Chain Length', default='2000', uiDict=uiDict)
                        #self.addRenderGlobalsUIElement(attName = 'mutationRange', uiType = 'float', displayName = 'Mutation Range', default='0.1', uiDict=uiDict)
                
                with pm.frameLayout(label='image', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'imageFormat', uiType = 'enum', displayName = 'Image Format', default='1', data='Exr:Png:Tga', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'premultiplyAlpha', uiType = 'bool', displayName = 'Premultiply Alpha', default='true', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'pixelfilter', uiType = 'enum', displayName = 'Pixel Filter', default='3', data='box:trianlge:gauss:mitchell:sinc', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'filterWidth', uiType = 'float', displayName = 'Width', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterHeight', uiType = 'float', displayName = 'Height', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterAlpha', uiType = 'float', displayName = 'Alpha', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'B', uiType = 'float', displayName = 'MitchellB', default='.33', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'C', uiType = 'float', displayName = 'MitchellC', default='.33', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mSupersample', uiType = 'bool', displayName = 'Supersample', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sincTau', uiType = 'float', displayName = 'Tau', default='3.0', uiDict=uiDict)
                
                with pm.frameLayout(label='accelerator', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                
                        self.addRenderGlobalsUIElement(attName = 'accelerator', uiType = 'enum', displayName = 'Accelerator', default='0', data='kdtree:qbvh:grid (not thread-safe):unsafekdtree (not thread-safe):bvh (not thread-safe):none', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
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

                with pm.frameLayout(label='surface integrator', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        self.addRenderGlobalsUIElement(attName = 'surfaceIntegrator', uiType = 'enum', displayName = 'Surface Integrator', default='0', data='bidirectional (default):path:exphotonmap:directlighting:distributedpath', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'lightStrategy', uiType = 'enum', displayName = 'Light Strategy', default='2', data='one:all:auto:importance:powerimp:allpowerimp:logpowerimp', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'shadowraycount', uiType = 'int', displayName = 'Shadow Rays', default='1', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'eyedepth', uiType = 'int', displayName = 'Eye Depth', default='8', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightdepth', uiType = 'int', displayName = 'Light Depth', default='8', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'eyerrthreshold', uiType = 'float', displayName = 'Eye RR Thresh', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightrrthreshold', uiType = 'float', displayName = 'Light RR Thresh', default='0.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'pathMaxdepth', uiType = 'int', displayName = 'Max Depth', default='16', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'rrstrategy', uiType = 'enum', displayName = 'RR Strategy', default='2', data='none:probability:efficiency', uiDict=uiDict,callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'rrcontinueprob', uiType = 'float', displayName = 'RR Continue', default='.65', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'dlightMaxdepth', uiType = 'int', displayName = 'Max Depth', default='5', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'phRenderingmode', uiType = 'enum', displayName = 'Rendering Mode', default='0', data='directlighting:path', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
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
                        self.addRenderGlobalsUIElement(attName = 'phRrstrategy', uiType = 'enum', displayName = 'RR Strategy', default='2', data='none:probability:efficiency', uiDict=uiDict, callback=pm.Callback(self.LuxRendererUpdateTab))
                        self.addRenderGlobalsUIElement(attName = 'phRrcontinueprob', uiType = 'float', displayName = 'Cont Prob RR', default='0.65', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phDistancethreshold', uiType = 'float', displayName = 'Dist Thresold', default='1.25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'phPhotonmapsfile', uiType = 'string', displayName = 'Photon File', default='""', uiDict=uiDict)

                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.LuxRendererUpdateTab()

    def LuxRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        #self.updateEnvironment()
        log.debug("LuxRendererUpdateTab()")
        
        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        
        defaultOn = ['haltspp','halttime', 'doMotionBlur', 'doDof','uiupdateinterval','renderer', 'sampler', 'imageFormat', 'premultiplyAlpha', 'noiseaware', 'pixelfilter', 'surfaceIntegrator', 'shadowraycount', 'lightStrategy']
        
        for key, val in uiDict.iteritems():
            val.setEnable(False)

        for key, val in uiDict.iteritems():
            if key in defaultOn:
                val.setEnable(True)

        sampler = self.renderGlobalsNode.sampler.get()
        if sampler in [0,1]:
            uiDict['numSamples'].setEnable(True)
            uiDict['pixelSampler'].setEnable(True)

        # metropolis
        if sampler == 2:            
            uiDict['maxconsecrejects'].setEnable(True)
            uiDict['largemutationprob'].setEnable(True)
            uiDict['mutationrange'].setEnable(True)
            uiDict['usevariance'].setEnable(True)
            uiDict['usecooldown'].setEnable(True)

        renderer = self.renderGlobalsNode.renderer.get()
        if renderer == 2:
            uiDict['hsConfigFile'].setEnable(True)
            uiDict['hsOclPlatformIndex'].setEnable(True)
            uiDict['hsOclGpuUse'].setEnable(True)
            uiDict['hsOclWGroupSize'].setEnable(True)
            
        pixelfilter = self.renderGlobalsNode.pixelfilter.get()
        uiDict['filterWidth'].setEnable(True)
        uiDict['filterHeight'].setEnable(True)
        if pixelfilter == 2:
            uiDict['filterAlpha'].setEnable(True)
        if pixelfilter == 3:
            uiDict['B'].setEnable(True)
            uiDict['C'].setEnable(True)
            uiDict['mSupersample'].setEnable(True)
        if pixelfilter == 4:
            uiDict['sincTau'].setEnable(True)
                        
            
    def LuxEnvironmentCreateTab(self, dummy = None):
        log.debug("LuxEnvironmentCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ENScrollLayout"
        envDict = {}
        self.rendererTabUiDict['environment'] = envDict
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Environment Lighting", collapsable=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        envDict['pskSkyUse'] = pm.checkBoxGrp(label="Use physical Sky:", value1=True, cc=pm.Callback(self.uiCallback, tab="environment"))
                        pm.connectControl(envDict['pskSkyUse'], self.renderGlobalsNodeName + ".usePhysicalSky", index=2)             
                        envDict['pskUsePhySun'] = pm.checkBoxGrp(label="Use Physical Sun:", value1=False, cc=pm.Callback(self.uiCallback, tab="environment"))
                        pm.connectControl(envDict['pskUsePhySun'], self.renderGlobalsNodeName + ".physicalSun", index=2)                    
                        envDict['pskPhySun'] = pm.textFieldGrp(label="Sun Object:", text="", editable=False) 
                        pm.separator()
                        envDict['pskSunGain'] = pm.floatFieldGrp(label="Sun Gain:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSunGain'], self.renderGlobalsNodeName + ".sunGain", index=2)             
                        envDict['pskTurb'] = pm.floatFieldGrp(label="Turbidity:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskTurb'], self.renderGlobalsNodeName + ".turbidity", index=2)                    
                        envDict['pskSamples'] = pm.floatFieldGrp(label="Samples:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSamples'], self.renderGlobalsNodeName + ".skySamples", index=2)                    
                        envDict['pskRelSize'] = pm.floatFieldGrp(label="Sun Size:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskRelSize'], self.renderGlobalsNodeName + ".sunRelSize", index=2)                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.physicalSun, pm.Callback(self.uiCallback, tab="environment")])   
        
        self.updateEnvironment()     

    def LuxEnvironmentUpdateTab(self, dummy = None):
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
                with pm.frameLayout(label="Lux Scene File export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="SceneFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Lux"
                        xmlDict['xmlFile'] = pm.textField(text = defaultXMLPath, width = 60)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index = 2 )
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
            
    def uiCallback(self, **args):
        log.debug("uiCallback()")
        
        if args['tab'] == "environment":
            self.updateEnvironment()
            

    def registerNodeExtensions(self):
        """Register Lux specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtlu_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtlu_diaphragm_blades", attributeType="long", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtlu_autofocus", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="camera", longName="mtlu_distribution", attributeType="enum", enumName = "uniform:exponential:inverse exponential:gaussian:inverse gaussian")
        pm.addExtension(nodeType="camera", longName="mtlu_power", attributeType="long", defaultValue = 1)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_generatetangents", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_subAlgo", attributeType="enum", enumName = "loop:microdisplacement")
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_displacementMap", dataType="string", usedAsFilename=True)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_dmscale", attributeType="float", defaultValue=0.1)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_dmoffset", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_dmnormalsmooth", attributeType="bool", defaultValue = True)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_dmnormalsplit", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_dmsharpboundary", attributeType="bool", defaultValue = False)
        pm.addExtension(nodeType="mesh", longName="mtlu_mesh_subdivlevel", attributeType="long", defaultValue = 0)

        # lights
        pm.addExtension(nodeType="light", longName="mtlu_light_importance", attributeType="float", defaultValue = 1.0)
        
        pm.addExtension(nodeType="directionalLight", longName="mtlu_dirLight_theta", attributeType="float", defaultValue = 0.0)
        
        pm.addExtension(nodeType="areaLight", longName="mtlu_areaLight_samples", attributeType="long", defaultValue = 1)
        pm.addExtension(nodeType="areaLight", longName="mtlu_areaLight_power", attributeType="float", defaultValue = 100.0)
        pm.addExtension(nodeType="areaLight", longName="mtlu_areaLight_efficacy", attributeType="float", defaultValue = 17.0)
        pm.addExtension(nodeType="areaLight", longName="mtlu_areaLight_ies", dataType="string", usedAsFilename=True)
        pm.addExtension(nodeType="areaLight", longName="mtlu_areaLight_geo", attributeType="message")
        
        pm.addExtension(nodeType="pointLight", longName="mtlu_pointLight_power", attributeType="float", defaultValue = 100.0)
        pm.addExtension(nodeType="pointLight", longName="mtlu_pointLight_efficacy", attributeType="float", defaultValue = 17.0)
        pm.addExtension(nodeType="pointLight", longName="mtlu_pointLight_ies", dataType="string", usedAsFilename=True)
        
        pm.addExtension(nodeType="spotLight", longName="mtlu_spotLight_power", attributeType="float", defaultValue = 100.0)
        pm.addExtension(nodeType="spotLight", longName="mtlu_spotLight_efficacy", attributeType="float", defaultValue = 17.0)
        pm.addExtension(nodeType="spotLight", longName="mtlu_spotLight_ies", dataType="string", usedAsFilename=True)

        pm.addExtension(nodeType="ambientLight", longName="mtlu_ambientLight_map", dataType="string", usedAsFilename=True)
        pm.addExtension(nodeType="ambientLight", longName="mtlu_ambientLight_samples", attributeType="long", defaultValue = 1)
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
        #optimizeTextures.preRenderOptimizeTextures(optimizedFilePath = self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        LuxTemplates.AELuxNodeTemplate(nodeName)
        

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    rendererName = "Lux"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

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

