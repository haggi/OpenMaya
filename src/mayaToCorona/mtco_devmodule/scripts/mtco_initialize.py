import pymel.core as pm
import logging
import traceback
import sys
import os
import path
import Renderer as Renderer
import optimizeTextures
import Corona.aeNodeTemplates as aet

reload(Renderer)

log = logging.getLogger("mtCoronaLogger")


RENDERER_NAME = "Corona"

class CoronaRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg=None):
        if not CoronaRenderer.theRendererInstance:
            CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , __name__)
            # CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , "mtco_initialize")
        return CoronaRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        
    
    def getEnumList(self, attr):
        return [(i, v) for i, v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy=None):
        print "UpdateTest", dummy             

    def updateEnvironment(self, dummy=None):
        pass

    def CoronaCommonGlobalsCreateTab(self):
        self.OpenMayaCommonGlobalsCreateTab()            

    def CoronaCommonGlobalsUpdateTab(self):
        self.OpenMayaCommonGlobalsUpdateTab()            
        
    def CoronaRendererCreateTab(self):
        print "CoronaRendererCreateTab()"
        log.debug("CoronaRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')        
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):                
                with pm.frameLayout(label="Renderer", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='renderer', uiType='enum', displayName='Renderer', default='Progressive', data='Progressive:Bucket', uiDict=uiDict, callback=self.CoronaRendererUpdateTab)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='progressive_maxPasses', uiType='int', displayName='Maxpasses', default='50', data='minmax:0:9999', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='progressive_timeLimit', uiType='int', displayName='Timelimit', default='60', uiDict=uiDict)
                        # self.addRenderGlobalsUIElement(attName='progressive_recalculateEvery', uiType='int', displayName='Recalculate Every', default='0', data='minmax:0:999', uiDict=uiDict)
                        # self.addRenderGlobalsUIElement(attName='progressive_adaptivity', uiType='float', displayName='Adaptivity', default='0.0', data='minmax:0.0:99.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='image_bucketSize', uiType='int', displayName='Tile Size', default=64, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='buckets_initialSamples', uiType='int', displayName='Initialsamples', default='1', data='minmax:1:999', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='buckets_adaptiveSteps', uiType='int', displayName='Adaptivesteps', default='2', data='minmax:1:10', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='buckets_adaptiveThreshold', uiType='float', displayName='Adaptive Threshold', default='0.03', data='minmax:0.001:1.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='vcm_mode', uiType='enum', displayName='VCM Mode', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='bidir_doMis', uiType='bool', displayName='Use MIS', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='ppm_photonsPerIter', uiType='int', displayName='Photons Per Iteration', default='5000000', data='minmax:1000:99000000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'ppm_samplesPerIter', uiType = 'int', displayName = 'Ppm_samplesperiter', default='1', data='minmax:1:50', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='ppm_initialRadius', uiType='float', displayName='Initial Radius', default='2.0', data='minmax:0.0001:200.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='ppm_alpha', uiType='float', displayName='Alpha (radius reduction)', default='0.666', data='minmax:0.01:1.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='maxPtSampleIntensity', uiType='float', displayName='Max Sample Intensity', default='0.0', data='minmax:0.0:99999.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='threads', uiType='int', displayName='Threads', default=8, uiDict=uiDict)

                with pm.frameLayout(label="Filter", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='filtertype', uiType='enum', displayName='Imagefilter', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='imagefilter_width', uiType='float', displayName='Filter Width', default='1.5', data='minmax:1.0:64', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='imagefilter_blurring', uiType='float', displayName='Blur', default='0.5', data='minmax:0.0:1.0', uiDict=uiDict)
                    
                with pm.frameLayout(label="Raytrace Features", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        # self.addRenderGlobalsUIElement(attName = 'raycaster_minDepth', uiType = 'int', displayName = 'Min Ray Depth', default='0', data='minmax:0:25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='raycaster_maxDepth', uiType='int', displayName='Max Ray Depth', default='25', data='minmax:1:25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='color_exit', uiType='color', displayName='Exit Color', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='system_maxNormalDev', uiType='float', displayName='Max Normal Diff', uiDict=uiDict)
                        with pm.frameLayout(label="Accerleration Structure", collapsable=True, collapse=True):
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName='accelerationStructure', uiType='enum', displayName='Accelerationstructure', default='Embree_bvh4_spatial', data='None:Embree_bvh4_spatial', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName='embree_triangles', uiType='enum', displayName='Triangles', default='Fast', data='Fast:Avx', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName='instance_minSize', uiType='int', displayName='Instance Min Size', default='1', data='minmax:1:999999', uiDict=uiDict)
                                    # self.addRenderGlobalsUIElement(attName = 'bvh_cost_iteration', uiType = 'float', displayName = 'Bvh_cost_iteration', default='1.0', data='minmax:0.01:1000.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'bvh_cost_triangle', uiType = 'float', displayName = 'Bvh_cost_triangle', default='1.0', data='minmax:0.01:1000.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'bvh_leafSizeMin', uiType = 'int', displayName = 'Bvh_leafsizemin', default='2', data='minmax:1:1000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'bvh_leafSizeMax', uiType = 'int', displayName = 'Bvh_leafsizemax', default='6', data='minmax:2:1000', uiDict=uiDict)

                with pm.frameLayout(label="Color Mapping", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_gamma', uiType = 'float', displayName = 'Colormapping_gamma', default='2.2', data='minmax:0.01:10.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_highlightCompression', uiType = 'float', displayName = 'Highlight Compression', default='1.0', data='minmax:0.01:99.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_colorTemperature', uiType = 'float', displayName = 'Color Temperature', default='6500.0', data='minmax:1000.0:99999.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_tint', uiType = 'color', displayName = 'Tint', default='1.0:1.0:1.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_useSimpleExposure', uiType = 'bool', displayName = 'Use Simple Exposure', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_simpleExposure', uiType = 'float', displayName = 'Simple Exposure', default='1.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_useContrast', uiType = 'bool', displayName = 'Use Contrast', default='false', uiDict=uiDict, callback=self.CoronaRendererUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'colorMapping_contrast', uiType = 'float', displayName = 'Contrast', default='1.0', data='minmax:1.0:99.0', uiDict=uiDict)


                with pm.frameLayout(label="Features", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='doShading', uiType='bool', displayName='Do Shading', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='doDof', uiType='bool', displayName='Depth of Field:', default='True', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='doMotionBlur', uiType='bool', displayName='Motion Blur:', default='True', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='xftimesamples', uiType='int', displayName='Mb Samples:', default='2', uiDict=uiDict, callback=self.CoronaRendererUpdateTab)
                        # self.addRenderGlobalsUIElement(attName = 'random_sampler', uiType = 'enum', displayName = 'Random_Sampler', default='5d_highd', data='5d_highd:Shared:Maximal_value', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'dumpAndResume', uiType = 'bool', displayName = 'Dump and Resume', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'dumpExrFile', uiType = 'string', displayName = 'Dump Exr File', default='""', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'renderstamp_use', uiType = 'bool', displayName = 'Renderstamp', default='true', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'renderstamp_inFile', uiType = 'bool', displayName = 'Save File with Stamp', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'renderStamp', uiType = 'string', displayName = 'Renderstamp', default='"corona renderer alpha | %c | time: %t | passes: %p | primitives: %o | rays/s: %r"', uiDict=uiDict)
                    

# self.addRenderGlobalsUIElement(attName='adaptiveSampling', uiType='bool', displayName='Adaptive Sampling:', default='True', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lowThreadPriority', uiType = 'bool', displayName = 'Lowthreadpriority', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'vfb_showBucketOrder', uiType = 'bool', displayName = 'Vfb_showbucketorder', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'doAa', uiType = 'bool', displayName = 'Doaa', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'enviroSolver', uiType = 'enum', displayName = 'Envirosolver', default='Fast', data='Fast:Fast_compensate', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'random_sampler', uiType = 'enum', displayName = 'Random_Sampler', default='5d_highd', data='5d_highd:Shared:Maximal_value', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lights_env_resolution', uiType = 'int', displayName = 'Lights_env_resolution', default='1000', data='minmax:1:100000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lights_areaMethod', uiType = 'enum', displayName = 'Lights Areamethod', default='Reproject', data='Simple:Reproject', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'buffer_updateInterval', uiType = 'int', displayName = 'Buffer_updateinterval', default='1000', data='minmax:0:36000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_primaryFile', uiType = 'string', displayName = 'Gi_primaryfile', default='"c:/primarygi.dat"', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'image_bucketSize', uiType = 'int', displayName = 'Image_bucketsize', default='32', data='minmax:1:1024', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'color_exit', uiType = 'color', displayName = 'Exit Color', default='0.0:0.0:1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fb_internalResolutionMult', uiType = 'int', displayName = 'Fb_internalresolutionmult', default='1', data='minmax:1:10', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'subdivEnviroThreshold', uiType = 'float', displayName = 'Subdivenvirothreshold', default='0.005', data='minmax:0.0:1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lights_texturedResolution', uiType = 'float', displayName = 'Lights_texturedresolution', default='0.3', data='minmax:0.0001:99.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'random_seed', uiType = 'int', displayName = 'Random_seed', default='1234', data='minmax:Int_min:Int_max', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lightSolver_localFrac', uiType = 'float', displayName = 'Lightsolver_localfrac', default='0.33', data='minmax:0.0:1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'lightSolver_globalFrac', uiType = 'float', displayName = 'Lightsolver_globalfrac', default='0.33', data='minmax:0.0:1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'portals_sampleAmount', uiType = 'float', displayName = 'Portals_sampleamount', default='0.75', data='minmax:0.0:1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'shadowBias', uiType = 'float', displayName = 'Shadowbias', default='-6.07', data='-8.0:-2.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'resumeRendering', uiType = 'bool', displayName = 'Resumerendering', default='false', uiDict=uiDict)
                with pm.frameLayout(label="Displacement", collapsable=True, collapse=False):
                    self.addRenderGlobalsUIElement(attName='displace_maxSubdiv', uiType='int', displayName='Displace_maxsubdiv', default='100', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='displace_useProjectionSize', uiType='bool', displayName='Use Camera Projection', default='true', uiDict=uiDict, callback=self.CoronaRendererUpdateTab)
                    self.addRenderGlobalsUIElement(attName='displace_maxProjectSize', uiType='float', displayName='Max Edgelen (Pixels)', default='2.0', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='displace_maxWorldSize', uiType='float', displayName='Max Edgelen (Worldspace)', default='1.0', uiDict=uiDict)
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.CoronaRendererUpdateTab()

    def CoronaRendererUpdateTab(self, dummy=None):
        print "Update corona renderer tab"
        self.createGlobalsNode()
        self.updateEnvironment()

        if not self.rendererTabUiDict.has_key('common'):
            return
        
        uiDict = self.rendererTabUiDict['common']    

        # progressive/bucket
        renderer = self.renderGlobalsNode.renderer.get()
        for key in uiDict:
            if key.startswith("progressive_"):                
                uiDict[key].setEnable(False)
            if key.startswith("buckets_"):                
                uiDict[key].setEnable(False)
            if key in ["image_bucketSize", "ppm_initialRadius", "ppm_photonsPerIter", "ppm_initialRadius", "ppm_alpha"]:                
                uiDict[key].setEnable(False)
            if key.startswith("vcm_"):      
                uiDict[key].setEnable(False)
              
        for key in uiDict:
            if renderer == 0:  # progressive
                if key.startswith("progressive_"):                
                    uiDict[key].setEnable(True)
            if renderer == 1:  # bucket
                if key.startswith("buckets_"):                
                    uiDict[key].setEnable(True)
                if key == "image_bucketSize":                
                    uiDict[key].setEnable(True)
            if renderer == 2:  # vcm
                if key.startswith("vcm_"):                
                    uiDict[key].setEnable(True)
                if key == "ppm_initialRadius":                
                    uiDict[key].setEnable(True)
                if key in ["ppm_initialRadius", "ppm_photonsPerIter", "ppm_initialRadius", "ppm_alpha"]:                
                    uiDict[key].setEnable(True)
        
        uiDict['displace_maxWorldSize'].setEnable(False)
        uiDict['displace_maxProjectSize'].setEnable(False)
        if self.renderGlobalsNode.displace_useProjectionSize.get():
            uiDict['displace_maxProjectSize'].setEnable(True)
        else:
            uiDict['displace_maxWorldSize'].setEnable(True)

        uiDict['colorMapping_contrast'].setEnable(False)
        if self.renderGlobalsNode.colorMapping_useContrast.get():
            uiDict['colorMapping_contrast'].setEnable(True)
        uiDict['colorMapping_contrast'].setEnable(False)
        
        uiDict['colorMapping_simpleExposure'].setEnable(False)
        if self.renderGlobalsNode.colorMapping_useSimpleExposure.get():
            uiDict['colorMapping_simpleExposure'].setEnable(True)
            
        uiDict['renderstamp_inFile'].setEnable(False)
        uiDict['renderstamp'].setEnable(False)
        if self.renderGlobalsNode.renderstamp_use.get():
            uiDict['renderstamp_inFile'].setEnable(True)
            uiDict['renderstamp'].setEnable(True)
            
            

    def CoronaGiCreateTab(self):
        log.debug("CoronaGiCreateTab()")
        tabname = "gi"
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)        
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key(tabname):
            self.rendererTabUiDict.pop(tabname)        
        uiDict = {}
        self.rendererTabUiDict[tabname] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):                
                with pm.frameLayout(label="Lights", collapsable=True, collapse=False):
                     with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName = 'pathtracingSamples', uiType = 'int', displayName = 'GI Samples Multiplier', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'lights_areaSamplesMult', uiType = 'float', displayName = 'Light Samples Multiplier', uiDict=uiDict)
#                         pass
# #                         self.addRenderGlobalsUIElement(attName = 'lights_solver', uiType = 'enum', displayName = 'Lights_solver', default='Combined', data='Combined:Photon', uiDict=uiDict)
# #                         self.addRenderGlobalsUIElement(attName = 'gi_pathtracing_directMode', uiType = 'enum', displayName = 'Sampling Mode', default='Mis', data='Sample_lights:Mis', uiDict=uiDict)
                with pm.frameLayout(label="Global Illumination", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='gi_primarySolver', uiType='enum', displayName='Primary Solver', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='gi_secondarySolver', uiType='enum', displayName='Secondary Solver', uiDict=uiDict)
                        # self.addRenderGlobalsUIElement(attName='gi_ic_hemisphereSubdiv', uiType='float', displayName='Hemi Subdiv', default='7', uiDict=uiDict, callback=self.CoronaGiUpdateTab)
#                with pm.frameLayout(label="Primary GI", collapsable=True, collapse=False):
#                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
#                        pass
                        # self.addRenderGlobalsUIElement(attName='gi_ic_hemisphereSubdiv', uiType='float', displayName='Hemi Subdiv', default='7', uiDict=uiDict, callback=self.CoronaGiUpdateTab)
                with pm.frameLayout(label="Secondary GI ", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='gi_secondaryFile', uiType='string', displayName='HD Cache File', default='"c:/secondarygi.dat"', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='gi_saveSecondary', uiType = 'bool', displayName = 'Saves HD Cache', default='false', uiDict=uiDict)
#                        self.addRenderGlobalsUIElement(attName='gi_loadSecondary', uiType = 'bool', displayName = 'Load Secondary GI', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_precompMult', uiType = 'float', displayName = 'Precom Density', default='1.0', data='minmax:0.0:99.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_interpolationCount', uiType = 'int', displayName = 'Interpolation Count', default='3', data='1:64', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_ptSamples', uiType = 'int', displayName = 'Record Quality', default='256', data='minmax:1:4096', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_smoothing', uiType = 'float', displayName = 'Smoothing', default='2.0', data='minmax:1.0:10.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_maxRecords', uiType = 'int', displayName = 'Max Records', default='100000', data='minmax:1000:999000', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_glossyThreshold', uiType = 'float', displayName = 'Glossy Threshold', default='0.9', data='minmax:0.0:1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_writePasses', uiType = 'int', displayName = 'Write # of Passes', default='0', data='minmax:0:9999999', uiDict=uiDict)
                        with pm.frameLayout(label="Interpolation accuracy ", collapsable=True, collapse=False):
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_dirSensitivity', uiType = 'float', displayName = 'Direction', default='2.0', data='minmax:0.001:100.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_posSensitivity', uiType = 'float', displayName = 'Position', default='20.0', data='minmax:0.0:100.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_normalSensitivity', uiType = 'float', displayName = 'Normal', default='3.0', data='minmax:0.0:10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_incrementalBuild', uiType = 'bool', displayName = 'Gi_ic_incrementalbuild', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_incrementalFilename', uiType = 'string', displayName = 'Gi_ic_incrementalfilename', default='"incrementalic.dat"', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_hemisphereSubdiv', uiType = 'int', displayName = 'Gi_ic_hemispheresubdiv', default='7', data='minmax:1:100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_precompAmount', uiType = 'float', displayName = 'Gi_ic_precompamount', default='1.0', data='minmax:0.0:99.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_pathtracingCorners', uiType = 'bool', displayName = 'Gi_ic_pathtracingcorners', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_maxGeomError', uiType = 'float', displayName = 'Gi_ic_maxgeomerror', default='0.6', data='minmax:0.01:10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_smoothing', uiType = 'float', displayName = 'Gi_ic_smoothing', default='1.8', data='minmax:1.0:10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_colorThreshold', uiType = 'float', displayName = 'Gi_ic_colorthreshold', default='10.0', data='minmax:0.01:10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_recordSpacingMin', uiType = 'float', displayName = 'Gi_ic_recordspacingmin', default='1.0', data='minmax:0.01:500.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_recordSpacingMax', uiType = 'float', displayName = 'Gi_ic_recordspacingmax', default='20.0', data='minmax:0.1:500.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_useRotationGradient', uiType = 'bool', displayName = 'Gi_ic_userotationgradient', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_useTranslationGradient', uiType = 'bool', displayName = 'Gi_ic_usetranslationgradient', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_interpolationSchema', uiType = 'enum', displayName = 'Gi_ic_interpolationschema', default='Tabellion', data='Ward:Tabellion', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_searchStructure', uiType = 'enum', displayName = 'Gi_ic_searchstructure', default='Multiple_octree', data='Multiple_octree:Bvh', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_relaxedInterpolation', uiType = 'bool', displayName = 'Gi_ic_relaxedinterpolation', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_vizualization', uiType = 'enum', displayName = 'Gi_ic_vizualization', default='Indirect', data='Off:Indirect', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_ic_minInterpSamples', uiType = 'int', displayName = 'Gi_ic_mininterpsamples', default='2', data='minmax:1:20', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_photons_emitted', uiType = 'int', displayName = 'Gi_photons_emitted', default='5000000', data='minmax:1000:100000000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_photons_storeDirect', uiType = 'bool', displayName = 'Gi_photons_storedirect', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_photons_depth', uiType = 'int', displayName = 'Gi_photons_depth', default='-1', data='minmax:-1:100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_photons_lookupCount', uiType = 'int', displayName = 'Gi_photons_lookupcount', default='50', data='minmax:1:4096', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_photons_0ilter', uiType = 'enum', displayName = 'Gi_photons_filter', default='Linear', data='Linear:Constant:Gaussian', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_vpl_emittedCount', uiType = 'int', displayName = 'Gi_vpl_emittedcount', default='1000000', data='minmax:1:9999999', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_vpl_usedCount', uiType = 'int', displayName = 'Gi_vpl_usedcount', default='150', data='minmax:1:9999', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_vpl_progressiveBatch', uiType = 'int', displayName = 'Gi_vpl_progressivebatch', default='150', data='minmax:1:9999', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gi_vpl_clamping', uiType = 'float', displayName = 'Gi_vpl_clamping', default='50.0', data='minmax:0.0:999.0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.CoronaGiUpdateTab()

    def CoronaGiUpdateTab(self):
        log.debug("CoronaGiUpdateTab()")


    def xmlFileBrowse(self, args=None):
        filename = pm.fileDialog2(fileMode=0, caption="Export Corona File Name")
        if len(filename) > 0:
            filename = filename[0]
            if not filename.endswith(".igs"):
                filename += ".igs"
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename)
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])

    def editSun(self, *args):
        if not self.rendererTabUiDict.has_key('environment'):
            return
        uiDict = self.rendererTabUiDict['environment']    
        if self.renderGlobalsNode.useSunLightConnection.get():
            if len(self.renderGlobalsNode.sunLightConnection.inputs()) > 0:
                # maybe check the connection?
                pass
            else:
                suns = pm.ls("CoronaSun")
                sun = None
                if len(suns) > 0:
                    sun = suns[0]
                else:
                    sun = pm.createNode("directionalLight")
                    sun = sun.getParent()
                    sun.rename("CoronaSun")
                sun.message >> self.renderGlobalsNode.sunLightConnection
                       
    def CoronaEnvironmentCreateTab(self):
        log.debug("CoronaEnvironmentCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "EnvScrollLayout"

        if self.rendererTabUiDict.has_key('environment'):
            self.rendererTabUiDict.pop('environment')        
        uiDict = {}
        self.rendererTabUiDict['environment'] = uiDict
                
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                self.addRenderGlobalsUIElement(attName = 'bgType', uiType = 'enum', displayName = 'Background', default='0', data='Color/Image:PhysicalSky', uiDict=uiDict, callback=self.CoronaEnvironmentUpdateTab)
                envLightingFrame = None
                physSkyFrame = None
                physSkyPreetham = None
                physSkyRawafake = None
                self.addRenderGlobalsUIElement(attName='useSunLightConnection', uiType='bool', displayName='Use Sun', uiDict=uiDict, callback=self.editSun)
                self.addRenderGlobalsUIElement(attName = 'sunSizeMulti', uiType = 'float', displayName = 'Sun Size Multiplier', default='1.0', uiDict=uiDict)
                with pm.frameLayout(label="Color/Image", collapsable=False) as envLightingFrame:
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='bgColor', uiType='color', displayName='Background Color', default='0.4:0.4:1.0', uiDict=uiDict)
                with pm.frameLayout(label="Physical Sky", collapsable=True, collapse=False) as physSkyFrame:
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName = 'pSkyModel', uiType = 'enum', displayName = 'Sky Model', default='0', data='Preetham:Rawafake:Hosek', uiDict=uiDict, callback=self.CoronaEnvironmentUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'pSkyMultiplier', uiType = 'float', displayName = 'Sky Multiplier', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkyHorizBlur', uiType = 'float', displayName = 'Sky Horizon Blur', default='0.1', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkyGroundColor', uiType = 'color', displayName = 'Ground Color', default='0.25:0.25:0.25', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkyAffectGround', uiType = 'bool', displayName = 'Sky Affect Ground', default='true', uiDict=uiDict)
                    with pm.columnLayout(self.rendererName + 'ColumnLayoutphysSkyPreetham', adjustableColumn=True, width=400) as physSkyPreetham:
                        self.addRenderGlobalsUIElement(attName = 'pSkyPreethamTurb', uiType = 'float', displayName = 'Turbidity', default='2.5', uiDict=uiDict)
                    with pm.columnLayout(self.rendererName + 'ColumnLayoutphysSkyRawafake', adjustableColumn=True, width=400) as physSkyRawafake:
                        self.addRenderGlobalsUIElement(attName = 'pSkySunFalloff', uiType = 'float', displayName = 'Sun Falloff', default='3.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkyZenith', uiType = 'color', displayName = 'Zenith Color', default='0.1:0.1:0.5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkyHorizon', uiType = 'color', displayName = 'Horizon Color', default='0.25:0.5:0.5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkySunGlow', uiType = 'float', displayName = 'Sun Glow', default='1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkySunSideGlow', uiType = 'float', displayName = 'Sun Side Glow', default='0.2', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'pSkySunBleed', uiType = 'float', displayName = 'Sun Bleed', default='1.0', uiDict=uiDict)
                uiDict['envLightingFrame'] = envLightingFrame
                uiDict['physSkyFrame'] = physSkyFrame
                uiDict['physSkyPreetham'] = physSkyPreetham
                uiDict['physSkyRawafake'] = physSkyRawafake
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])        
        self.CoronaEnvironmentUpdateTab()

        
    def CoronaEnvironmentUpdateTab(self):
        self.createGlobalsNode()

        if not self.rendererTabUiDict.has_key('environment'):
            return
        uiDict = self.rendererTabUiDict['environment']    

        bgType = self.renderGlobalsNode.bgType.get()
        if bgType == 0: # color/image
            uiDict['envLightingFrame'].setEnable(True)
            uiDict['physSkyFrame'].setEnable(False)
        if bgType == 1: # color/image
            uiDict['envLightingFrame'].setEnable(False)
            uiDict['physSkyFrame'].setEnable(True)
            pSkyModel = self.renderGlobalsNode.pSkyModel.get()
            
            if pSkyModel == 0: #preetham
                uiDict['physSkyPreetham'].setEnable(True)
                uiDict['physSkyRawafake'].setEnable(False)
            if pSkyModel == 1: #Rawafake
                uiDict['physSkyPreetham'].setEnable(False)
                uiDict['physSkyRawafake'].setEnable(True)
            if pSkyModel == 2: #hosek
                uiDict['physSkyPreetham'].setEnable(False)
                uiDict['physSkyRawafake'].setEnable(False)
        
    
    def CoronaTranslatorCreateTab(self):
        log.debug("CoronaTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Translator", collapsable=True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label="Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei=self.getEnumList(attr)) 

                with pm.frameLayout(label="{0} export".format(self.rendererName), collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export {0} Scene file (no rendering):".format(self.rendererName), value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index=2)
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Corona"
                    if not defaultXMLPath.dirname().exists():
                        defaultXMLPath.dirname().makedirs()
                    with pm.rowLayout(nc=3):
                        xmlDict['xmlFileText'] = pm.text(label="Export to")
                        xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index=2)
                        
                with pm.frameLayout(label="Optimize Textures", collapsable=True, collapse=False):
                    optiDict = {}
                    ui = pm.checkBoxGrp(label="Use Optimized Textures:", value1=False)
                    with pm.rowLayout(nc=3):
                        self.rendererTabUiDict['opti'] = optiDict
                        pm.text(label="OptimizedTex Dir:")
                        optiDict['optiField'] = pm.textField(text=self.renderGlobalsNode.optimizedTexturePath.get())
                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
                        pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index=2)
                
                with pm.frameLayout(label="Additional Settings", collapsable=True, collapse=False):
                    ui = pm.floatFieldGrp(label="Scene scale:", value1=1.0, numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index=2)
                         
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def CoronaTranslatorUpdateTab(self):
        log.debug("CoronaTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)

    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Environment"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Gi"))    


    def registerNodeExtensions(self):
        """Register Corona specific node extensions. e.g. camera type, diaphram_blades and others
        """
        pm.addExtension(nodeType="camera", longName="mtco_iso", attributeType="float", defaultValue=100.0)
        pm.addExtension(nodeType="camera", longName="mtco_shutterSpeed", attributeType="float", defaultValue=125.0, minValue=0.001, softMaxValue=2048.0)
        
        # exponent for sun light
        pm.addExtension(nodeType="directionalLight", longName="mtco_sun_multiplier", attributeType="float", defaultValue=1.0)
        pm.addExtension( nodeType='areaLight', longName='mtco_envPortal', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType='areaLight', longName='mtco_areaVisible', attributeType='bool', defaultValue=True) 

        # displacement shader        
        pm.addExtension(nodeType="displacementShader", longName="mtco_displacementMin", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="displacementShader", longName="mtco_displacementMax", attributeType="float", defaultValue=0.01)
        
        #bump map because the maya implementation is too strange
        pm.addExtension( nodeType='bump2d', longName='normalMap', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='bump2d', longName='normalMapR', attributeType='float', parent='normalMap' )
        pm.addExtension( nodeType='bump2d', longName='normalMapG', attributeType='float', parent='normalMap' )
        pm.addExtension( nodeType='bump2d', longName='normalMapB', attributeType='float', parent='normalMap' )        
        
        # gamma
        pm.addExtension( nodeType='file', longName='fromSRGB', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='file', longName='toSRGB', attributeType='bool', defaultValue=False) 

        #mesh
        pm.addExtension( nodeType='mesh', longName='mtco_visibleInGI', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='mesh', longName='mtco_envPortal', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType="mesh", longName="mtco_shadowCatcherMode", attributeType="enum", enumName="Off::Final:Composite", defaultValue = 0.0)

        
        pm.addExtension(nodeType="CoronaSurface", longName="mtco_mat_iesProfile", dataType="string", usedAsFilename=True)
        
        pm.addExtension( nodeType='samplerInfo', longName='outDPdu', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduR', attributeType='float', parent='outDPdu' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduG', attributeType='float', parent='outDPdu' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduB', attributeType='float', parent='outDPdu' )        
        
        pm.addExtension( nodeType='samplerInfo', longName='outDPdv', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvR', attributeType='float', parent='outDPdv' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvG', attributeType='float', parent='outDPdv' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvB', attributeType='float', parent='outDPdv' )        
            
    def removeLogFile(self):
        pass

    def showLogFile(self):
        pass
    
    def createRenderNode(self, nodeType=None):
        log.debug("createRenderNode callback for renderer {0} with node: {1}".format(self.rendererName.lower(), nodeType))
        mat = pm.shadingNode(nodeType, asShader=True)
        shadingGroup = pm.sets(renderable=True, noSurfaceShader=True, empty=True, name="{0}SG".format(mat))
        mat.outColor >> shadingGroup.surfaceShader
    
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
            # TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        
        if self.renderGlobalsNode.useOptimizedTextures.get():
            if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
                optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
                if not os.path.exists(optimizedPath):
                    optimizedPath.makedirs()
                self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
    
            # craete optimized exr textures
            optimizeTextures.preRenderOptimizeTextures(optimizedFilePath=self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AECoronaNodeTemplate(nodeName)
        
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
    rendererName = "Corona"
    aeDir = path.path(__file__).dirname() + "/" + rendererName + "/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            # log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

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

