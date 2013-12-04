import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import Corona.aeNodeTemplates as aet
import path

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

    def updateEnvironment(self, dummy=None):
        pass
            
    def CoronaRendererCreateTab(self):
        log.debug("CoronaRendererCreateTab()")
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
                    self.addRenderGlobalsUIElement(attName = 'doDof', uiType = 'bool', displayName = 'Depth of Field:', default='True', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'doMotionBlur', uiType = 'bool', displayName = 'Motion Blur:', default='True', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'xftimesamples', uiType = 'int', displayName = 'Mb Samples:', default='2', uiDict=uiDict)
                    #self.addRenderGlobalsUIElement(attName = 'random_sampler', uiType = 'enum', displayName = 'Random_Sampler', default='5d_highd', data='5d_highd:Shared:Maximal_value', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'progressive_maxPasses', uiType = 'int', displayName = 'Progressive_maxpasses', default='50', data='minmax:0:9999', uiDict=uiDict)

                    
                with pm.frameLayout(label="Renderer", collapsable = True, collapse=False):
                    self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Threads', default=8, uiDict=uiDict)
                    #self.addRenderGlobalsUIElement(attName = 'tilesize', uiType = 'int', displayName = 'Tile Size', default=64, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'bgColor', uiType = 'color', displayName = 'Background Color', default='0.4:0.4:1.0', uiDict=uiDict)

#self.addRenderGlobalsUIElement(attName = 'exportOnly', uiType = 'bool', displayName = 'Exportonly', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_saveSecondary', uiType = 'bool', displayName = 'Gi_savesecondary', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_loadSecondary', uiType = 'bool', displayName = 'Gi_loadsecondary', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lowThreadPriority', uiType = 'bool', displayName = 'Lowthreadpriority', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'vfb_showBucketOrder', uiType = 'bool', displayName = 'Vfb_showbucketorder', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'doShading', uiType = 'bool', displayName = 'Doshading', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'doAa', uiType = 'bool', displayName = 'Doaa', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'renderer', uiType = 'enum', displayName = 'Renderer', default='Progressive', data='Progressive:Bucket:Ppm', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'accelerationStructure', uiType = 'enum', displayName = 'Accelerationstructure', default='Embree_bvh4_spatial', data='None:Embree_bvh4_spatial', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_primarySolver', uiType = 'enum', displayName = 'Gi_primarysolver', default='Pathtracing', data='Pathtracing:None:Irradiance_cache', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_secondarySolver', uiType = 'enum', displayName = 'Gi_secondarysolver', default='Pathtracing', data='Pathtracing:None:Vpl', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'imageFilter', uiType = 'enum', displayName = 'Imagefilter', default='None', data='None:Tent', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lights_solver', uiType = 'enum', displayName = 'Lights_solver', default='Combined', data='Combined:Photon', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'enviroSolver', uiType = 'enum', displayName = 'Envirosolver', default='Fast', data='Fast:Fast_compensate', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'embree_triangles', uiType = 'enum', displayName = 'Triangles', default='Fast', data='Fast:Avx', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'random_sampler', uiType = 'enum', displayName = 'Random_Sampler', default='5d_highd', data='5d_highd:Shared:Maximal_value', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'progressive_maxPasses', uiType = 'int', displayName = 'Progressive_maxpasses', default='0', data='minmax:0:9999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'progressive_timeLimit', uiType = 'int', displayName = 'Progressive_timelimit', default='60', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lights_areaSamplesMult', uiType = 'float', displayName = 'Lights Areasamples Mult', default='1.0', data='minmax:0.0001:100.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lights_env_resolution', uiType = 'int', displayName = 'Lights_env_resolution', default='1000', data='minmax:1:100000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'pathtracingSamples', uiType = 'int', displayName = 'Pathtracingsamples', default='16', data='minmax:1:100', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lights_areaMethod', uiType = 'enum', displayName = 'Lights Areamethod', default='Reproject', data='Simple:Reproject', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'raycaster_maxDepth', uiType = 'int', displayName = 'Raycaster Maxdepth', default='25', data='minmax:1:25', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'raycaster_minDepth', uiType = 'int', displayName = 'Raycaster Mindepth', default='0', data='minmax:0:25', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'buffer_updateInterval', uiType = 'int', displayName = 'Buffer_updateinterval', default='1000', data='minmax:0:36000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_primaryFile', uiType = 'string', displayName = 'Gi_primaryfile', default='"c:/primarygi.dat"', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_secondaryFile', uiType = 'string', displayName = 'Gi_secondaryfile', default='"c:/secondarygi.dat"', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'image_bucketSize', uiType = 'int', displayName = 'Image_bucketsize', default='32', data='minmax:1:1024', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'color_exit', uiType = 'color', displayName = 'Exit Color', default='0.0:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'fb_internalResolutionMult', uiType = 'int', displayName = 'Fb_internalresolutionmult', default='1', data='minmax:1:10', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'imagefilter_width', uiType = 'float', displayName = 'Imagefilter_width', default='1.5', data='minmax:1.0:64', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'imagefilter_blurring', uiType = 'float', displayName = 'Imagefilter_blurring', default='0.5', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'system_maxNormalDev', uiType = 'float', displayName = 'System_maxnormaldev', default='0.55', data='minmax:0.001:2.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'progressive_recalculateEvery', uiType = 'int', displayName = 'Progressive_recalculateevery', default='0', data='minmax:0:999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'progressive_adaptivity', uiType = 'float', displayName = 'Progressive_adaptivity', default='0.0', data='minmax:0.0:99.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'maxPtSampleIntensity', uiType = 'float', displayName = 'Maxptsampleintensity', default='0.0', data='minmax:0.0:99999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'subdivEnviroThreshold', uiType = 'float', displayName = 'Subdivenvirothreshold', default='0.005', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lights_texturedResolution', uiType = 'float', displayName = 'Lights_texturedresolution', default='0.3', data='minmax:0.0001:99.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'random_seed', uiType = 'int', displayName = 'Random_seed', default='1234', data='minmax:Int_min:Int_max', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lightSolver_localFrac', uiType = 'float', displayName = 'Lightsolver_localfrac', default='0.33', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'lightSolver_globalFrac', uiType = 'float', displayName = 'Lightsolver_globalfrac', default='0.33', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'portals_sampleAmount', uiType = 'float', displayName = 'Portals_sampleamount', default='0.75', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'shadowBias', uiType = 'float', displayName = 'Shadowbias', default='-6.07', data='-8.0:-2.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'resumeRendering', uiType = 'bool', displayName = 'Resumerendering', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'instance_minSize', uiType = 'int', displayName = 'Instance_minsize', default='1', data='minmax:1:999999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_incrementalBuild', uiType = 'bool', displayName = 'Gi_ic_incrementalbuild', default='false', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_incrementalFilename', uiType = 'string', displayName = 'Gi_ic_incrementalfilename', default='"incrementalic.dat"', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_hemisphereSubdiv', uiType = 'int', displayName = 'Gi_ic_hemispheresubdiv', default='7', data='minmax:1:100', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_precompAmount', uiType = 'float', displayName = 'Gi_ic_precompamount', default='1.0', data='minmax:0.0:99.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_pathtracingCorners', uiType = 'bool', displayName = 'Gi_ic_pathtracingcorners', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_maxGeomError', uiType = 'float', displayName = 'Gi_ic_maxgeomerror', default='0.6', data='minmax:0.01:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_smoothing', uiType = 'float', displayName = 'Gi_ic_smoothing', default='1.8', data='minmax:1.0:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_colorThreshold', uiType = 'float', displayName = 'Gi_ic_colorthreshold', default='10.0', data='minmax:0.01:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_recordSpacingMin', uiType = 'float', displayName = 'Gi_ic_recordspacingmin', default='1.0', data='minmax:0.01:500.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_recordSpacingMax', uiType = 'float', displayName = 'Gi_ic_recordspacingmax', default='20.0', data='minmax:0.1:500.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_useRotationGradient', uiType = 'bool', displayName = 'Gi_ic_userotationgradient', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_useTranslationGradient', uiType = 'bool', displayName = 'Gi_ic_usetranslationgradient', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_interpolationSchema', uiType = 'enum', displayName = 'Gi_ic_interpolationschema', default='Tabellion', data='Ward:Tabellion', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_searchStructure', uiType = 'enum', displayName = 'Gi_ic_searchstructure', default='Multiple_octree', data='Multiple_octree:Bvh', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_relaxedInterpolation', uiType = 'bool', displayName = 'Gi_ic_relaxedinterpolation', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_vizualization', uiType = 'enum', displayName = 'Gi_ic_vizualization', default='Indirect', data='Off:Indirect', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_ic_minInterpSamples', uiType = 'int', displayName = 'Gi_ic_mininterpsamples', default='2', data='minmax:1:20', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_precompMult', uiType = 'float', displayName = 'Gi_hdcache_precompmult', default='1.0', data='minmax:0.0:99.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_interpolationCount', uiType = 'int', displayName = 'Gi_hdcache_interpolationcount', default='3', data='1:64', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_dirSensitivity', uiType = 'float', displayName = 'Gi_hdcache_dirsensitivity', default='2.0', data='minmax:0.001:100.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_posSensitivity', uiType = 'float', displayName = 'Gi_hdcache_possensitivity', default='20.0', data='minmax:0.0:100.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_normalSensitivity', uiType = 'float', displayName = 'Gi_hdcache_normalsensitivity', default='3.0', data='minmax:0.0:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_ptSamples', uiType = 'int', displayName = 'Gi_hdcache_ptsamples', default='256', data='minmax:1:4096', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_smoothing', uiType = 'float', displayName = 'Gi_hdcache_smoothing', default='2.0', data='minmax:1.0:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_glossyThreshold', uiType = 'float', displayName = 'Gi_hdcache_glossythreshold', default='0.9', data='minmax:0.0:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_maxRecords', uiType = 'int', displayName = 'Gi_hdcache_maxrecords', default='100000', data='minmax:1000:999000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_hdCache_writePasses', uiType = 'int', displayName = 'Gi_hdcache_writepasses', default='0', data='minmax:0:9999999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_photons_emitted', uiType = 'int', displayName = 'Gi_photons_emitted', default='5000000', data='minmax:1000:100000000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_photons_storeDirect', uiType = 'bool', displayName = 'Gi_photons_storedirect', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_photons_depth', uiType = 'int', displayName = 'Gi_photons_depth', default='-1', data='minmax:-1:100', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_photons_lookupCount', uiType = 'int', displayName = 'Gi_photons_lookupcount', default='50', data='minmax:1:4096', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_photons_0ilter', uiType = 'enum', displayName = 'Gi_photons_filter', default='Linear', data='Linear:Constant:Gaussian', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_vpl_emittedCount', uiType = 'int', displayName = 'Gi_vpl_emittedcount', default='1000000', data='minmax:1:9999999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_vpl_usedCount', uiType = 'int', displayName = 'Gi_vpl_usedcount', default='150', data='minmax:1:9999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_vpl_progressiveBatch', uiType = 'int', displayName = 'Gi_vpl_progressivebatch', default='150', data='minmax:1:9999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_vpl_clamping', uiType = 'float', displayName = 'Gi_vpl_clamping', default='50.0', data='minmax:0.0:999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'gi_pathtracing_directMode', uiType = 'enum', displayName = 'Gi_pathtracing_directmode', default='Mis', data='Sample_lights:Mis', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'buckets_initialSamples', uiType = 'int', displayName = 'Buckets_initialsamples', default='1', data='minmax:1:999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'buckets_adaptiveSteps', uiType = 'int', displayName = 'Buckets_adaptivesteps', default='2', data='minmax:1:10', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'buckets_adaptiveThreshold', uiType = 'float', displayName = 'Buckets Adaptive Threshold', default='0.03', data='minmax:0.001:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'bvh_cost_iteration', uiType = 'float', displayName = 'Bvh_cost_iteration', default='1.0', data='minmax:0.01:1000.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'bvh_cost_triangle', uiType = 'float', displayName = 'Bvh_cost_triangle', default='1.0', data='minmax:0.01:1000.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'bvh_leafSizeMin', uiType = 'int', displayName = 'Bvh_leafsizemin', default='2', data='minmax:1:1000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'bvh_leafSizeMax', uiType = 'int', displayName = 'Bvh_leafsizemax', default='6', data='minmax:2:1000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_exponent', uiType = 'float', displayName = 'Colormapping_exponent', default='0.0', data='minmax:-100.0:100.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_gamma', uiType = 'float', displayName = 'Colormapping_gamma', default='2.2', data='minmax:0.1:10.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_whiteMultiplier', uiType = 'float', displayName = 'Colormapping_whitemultiplier', default='1.0', data='minmax:0.01:999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_displayTemperature', uiType = 'float', displayName = 'Colormapping_displaytemperature', default='6500.0', data='minmax:1000.0:99999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_sceneTemperature', uiType = 'float', displayName = 'Colormapping_scenetemperature', default='6500.0', data='minmax:1000.0:99999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_rBalance', uiType = 'float', displayName = 'Colormapping_rbalance', default='1.0', data='minmax:0.0:999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_gBalance', uiType = 'float', displayName = 'Colormapping_gbalance', default='1.0', data='minmax:0.0:999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_bBalance', uiType = 'float', displayName = 'Colormapping_bbalance', default='1.0', data='minmax:0.0:999.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_workingSpace', uiType = 'enum', displayName = 'Colormapping Workingspace', default='Rgb', data='Lms:Rgb', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'colorMapping_contrast', uiType = 'float', displayName = 'Colormapping_contrast', default='1.0', data='minmax:1.0:99.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'ppm_samplesPerIter', uiType = 'int', displayName = 'Ppm_samplesperiter', default='1', data='minmax:1:50', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'ppm_photonsPerIter', uiType = 'int', displayName = 'Ppm_photonsperiter', default='5000000', data='minmax:1000:99000000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'ppm_alpha', uiType = 'float', displayName = 'Ppm_alpha', default='0.666', data='minmax:0.01:1.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'ppm_initialRadius', uiType = 'float', displayName = 'Ppm_initialradius', default='2.0', data='minmax:0.0001:200.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'bidir_doMis', uiType = 'bool', displayName = 'Bidir_domis', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'vcm_mode', uiType = 'enum', displayName = 'Vcm_mode', default='Bidir', data='Bidir:Pt:Vcm', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'displace_useProjectionSize', uiType = 'bool', displayName = 'Displace_useprojectionsize', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'displace_maxProjectSize', uiType = 'float', displayName = 'Displace_maxprojectsize', default='2.0', data='minmax:0.01:100.0', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'displace_maxWorldSize', uiType = 'float', displayName = 'Displace_maxworldsize', default='1.0', data='minmax:0.00001:10000000', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'displace_maxSubdiv', uiType = 'int', displayName = 'Displace_maxsubdiv', default='100', data='minmax:1:9999', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'renderstamp_use', uiType = 'bool', displayName = 'Renderstamp_use', default='true', uiDict=uiDict)
#self.addRenderGlobalsUIElement(attName = 'renderStamp', uiType = 'string', displayName = 'Renderstamp', default='"corona renderer alpha | %c | time: %t | passes: %p | primitives: %o | rays/s: %r"', uiDict=uiDict)
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.CoronaRendererUpdateTab()

    def CoronaRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        self.updateEnvironment()
        log.debug("CoronaRendererUpdateTab()")
#        if self.renderGlobalsNode.adaptiveSampling.get():
#            self.rendererTabUiDict['minSamples'].setEnable(True)
#            self.rendererTabUiDict['maxError'].setEnable(True)
#        else:
#            self.rendererTabUiDict['minSamples'].setEnable(False)
#            self.rendererTabUiDict['maxError'].setEnable(False)

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
                with pm.frameLayout(label="Corona export (no rendering)", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="ExportFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Corona"
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

    def CoronaTranslatorUpdateTab(self):
        log.debug("CoronaTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register Corona specific node extensions. e.g. camera type, diaphram_blades and others
        """
        pm.addExtension(nodeType="camera", longName="mtco_diaphragm_blades", attributeType="long", defaultValue = 5)
        pm.addExtension(nodeType="camera", longName="mtco_diaphragm_tilt_angle", attributeType="float", defaultValue = 0.0)
        
        # exponent for sun light
        pm.addExtension(nodeType="directionalLight", longName="mtco_sun_exponent", attributeType="float", defaultValue = 4.0)
        
            
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
        
        if self.renderGlobalsNode.useOptimizedTextures.get():
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
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
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

