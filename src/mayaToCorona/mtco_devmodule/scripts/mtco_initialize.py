import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import renderGlobalsUIInfo

reload(Renderer)

log = logging.getLogger("mtCoronaLogger")


RENDERER_NAME="Corona"

class CoronaRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    
    @staticmethod
    def theRenderer(arg = None):
        if not CoronaRenderer.theRendererInstance:
            CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , __name__)
            #CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , "mtco_initialize")
        return CoronaRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             

    def addUIElement(self, uiType, attribute, uiLabel):
        ui = None
        if uiType == 'bool':
            ui = pm.checkBoxGrp(label=uiLabel)
        if uiType == 'int':
            ui = pm.intFieldGrp(label=uiLabel, numberOfFields = 1)
        if uiType == 'float':
            ui = pm.floatFieldGrp(label=uiLabel, numberOfFields = 1)
        if uiType == 'enum':
            ui = pm.attrEnumOptionMenuGrp(label = uiLabel, at=attribute, ei = self.getEnumList(attribute)) 
        if uiType == 'color':
            ui = pm.attrColorSliderGrp(label=uiLabel, at=attribute)
        if uiType == 'string':
            ui = pm.textFieldGrp(label=uiLabel)
        return ui
    
    def connectUIElement(self, uiElement, attribute):
        
        if attribute.type() == 'enum':
            #enum is automatically connnected via attrEnumOptionMenu
            return        
        if attribute.type() == 'float3':
            #float3 == color is automatically connnected via attrColorSliderGrp
            return
        if attribute.type() == 'message':
            #no automatic connection necessary, will be controlled by other scritps
            return
        log.debug("Adding connection for {0}".format(attribute))
        pm.connectControl(uiElement, attribute, index = 2)
    
    def addRenderGlobalsUIElement(self, attName = None, uiType = None, displayName = None, default=None, data=None, uiDict=None):
        
        attribute = pm.Attribute(self.renderGlobalsNodeName + "." + attName)
        uiElement = self.addUIElement(uiType, attribute, displayName)
        self.connectUIElement(uiElement, attribute)
                
    def CoronaRendererCreateTab(self):
        log.debug("CoronaRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Sampling", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                                    
                        self.addRenderGlobalsUIElement(attName = 'subdivAA', uiType = 'int', displayName = 'Antialias Subdivs', default='1', uiDict=uiDict)                        
                        self.addRenderGlobalsUIElement(attName = 'adaptiveSteps', uiType = 'int', displayName = 'Adaptive Steps', default='2', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'adaptiveThreshold', uiType = 'float', displayName = 'Adaptive Threshold', default='0.03', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'AASampler', uiType = 'enum', displayName = 'Sampler', default='0', data='Adaptive:None', uiDict=uiDict)
                        pm.separator()                        
                        self.addRenderGlobalsUIElement(attName = 'accStruct', uiType = 'enum', displayName = 'Acceleration Structure', default='2', data='None:BVH full SAH:Embree BVH2:Embree BVH Spatial:Embree BVH4:Embree BVH4 Spatial', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'embreeTris', uiType = 'enum', displayName = 'Embree Triangles', default='0', data='Fast:Precise:Low Memory', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'minInstSaving', uiType = 'int', displayName = 'Min. Inst Saving', default='50000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxDepth', uiType = 'int', displayName = 'Max Depth', default='25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'exitColor', uiType = 'color', displayName = 'Ray Exit Color', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxNormalDiff', uiType = 'float', displayName = 'Max Normal Diff', default='0.55', uiDict=uiDict)
                        pm.separator()                        
                        self.addRenderGlobalsUIElement(attName = 'timeH', uiType = 'int', displayName = 'Hours', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'timeM', uiType = 'int', displayName = 'Minutes', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'timeS', uiType = 'int', displayName = 'Seconds', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'numberOfPasses', uiType = 'int', displayName = 'Number of Passes', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'adaptivity', uiType = 'float', displayName = 'Adaptivity', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'realcENP', uiType = 'int', displayName = 'Recalc Each N Pass', default='999', uiDict=uiDict)
                        
                with pm.frameLayout(label='Bokeh/DOF', collapsable = True, collapse=False):     
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'bokehUse', uiType = 'bool', displayName = 'Use Bokeh', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sensorWidth', uiType = 'float', displayName = 'Sensor Width', default='35.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'bokehBlades', uiType = 'int', displayName = 'Blades', default='6', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'bokehRot', uiType = 'float', displayName = 'Rotation', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'bokehCustomShape', uiType = 'message', displayName = 'Custom Shape', default='-', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'bokehCustomShapeRes', uiType = 'int', displayName = 'Custom Shape Resolution', default='500', uiDict=uiDict)
                        
                with pm.frameLayout(label='Motion blur', collapsable = True, collapse=False):     
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'mbXform', uiType = 'bool', displayName = 'Transformation Blur', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mbDeform', uiType = 'bool', displayName = 'Deformation Blur', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mbDuration', uiType = 'float', displayName = 'Duration', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mbFrameOffset', uiType = 'float', displayName = 'Frame Offset', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mbTransformSegments', uiType = 'int', displayName = 'Transform Segments', default='16', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'mbDeformSegments', uiType = 'int', displayName = 'Deform Segments', default='5', uiDict=uiDict)
                        
                with pm.frameLayout(label='Displacement', collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'mode', uiType = 'enum', displayName = 'Subiv Mode', default='0', data='World:Camera', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'edgeLen', uiType = 'float', displayName = 'Edge Length', default='1.0', uiDict=uiDict)
                        pm.separator()                                                
                        self.addRenderGlobalsUIElement(attName = 'showVfb', uiType = 'bool', displayName = 'Use Corona Frame Buffer', default='false', uiDict=uiDict)
                        
                with pm.frameLayout(label='Pixel Filter', collapsable = True, collapse=False):                        
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'pixelFilter', uiType = 'enum', displayName = 'Pixel Filter', default='1', data='None:Box:Tent:Cone:Gauss', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterSize', uiType = 'float', displayName = 'Filer Size', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'blurSize', uiType = 'float', displayName = 'Blur Size', default='0.5', uiDict=uiDict)
                        pm.separator()                                                                        
                        self.addRenderGlobalsUIElement(attName = 'infoStamp', uiType = 'string', displayName = 'Info Stamp', default='Time %t', uiDict=uiDict)
                        
                with pm.frameLayout(label='Settings', collapsable = True, collapse=False):                        
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'multiThreading', uiType = 'bool', displayName = 'Use Multi Threading', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lowThreadPrio', uiType = 'bool', displayName = 'Low Thread Prio', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'exportSceneOnly', uiType = 'bool', displayName = 'Export Scene Only', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'overrideMaterial', uiType = 'string', displayName = 'Override Material', default='""', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'allObjectsHaveSolidAlpha', uiType = 'bool', displayName = 'All Solid Alpha', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'doShading', uiType = 'bool', displayName = 'Do Shading', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'filterTexture', uiType = 'bool', displayName = 'Filter Textures', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'renderer', uiType = 'enum', displayName = 'Renderer', default='0', data='Progressive:LightTracer:Bucket', uiDict=uiDict)
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaRendererUpdateTab()

    def CoronaRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        #self.updateEnvironment()
        log.debug("CoronaRendererUpdateTab()")
#        if self.renderGlobalsNode.adaptiveSampling.get():
#            self.rendererTabUiDict['minSamples'].setEnable(True)
#            self.rendererTabUiDict['maxError'].setEnable(True)
#        else:
#            self.rendererTabUiDict['minSamples'].setEnable(False)
#            self.rendererTabUiDict['maxError'].setEnable(False)

    def CoronaGiCreateTab(self):
        log.debug("CoronaGiCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="GI", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'primatrySolver', uiType = 'enum', displayName = 'Primary Solver', default='0', data='IrradianceCache:PathTracing:PhotonMapping:HD Cache', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'secondarySolver', uiType = 'enum', displayName = 'Secondary Solver', default='2', data='PathTracing:PhotonMapping:HD Cache', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pathTracingSamples', uiType = 'int', displayName = 'Path Tracing Samples', default='16', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxSampleIntensity', uiType = 'float', displayName = 'Max Sample Intensity', default='10.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'loadSecondaryGI', uiType = 'bool', displayName = 'Load Secondary GI', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'saveSecondaryGI', uiType = 'bool', displayName = 'Save Secondary GI', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'secondaryGIFile', uiType = 'string', displayName = 'Secondary GI File', default='""', uiDict=uiDict)
                        
                with pm.frameLayout(label="Primary GI", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'interpolation', uiType = 'enum', displayName = 'Interpolation', default='0', data='Tabellion&Larmorette:Ward et. al.', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxGeoError', uiType = 'float', displayName = 'Max Geometric Error', default='0.6', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'smoothing', uiType = 'float', displayName = 'Smoothing', default='1.8', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorThreshold', uiType = 'float', displayName = 'ColorThreshold', default='10.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxInterpSamples', uiType = 'int', displayName = 'Max Interpolation Samples', default='2', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gradientRot', uiType = 'bool', displayName = 'Gradient Rotation', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gradientTrans', uiType = 'bool', displayName = 'Gradient Translation', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'relaxedInterp', uiType = 'bool', displayName = 'Relaxed Interpolation', default='true', uiDict=uiDict)
                        
                        pm.separator()
                        
                        self.addRenderGlobalsUIElement(attName = 'hemisphericalSubd', uiType = 'int', displayName = 'Hemispherical Subdivs', default='7', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'validityMin', uiType = 'float', displayName = 'Validity Min', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'validityMax', uiType = 'float', displayName = 'Validity Max', default='20.0', uiDict=uiDict)
                        
                        pm.separator()
                        
                        self.addRenderGlobalsUIElement(attName = 'finalRes', uiType = 'int', displayName = 'Final Resolution', default='-1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'iterations', uiType = 'int', displayName = 'Iterations', default='5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'usePathTraceCorners', uiType = 'bool', displayName = 'Use Path Tracing Corners', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'incrementalBuild', uiType = 'bool', displayName = 'Incremental Build', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'previz', uiType = 'enum', displayName = 'Previz', default='0', data='Indirect Lighting:Dots:Off', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'serachStruct', uiType = 'enum', displayName = 'Seracrh Struct', default='0', data='MultiRef Octree:MultiRef KD Tree:BVH', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'storeDirectLight', uiType = 'bool', displayName = 'Store Direct Lighting', default='false', uiDict=uiDict)
                        
                with pm.frameLayout(label="Secondary GI", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'emittedPhotons', uiType = 'int', displayName = 'Emitted Photons', default='5000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lookUpCount', uiType = 'int', displayName = 'Lookup Count', default='50', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'photonStoreDirectLight', uiType = 'bool', displayName = 'Photon Store Direct', default='true', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaGiUpdateTab()

                        
    def CoronaGiUpdateTab(self):
        pass
    
    def CoronaLightsCreateTab(self):
        log.debug("CoronaLightsCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Post Effects", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'lightSolver', uiType = 'enum', displayName = 'Light Solver', default='3', data='Simple:Globally Adaptive:Locally Adaptive:Combined', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightSamplingMode', uiType = 'enum', displayName = 'Light SamplingMode', default='0', data='MIS Both:BSDF Sampling:Off', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightSamplesMultiplier', uiType = 'float', displayName = 'Light Samples Multiplier', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightSubdivThreshold', uiType = 'float', displayName = 'Light Subdiv Threshold', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightTextureRes', uiType = 'float', displayName = 'Light Texture Resolution', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lightSpotThresh', uiType = 'float', displayName = 'Light Spot Threshold', default='100.0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaLightsUpdateTab()
        
    def CoronaLightsUpdateTab(self):
        pass
    
    def CoronaEnvCreateTab(self):
        log.debug("CoronaEnvCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Post Effects", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'useCorona', uiType = 'bool', displayName = 'Use Corona', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'directVisibleOverride', uiType = 'color', displayName = 'Direct Visible Override', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'reflectionsOverride', uiType = 'color', displayName = 'Reflections Override', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'refractionsOverride', uiType = 'color', displayName = 'Refractions Override', default='0:0:0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaEnvUpdateTab()
        
    def CoronaEnvUpdateTab(self):
        pass
    
    def CoronaPostCreateTab(self):
        log.debug("CoronaPostCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "ScrollLayout"
        uiDict = {}
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Post Effects", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn = True, width = 400):
                        
                        self.addRenderGlobalsUIElement(attName = 'sceneWhitepoint', uiType = 'float', displayName = 'Scene Whitepoint[K]', default='6500.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'displayWhitepoint', uiType = 'float', displayName = 'Display Whitepoint[K]', default='6500.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'catColorSpace', uiType = 'enum', displayName = 'Cat Color Space', default='0', data='RGB:None', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorChannelOutput', uiType = 'color', displayName = 'ColorChannelOutput', default='1:1:1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'intensityMult', uiType = 'float', displayName = 'Intensity Multiplier', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'highLightBurn', uiType = 'float', displayName = 'Hightlight Burn', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gamma', uiType = 'float', displayName = 'Gamma', default='2.2', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'contrast', uiType = 'float', displayName = 'Contrast', default='1.0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaPostUpdateTab()
        
    def CoronaPostUpdateTab(self):
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
        
    def CoronaTranslatorCreateTab(self):
        log.debug("CoronaTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
#                with pm.frameLayout(label="Corona XML export", collapsable = True, collapse=False):
#                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
#                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
#                    xmlDict = {}
#                    self.rendererTabUiDict['xml'] = xmlDict
#                    with pm.rowColumnLayout(nc=3, width = 120):
#                        pm.text(label="XMLFileName:", width = 60, align="right")
#                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Corona"
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

    def CoronaTranslatorUpdateTab(self):
        log.debug("CoronaTranslatorUpdateTab()")

    def addUserTabs(self):
        for d in renderGlobalsUIInfo.UIList:
            if d['name'] not in "common":
                pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure(d['name'].capitalize()))    


    def createImageFormats(self):
        self.imageFormats = []
        self.imageFormats.append('Exr')
        self.imageFormats.append('Png')

    def registerNodeExtensions(self):
        pass
        """Register Corona specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtco_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
#        pm.addExtension(nodeType="camera", longName="mtco_diaphragm_blades", attributeType="long", defaultValue = 0)
#        pm.addExtension(nodeType="camera", longName="mtco_diaphragm_tilt_angle", attributeType="float", defaultValue = 0.0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtco_meshsubdivide", attributeType="bool", defaultValue = False)
        
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
        pass
#        logfile = pm.workspace.path + "/applelog.log"
#        try:
#            if os.path.exists(logfile):
#                os.remove(logfile)
#        except:
#            pass

    def showLogFile(self):
        pass
#        logfile = pm.workspace.path + "/applelog.log"
#        if os.path.exists(logfile):
#            lh = open(logfile, 'r')
#            rl = lh.readlines()
#            for l in rl:
#                sys.__stdout__.write(l)
            
    def renderProcedure(self, width, height, doShadows, doGlow, camera, options):
        log.debug("renderProcedure")
        self.removeLogFile()
        print "renderProcedure", width, height, doShadows, doGlow, camera, options
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        
        if pm.about(batch=True):
            pm.mayatoCorona()
        else:
            pm.mayatoCorona(width=width, height=height, camera=camera)
            
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
        pm.mayatoCorona(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoCorona(stopIpr=True)
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
    return CoronaRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Corona")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Corona FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

