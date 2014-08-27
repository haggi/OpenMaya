import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import Thea.aeNodeTemplates as aet
import path

reload(Renderer)

log = logging.getLogger("mtTheaLogger")


RENDERER_NAME="Thea"

class TheaRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg = None):
        if not TheaRenderer.theRendererInstance:
            TheaRenderer.theRendererInstance = TheaRenderer(RENDERER_NAME , __name__)
        return TheaRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        self.pluginBasePath = path.path(__file__).parent

    def TheaCommonGlobalsCreateTab(self):
        self.OpenMayaCommonGlobalsCreateTab()            

    def TheaCommonGlobalsUpdateTab(self):
        self.OpenMayaCommonGlobalsUpdateTab()            
        
    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy = None):
        print "UpdateTest", dummy             

    def updateEnvironment(self, dummy=None):
        pass
        #envDict = self.rendererTabUiDict['environment']
        #Constant
            
    def TheaRendererCreateTab(self):
        log.debug("TheaRendererCreateTab()")
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
                with pm.frameLayout(label="Renderer", collapsable = True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'engine', uiType = 'enum', displayName = 'Render Engine', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Threads', default=8, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'maxRenderSeconds', uiType = 'int', displayName = 'Max Time (sec)', default='0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='doDof', uiType='bool', displayName='Depth of Field:', default='True', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='doMotionBlur', uiType='bool', displayName='Motion Blur:', default='True', uiDict=uiDict)
                        
                        #self.addRenderGlobalsUIElement(attName = 'tilesize', uiType = 'int', displayName = 'Tile Size', default=64, uiDict=uiDict)
                    
# self.addRenderGlobalsUIElement(attName = 'giChannel', uiType = 'bool', displayName = 'GiChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'frameRate', uiType = 'int', displayName = 'FrameRate', default='25', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherRays', uiType = 'int', displayName = 'GatherRays', default='100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'normalChannel', uiType = 'bool', displayName = 'NormalChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'clayReflectance', uiType = 'float', displayName = 'ClayReflectance', default='0.5', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'selectedFrames', uiType = 'string', displayName = 'SelectedFrames', default='None', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoMultiply', uiType = 'bool', displayName = 'AoMultiply', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheForceInterpolation', uiType = 'bool', displayName = 'IrradianceCacheForceInterpolation', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'currentFrame', uiType = 'int', displayName = 'CurrentFrame', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fieldMaxError', uiType = 'float', displayName = 'FieldMaxError', default='0.02', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheSampleDensity', uiType = 'float', displayName = 'IrradianceCacheSampleDensity', default='0.70', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'antialiasingMaxContrast', uiType = 'float', displayName = 'AntialiasingMaxContrast', default='0.02', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'antialiasingClampLevel', uiType = 'float', displayName = 'AntialiasingClampLevel', default='1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'rayTraceDispersion', uiType = 'bool', displayName = 'RayTraceDispersion', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveRayTracingDepth', uiType = 'int', displayName = 'ProgressiveRayTracingDepth', default='4', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'rayTraceTransparencies', uiType = 'bool', displayName = 'RayTraceTransparencies', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCachePrepassSamples', uiType = 'int', displayName = 'IrradianceCachePrepassSamples', default='1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheDensityBoost', uiType = 'float', displayName = 'IrradianceCacheDensityBoost', default='1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'blurredEvaluateGI', uiType = 'bool', displayName = 'BlurredEvaluateGI', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'filmSaving', uiType = 'bool', displayName = 'FilmSaving', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'walkthroughPrepassFrames', uiType = 'int', displayName = 'WalkthroughPrepassFrames', default='1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'repaint', uiType = 'bool', displayName = 'Repaint', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'sssChannel', uiType = 'bool', displayName = 'SssChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherTraceReflections', uiType = 'bool', displayName = 'GatherTraceReflections', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherTraceGlossyReflections', uiType = 'bool', displayName = 'GatherTraceGlossyReflections', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'depthChannel', uiType = 'bool', displayName = 'DepthChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'renderFrames', uiType = 'enum', displayName = 'RenderFrames', default='RenderFramesCurrent', data='RenderFramesCurrent:RenderFramesAll:RenderFramesSelected', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherTraceRefractions', uiType = 'bool', displayName = 'GatherTraceRefractions', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'biasedGlossyTracingDepth', uiType = 'int', displayName = 'BiasedGlossyTracingDepth', default='2', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'skyAnimationEnd', uiType = 'string', displayName = 'SkyAnimationEnd', default='18:20:00', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'blurredReflections', uiType = 'bool', displayName = 'BlurredReflections', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'transparentChannel', uiType = 'bool', displayName = 'TransparentChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'alphaChannel', uiType = 'bool', displayName = 'AlphaChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoLowColor', uiType = 'color', displayName = 'AoLowColor', default='0:0:0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'displacement', uiType = 'bool', displayName = 'Displacement', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'devices', uiType = 'int', displayName = 'Devices', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'animationOutputFilename', uiType = 'string', displayName = 'AnimationOutputFilename', default='output.png', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCachePrepassOnly', uiType = 'bool', displayName = 'IrradianceCachePrepassOnly', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'network', uiType = 'enum', displayName = 'Network', default='NoNetwork', data='NoNetwork:ClientNetwork:ServerNetwork', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveClampLevel', uiType = 'float', displayName = 'ProgressiveClampLevel', default='1.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherTraceGlossyRefractions', uiType = 'bool', displayName = 'GatherTraceGlossyRefractions', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'directAdaptiveSampling', uiType = 'bool', displayName = 'DirectAdaptiveSampling', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'russianRouletteDepth', uiType = 'int', displayName = 'RussianRouletteDepth', default='4', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'causticPhotonsCaptured', uiType = 'int', displayName = 'CausticPhotonsCaptured', default='10000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'causticSharpening', uiType = 'bool', displayName = 'CausticSharpening', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherMaxError', uiType = 'float', displayName = 'GatherMaxError', default='0.04', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheMaxConstrast', uiType = 'float', displayName = 'IrradianceCacheMaxConstrast', default='0.02', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheMinPixelsDistance', uiType = 'float', displayName = 'IrradianceCacheMinPixelsDistance', default='4.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoAffectGI', uiType = 'bool', displayName = 'AoAffectGI', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoSamples', uiType = 'int', displayName = 'AoSamples', default='100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'reflectionChannel', uiType = 'bool', displayName = 'ReflectionChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveAmbientIntensity', uiType = 'float', displayName = 'ProgressiveAmbientIntensity', default='10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'minAASubdivs', uiType = 'int', displayName = 'MinAASubdivs', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'deviceMask', uiType = 'int', displayName = 'DeviceMask', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveCaustics', uiType = 'bool', displayName = 'ProgressiveCaustics', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveGlossyTracingDepth', uiType = 'int', displayName = 'ProgressiveGlossyTracingDepth', default='2', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'maxPasses', uiType = 'int', displayName = 'MaxPasses', default='10000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'animationOutputAlpha', uiType = 'bool', displayName = 'AnimationOutputAlpha', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'warmUpLevel', uiType = 'int', displayName = 'WarmUpLevel', default='2', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoChannel', uiType = 'bool', displayName = 'AoChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'rayTraceRefractions', uiType = 'bool', displayName = 'RayTraceRefractions', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'maxBlurredSubdivs', uiType = 'int', displayName = 'MaxBlurredSubdivs', default='3', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'causticEstimationPhotons', uiType = 'int', displayName = 'CausticEstimationPhotons', default='100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'volumetricScattering', uiType = 'bool', displayName = 'VolumetricScattering', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'antialiasingClampRadiance', uiType = 'bool', displayName = 'AntialiasingClampRadiance', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoDistance', uiType = 'float', displayName = 'AoDistance', default='10', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'priority', uiType = 'enum', displayName = 'Priority', default='LowPriority', data='InheritPriority:LowerPriority:LowPriority:NormalPriority:HighPriority:HighestPriority', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'skyAnimationStart', uiType = 'string', displayName = 'SkyAnimationStart', default='12:00:00', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'maxSamples', uiType = 'int', displayName = 'MaxSamples', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveClampRadiance', uiType = 'bool', displayName = 'ProgressiveClampRadiance', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'walkthroughAnimation', uiType = 'bool', displayName = 'WalkthroughAnimation', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'russianRoulette', uiType = 'bool', displayName = 'RussianRoulette', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheMaxPixelsDistance', uiType = 'float', displayName = 'IrradianceCacheMaxPixelsDistance', default='40.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'brightnessTermination', uiType = 'bool', displayName = 'BrightnessTermination', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveDiffuseTracingDepth', uiType = 'int', displayName = 'ProgressiveDiffuseTracingDepth', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveAmbientOcclusion', uiType = 'bool', displayName = 'ProgressiveAmbientOcclusion', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fieldMapping', uiType = 'bool', displayName = 'FieldMapping', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'ambientOcclusion', uiType = 'bool', displayName = 'AmbientOcclusion', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'clayRender', uiType = 'bool', displayName = 'ClayRender', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'minBlurredSubdivs', uiType = 'int', displayName = 'MinBlurredSubdivs', default='0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'finalGathering', uiType = 'bool', displayName = 'FinalGathering', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'maxAASubdivs', uiType = 'int', displayName = 'MaxAASubdivs', default='4', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'biasedSupersampling', uiType = 'bool', displayName = 'BiasedSupersampling', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCaching', uiType = 'bool', displayName = 'IrradianceCaching', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'supersampling', uiType = 'enum', displayName = 'Supersampling', default='AutoSS', data='AutoSS:NoneSS:NormalSS:HighSS', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fieldDensity', uiType = 'int', displayName = 'FieldDensity', default='100000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'imageSaving', uiType = 'bool', displayName = 'ImageSaving', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'brightnessThreshold', uiType = 'float', displayName = 'BrightnessThreshold', default='0.001', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'relight', uiType = 'bool', displayName = 'Relight', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveAmbientColor', uiType = 'color', displayName = 'ProgressiveAmbientColor', default='1:1:1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'biasedRayTracingDepth', uiType = 'int', displayName = 'BiasedRayTracingDepth', default='4', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheVisualize', uiType = 'bool', displayName = 'IrradianceCacheVisualize', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'objectIdChannel', uiType = 'bool', displayName = 'ObjectIdChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'directLighting', uiType = 'bool', displayName = 'DirectLighting', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'gatherDiffuseDepth', uiType = 'int', displayName = 'GatherDiffuseDepth', default='3', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'seed', uiType = 'int', displayName = 'Seed', default='None', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'rayTraceReflections', uiType = 'bool', displayName = 'RayTraceReflections', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'adaptiveBias', uiType = 'float', displayName = 'AdaptiveBias', default='0.25', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCacheAdaptiveDensity', uiType = 'bool', displayName = 'IrradianceCacheAdaptiveDensity', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'refractionChannel', uiType = 'bool', displayName = 'RefractionChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'subsurfaceScattering', uiType = 'bool', displayName = 'SubsurfaceScattering', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fieldForceNearestCell', uiType = 'bool', displayName = 'FieldForceNearestCell', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'materialIdChannel', uiType = 'bool', displayName = 'MaterialIdChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'serveraddress', uiType = 'string', displayName = 'Serveraddress', default='127.0.0.1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'networkCooperation', uiType = 'enum', displayName = 'NetworkCooperation', default='FrameCooperation', data='FrameCooperation:BucketPixelCooperation', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'motionBlur', uiType = 'bool', displayName = 'MotionBlur', default='true', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'endFrame', uiType = 'int', displayName = 'EndFrame', default='100', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'caustics', uiType = 'bool', displayName = 'Caustics', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoClamp', uiType = 'bool', displayName = 'AoClamp', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceChannel', uiType = 'bool', displayName = 'IrradianceChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'sssMaxSamples', uiType = 'int', displayName = 'SssMaxSamples', default='10000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'fieldCellSize', uiType = 'int', displayName = 'FieldCellSize', default='1000', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'directChannel', uiType = 'bool', displayName = 'DirectChannel', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'progressiveAmbientDistance', uiType = 'float', displayName = 'ProgressiveAmbientDistance', default='10.0', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'irradianceCachePrepassPixels', uiType = 'int', displayName = 'IrradianceCachePrepassPixels', default='1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'directMaxError', uiType = 'float', displayName = 'DirectMaxError', default='0.02', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'walkthroughPrepassSamples', uiType = 'int', displayName = 'WalkthroughPrepassSamples', default='1', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'skyAnimation', uiType = 'bool', displayName = 'SkyAnimation', default='false', uiDict=uiDict)
# self.addRenderGlobalsUIElement(attName = 'aoHighColor', uiType = 'color', displayName = 'AoHighColor', default='1:1:1', uiDict=uiDict)

# self.addRenderGlobalsUIElement(attName = 'threads', uiType = 'int', displayName = 'Threads', default='0', uiDict=uiDict)

                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate = True)
        pm.formLayout(parentForm, edit = True, attachForm = [ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.TheaRendererUpdateTab()

    def TheaRendererUpdateTab(self, dummy = None):
        self.createGlobalsNode()
        log.debug("TheaRendererUpdateTab()")


    def TheaEnvironmentCreateTab(self):
        log.debug("TheaEnvironmentCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "EnvScrollLayout"

        if self.rendererTabUiDict.has_key('environment'):
            self.rendererTabUiDict.pop('environment')        
        uiDict = {}
        self.rendererTabUiDict['environment'] = uiDict
        cb = self.TheaEnvironmentUpdateTab
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Environment Lighting", collapsable=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'illumination', uiType = 'enum', displayName = 'Illumination', default='0', data='NoIllumination:DomeIllumination:IBLIllumination:PhysicalSkyIllumination', uiDict=uiDict, callback=cb)
                        self.addRenderGlobalsUIElement(attName='backgroundColor', uiType='color', displayName='Background Color', default='0.4:0.4:1.0', uiDict=uiDict)
                    
                with pm.frameLayout(label="Physical Sky", collapsable=False) as uiDict['physSkyFrame']:
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'turbidity', uiType = 'float', displayName = 'Turbidity', default='2.5', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'ozone', uiType = 'float', displayName = 'Ozone', default='0.35', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'waterVapor', uiType = 'float', displayName = 'Water Vapour', default='2.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'turbidityCoefficient', uiType = 'float', displayName = 'Turbidity Coeff', default='0.046', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'wavelengthExponent', uiType = 'float', displayName = 'Wavelength Exponent', default='1.3', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'albedo', uiType = 'float', displayName = 'Albedo', default='0.5', uiDict=uiDict)

                with pm.frameLayout(label="Physical Sun", collapsable=False) as uiDict['physSunFrame']: 
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'sunPolarAngle', uiType = 'float', displayName = 'Sun Polar Angle', default='-1.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'sunAzimuth', uiType = 'float', displayName = 'Sun Azimuth', default='-1.0', uiDict=uiDict)
                        #self.addRenderGlobalsUIElement(attName = 'sunDirection', uiType = 'vector', displayName = 'Sun Direction', default='0:0:0', uiDict=uiDict)

                with pm.frameLayout(label="Time Zone", collapsable=False) as uiDict['timeZoneFrame']:              
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'latitude', uiType = 'float', displayName = 'Latitude', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'longitude', uiType = 'float', displayName = 'Longitude', default='0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'timezone', uiType = 'int', displayName = 'Timezone', default='0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'date', uiType = 'string', displayName = 'Date', default='"1/6/2014"', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'localtime', uiType = 'string', displayName = 'Localtime', default='"12:00:00"', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'ior', uiType = 'float', displayName = 'IOR', default='1.0', uiDict=uiDict)
                
                with pm.frameLayout(label="IBL", collapsable=False) as uiDict['iblFrame']:                   
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):                    
                        self.addRenderGlobalsUIElement(attName = 'illuminationMap', uiType = 'color', displayName = 'Illumination Map', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'backgroundMap', uiType = 'color', displayName = 'Background Map', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'reflectionMap', uiType = 'color', displayName = 'Reflection Map', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'RefractionMap', uiType = 'color', displayName = 'Refraction Map', default='0:0:0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'medium', uiType = 'message', displayName = 'Medium', default='', uiDict=uiDict)

#                     self.addRenderGlobalsUIElement(attName='useSunLightConnection', uiType='bool', displayName='Use Sun', uiDict=uiDict)
#                     buttonLabel = "Create Sun"
#                     suns = pm.ls("CoronaSun")
#                     if len(suns) > 0:
#                         buttonLabel = "Delete Sun"
#                     uiDict['sunButton'] = pm.button(label=buttonLabel, command=self.editSun)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.TheaEnvironmentUpdateTab()

    def TheaEnvironmentUpdateTab(self):
        log.debug("TheaEnvironmentUpdateTab()")
        self.createGlobalsNode()
        uiDict = self.rendererTabUiDict['environment']
        envType = self.renderGlobalsNode.illumination.get()
        uiDict['backgroundColor'].setEnable(False)
        
        uiDict['physSkyFrame'].setEnable(False)
        uiDict['physSunFrame'].setEnable(False)
        uiDict['timeZoneFrame'].setEnable(False)
        uiDict['iblFrame'].setEnable(False)
        
        if envType == 1: #dome light
            uiDict['backgroundColor'].setEnable(True)
        elif envType == 2: #ibl lighting
            uiDict['iblFrame'].setEnable(True)
        elif envType == 3: #sun light
            uiDict['physSkyFrame'].setEnable(True)
            uiDict['physSunFrame'].setEnable(True)
            uiDict['timeZoneFrame'].setEnable(True)

            # check for connected directional light as sun light
            sun = self.renderGlobalsNode.sunLightConnection.inputs()
            if len(sun) == 0:
                sunLight = pm.createNode("directionalLight")
                sunLight.getParent().rename("TheaSun")
                sunLight.getParent().message >> self.renderGlobalsNode.sunLightConnection
                
                
    
    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Environment"))    

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
        
    def TheaTranslatorCreateTab(self):
        log.debug("TheaTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query = True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate = True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness = 0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn = True, width = 400):
                with pm.frameLayout(label="Translator", collapsable = True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label = "Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei = self.getEnumList(attr)) 
                with pm.frameLayout(label="Thea XML export", collapsable = True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1 = False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index = 2 )
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width = 120):
                        pm.text(label="XMLFileName:", width = 60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Thea"
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

    def TheaTranslatorUpdateTab(self):
        log.debug("TheaTranslatorUpdateTab()")

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)


    def registerNodeExtensions(self):
        """Register Thea specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        pm.addExtension(nodeType="camera", longName="mtth_camera_projection", attributeType="enum", enumName="Perspective:Parallel:Spherical:Cylindrical", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtth_diaphragm_blades", attributeType="long", defaultValue = 5)
        pm.addExtension(nodeType="camera", longName="mtth_diaphragm_type", attributeType="enum", enumName="Circular:Polygonal", defaultValue = 0.0)
        pm.addExtension(nodeType="camera", longName="mtth_shutter_speed", attributeType="float", defaultValue = 250.0)
        pm.addExtension(nodeType="camera", longName="mtth_focusRange", attributeType="float", defaultValue = 0.1)
        pm.addExtension(nodeType="camera", longName="mtth_autoFocus", attributeType="bool", defaultValue = False)
        
        # mesh
        #pm.addExtension(nodeType="mesh", longName="mtth_mesh_useassembly", attributeType="bool", defaultValue = False)

        # file
        pm.addExtension(nodeType="file", longName="mtth_file_iblIntensity", attributeType="float", defaultValue = 1.0)
        pm.addExtension(nodeType="file", longName="mtth_file_iblRotation", attributeType="float", defaultValue = 0.0)
        pm.addExtension(nodeType="file", longName="mtth_file_iblWrapping", attributeType="enum", enumName="CenterWrapping:TileWrapping:FitWrapping:AngularProbeWrapping:HemisphericalWrapping:SphericalWrapping", defaultValue = 3.0)
        pm.addExtension(nodeType="file", longName="mtth_file_interpolation", attributeType="enum", enumName="NoInterpolation:BilinearInterpolation:TrilinearInterpolation", defaultValue = 1.0)

        
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
                
    def createRenderNode(self, nodeType=None):
        log.debug("createRenderNode callback for renderer {0} with node: {1}".format(self.rendererName.lower(), nodeType))
        if nodeType == "TheaMaterial":
            mat = pm.shadingNode("TheaMaterial", asShader=True)
            bsdf = pm.shadingNode("BasicBSDF", asShader=True)
            bsdf.outColor >> mat.bsdf
            shadingGroup = pm.sets(renderable=True, noSurfaceShader=True, empty=True, name="{0}SG".format(mat))
            mat.outColor >> shadingGroup.surfaceShader
            return
        
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
            pm.mayatoThea()
        else:
            pm.mayatoThea(width=width, height=height, camera=camera)
            
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
        pm.mayatoThea(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoThea(stopIpr=True)
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
        aet.AETheaNodeTemplate(nodeName)
        
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
    rendererName = "Thea"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            pm.mel.eval(melCommand)

def theRenderer():
    return TheaRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Thea")
        theRenderer().registerRenderer()
        loadAETemplates()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Thea FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

