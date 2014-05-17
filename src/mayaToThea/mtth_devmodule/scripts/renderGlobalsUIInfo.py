UIList = []
entry = {}
entry['name'] = 'giChannel'
entry['type'] = 'bool'
entry['displayName'] = 'GiChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'frameRate'
entry['type'] = 'int'
entry['displayName'] = 'FrameRate'
entry['default'] = '25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherRays'
entry['type'] = 'int'
entry['displayName'] = 'GatherRays'
entry['default'] = '100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'normalChannel'
entry['type'] = 'bool'
entry['displayName'] = 'NormalChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'clayReflectance'
entry['type'] = 'float'
entry['displayName'] = 'ClayReflectance'
entry['default'] = '0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'selectedFrames'
entry['type'] = 'string'
entry['displayName'] = 'SelectedFrames'
entry['default'] = 'None'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoMultiply'
entry['type'] = 'bool'
entry['displayName'] = 'AoMultiply'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheForceInterpolation'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceCacheForceInterpolation'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'currentFrame'
entry['type'] = 'int'
entry['displayName'] = 'CurrentFrame'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fieldMaxError'
entry['type'] = 'float'
entry['displayName'] = 'FieldMaxError'
entry['default'] = '0.02'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheSampleDensity'
entry['type'] = 'float'
entry['displayName'] = 'IrradianceCacheSampleDensity'
entry['default'] = '0.70'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'antialiasingMaxContrast'
entry['type'] = 'float'
entry['displayName'] = 'AntialiasingMaxContrast'
entry['default'] = '0.02'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'antialiasingClampLevel'
entry['type'] = 'float'
entry['displayName'] = 'AntialiasingClampLevel'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rayTraceDispersion'
entry['type'] = 'bool'
entry['displayName'] = 'RayTraceDispersion'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveRayTracingDepth'
entry['type'] = 'int'
entry['displayName'] = 'ProgressiveRayTracingDepth'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rayTraceTransparencies'
entry['type'] = 'bool'
entry['displayName'] = 'RayTraceTransparencies'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCachePrepassSamples'
entry['type'] = 'int'
entry['displayName'] = 'IrradianceCachePrepassSamples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheDensityBoost'
entry['type'] = 'float'
entry['displayName'] = 'IrradianceCacheDensityBoost'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'blurredEvaluateGI'
entry['type'] = 'bool'
entry['displayName'] = 'BlurredEvaluateGI'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filmSaving'
entry['type'] = 'bool'
entry['displayName'] = 'FilmSaving'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'walkthroughPrepassFrames'
entry['type'] = 'int'
entry['displayName'] = 'WalkthroughPrepassFrames'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'repaint'
entry['type'] = 'bool'
entry['displayName'] = 'Repaint'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sssChannel'
entry['type'] = 'bool'
entry['displayName'] = 'SssChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherTraceReflections'
entry['type'] = 'bool'
entry['displayName'] = 'GatherTraceReflections'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherTraceGlossyReflections'
entry['type'] = 'bool'
entry['displayName'] = 'GatherTraceGlossyReflections'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'depthChannel'
entry['type'] = 'bool'
entry['displayName'] = 'DepthChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderFrames'
entry['type'] = 'enum'
entry['displayName'] = 'RenderFrames'
entry['default'] = 'RenderFramesCurrent'
entry['addInfo'] = 'RenderFramesCurrent:RenderFramesAll:RenderFramesSelected'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherTraceRefractions'
entry['type'] = 'bool'
entry['displayName'] = 'GatherTraceRefractions'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'biasedGlossyTracingDepth'
entry['type'] = 'int'
entry['displayName'] = 'BiasedGlossyTracingDepth'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'skyAnimationEnd'
entry['type'] = 'string'
entry['displayName'] = 'SkyAnimationEnd'
entry['default'] = '18:20:00'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'blurredReflections'
entry['type'] = 'bool'
entry['displayName'] = 'BlurredReflections'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'transparentChannel'
entry['type'] = 'bool'
entry['displayName'] = 'TransparentChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'alphaChannel'
entry['type'] = 'bool'
entry['displayName'] = 'AlphaChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoLowColor'
entry['type'] = 'color'
entry['displayName'] = 'AoLowColor'
entry['default'] = '0:0:0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displacement'
entry['type'] = 'bool'
entry['displayName'] = 'Displacement'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'devices'
entry['type'] = 'int'
entry['displayName'] = 'Devices'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'animationOutputFilename'
entry['type'] = 'string'
entry['displayName'] = 'AnimationOutputFilename'
entry['default'] = 'output.png'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCachePrepassOnly'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceCachePrepassOnly'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'network'
entry['type'] = 'enum'
entry['displayName'] = 'Network'
entry['default'] = 'NoNetwork'
entry['addInfo'] = 'NoNetwork:ClientNetwork:ServerNetwork'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveClampLevel'
entry['type'] = 'float'
entry['displayName'] = 'ProgressiveClampLevel'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherTraceGlossyRefractions'
entry['type'] = 'bool'
entry['displayName'] = 'GatherTraceGlossyRefractions'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directAdaptiveSampling'
entry['type'] = 'bool'
entry['displayName'] = 'DirectAdaptiveSampling'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'russianRouletteDepth'
entry['type'] = 'int'
entry['displayName'] = 'RussianRouletteDepth'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'causticPhotonsCaptured'
entry['type'] = 'int'
entry['displayName'] = 'CausticPhotonsCaptured'
entry['default'] = '10000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'threads'
entry['type'] = 'int'
entry['displayName'] = 'Threads'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'causticSharpening'
entry['type'] = 'bool'
entry['displayName'] = 'CausticSharpening'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherMaxError'
entry['type'] = 'float'
entry['displayName'] = 'GatherMaxError'
entry['default'] = '0.04'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheMaxConstrast'
entry['type'] = 'float'
entry['displayName'] = 'IrradianceCacheMaxConstrast'
entry['default'] = '0.02'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheMinPixelsDistance'
entry['type'] = 'float'
entry['displayName'] = 'IrradianceCacheMinPixelsDistance'
entry['default'] = '4.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoAffectGI'
entry['type'] = 'bool'
entry['displayName'] = 'AoAffectGI'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoSamples'
entry['type'] = 'int'
entry['displayName'] = 'AoSamples'
entry['default'] = '100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'reflectionChannel'
entry['type'] = 'bool'
entry['displayName'] = 'ReflectionChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveAmbientIntensity'
entry['type'] = 'float'
entry['displayName'] = 'ProgressiveAmbientIntensity'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'minAASubdivs'
entry['type'] = 'int'
entry['displayName'] = 'MinAASubdivs'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'deviceMask'
entry['type'] = 'int'
entry['displayName'] = 'DeviceMask'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveCaustics'
entry['type'] = 'bool'
entry['displayName'] = 'ProgressiveCaustics'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveGlossyTracingDepth'
entry['type'] = 'int'
entry['displayName'] = 'ProgressiveGlossyTracingDepth'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxPasses'
entry['type'] = 'int'
entry['displayName'] = 'MaxPasses'
entry['default'] = '10000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'animationOutputAlpha'
entry['type'] = 'bool'
entry['displayName'] = 'AnimationOutputAlpha'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'warmUpLevel'
entry['type'] = 'int'
entry['displayName'] = 'WarmUpLevel'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoChannel'
entry['type'] = 'bool'
entry['displayName'] = 'AoChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rayTraceRefractions'
entry['type'] = 'bool'
entry['displayName'] = 'RayTraceRefractions'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxBlurredSubdivs'
entry['type'] = 'int'
entry['displayName'] = 'MaxBlurredSubdivs'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'causticEstimationPhotons'
entry['type'] = 'int'
entry['displayName'] = 'CausticEstimationPhotons'
entry['default'] = '100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'volumetricScattering'
entry['type'] = 'bool'
entry['displayName'] = 'VolumetricScattering'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'antialiasingClampRadiance'
entry['type'] = 'bool'
entry['displayName'] = 'AntialiasingClampRadiance'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoDistance'
entry['type'] = 'float'
entry['displayName'] = 'AoDistance'
entry['default'] = '10'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'priority'
entry['type'] = 'enum'
entry['displayName'] = 'Priority'
entry['default'] = 'LowPriority'
entry['addInfo'] = 'InheritPriority:LowerPriority:LowPriority:NormalPriority:HighPriority:HighestPriority'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'skyAnimationStart'
entry['type'] = 'string'
entry['displayName'] = 'SkyAnimationStart'
entry['default'] = '12:00:00'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxSamples'
entry['type'] = 'int'
entry['displayName'] = 'MaxSamples'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveClampRadiance'
entry['type'] = 'bool'
entry['displayName'] = 'ProgressiveClampRadiance'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'walkthroughAnimation'
entry['type'] = 'bool'
entry['displayName'] = 'WalkthroughAnimation'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'russianRoulette'
entry['type'] = 'bool'
entry['displayName'] = 'RussianRoulette'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheMaxPixelsDistance'
entry['type'] = 'float'
entry['displayName'] = 'IrradianceCacheMaxPixelsDistance'
entry['default'] = '40.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'brightnessTermination'
entry['type'] = 'bool'
entry['displayName'] = 'BrightnessTermination'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveDiffuseTracingDepth'
entry['type'] = 'int'
entry['displayName'] = 'ProgressiveDiffuseTracingDepth'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveAmbientOcclusion'
entry['type'] = 'bool'
entry['displayName'] = 'ProgressiveAmbientOcclusion'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fieldMapping'
entry['type'] = 'bool'
entry['displayName'] = 'FieldMapping'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'ambientOcclusion'
entry['type'] = 'bool'
entry['displayName'] = 'AmbientOcclusion'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'clayRender'
entry['type'] = 'bool'
entry['displayName'] = 'ClayRender'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'minBlurredSubdivs'
entry['type'] = 'int'
entry['displayName'] = 'MinBlurredSubdivs'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'finalGathering'
entry['type'] = 'bool'
entry['displayName'] = 'FinalGathering'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxAASubdivs'
entry['type'] = 'int'
entry['displayName'] = 'MaxAASubdivs'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'biasedSupersampling'
entry['type'] = 'bool'
entry['displayName'] = 'BiasedSupersampling'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCaching'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceCaching'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'supersampling'
entry['type'] = 'enum'
entry['displayName'] = 'Supersampling'
entry['default'] = 'AutoSS'
entry['addInfo'] = 'AutoSS:NoneSS:NormalSS:HighSS'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fieldDensity'
entry['type'] = 'int'
entry['displayName'] = 'FieldDensity'
entry['default'] = '100000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'engine'
entry['type'] = 'enum'
entry['displayName'] = 'Engine'
entry['default'] = 'UnbiasedTR1'
entry['addInfo'] = 'AdaptiveBSD:UnbiasedTR1:UnbiasedTR2:ProgressiveAO:ProgressiveMC:AdaptiveAMC:PrestoAO:PrestoMC:TotalEngines'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'imageSaving'
entry['type'] = 'bool'
entry['displayName'] = 'ImageSaving'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'brightnessThreshold'
entry['type'] = 'float'
entry['displayName'] = 'BrightnessThreshold'
entry['default'] = '0.001'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'relight'
entry['type'] = 'bool'
entry['displayName'] = 'Relight'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveAmbientColor'
entry['type'] = 'color'
entry['displayName'] = 'ProgressiveAmbientColor'
entry['default'] = '1:1:1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'biasedRayTracingDepth'
entry['type'] = 'int'
entry['displayName'] = 'BiasedRayTracingDepth'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheVisualize'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceCacheVisualize'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'objectIdChannel'
entry['type'] = 'bool'
entry['displayName'] = 'ObjectIdChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directLighting'
entry['type'] = 'bool'
entry['displayName'] = 'DirectLighting'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gatherDiffuseDepth'
entry['type'] = 'int'
entry['displayName'] = 'GatherDiffuseDepth'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'seed'
entry['type'] = 'int'
entry['displayName'] = 'Seed'
entry['default'] = 'None'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rayTraceReflections'
entry['type'] = 'bool'
entry['displayName'] = 'RayTraceReflections'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'adaptiveBias'
entry['type'] = 'float'
entry['displayName'] = 'AdaptiveBias'
entry['default'] = '0.25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCacheAdaptiveDensity'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceCacheAdaptiveDensity'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'refractionChannel'
entry['type'] = 'bool'
entry['displayName'] = 'RefractionChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'subsurfaceScattering'
entry['type'] = 'bool'
entry['displayName'] = 'SubsurfaceScattering'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fieldForceNearestCell'
entry['type'] = 'bool'
entry['displayName'] = 'FieldForceNearestCell'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'materialIdChannel'
entry['type'] = 'bool'
entry['displayName'] = 'MaterialIdChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'serveraddress'
entry['type'] = 'string'
entry['displayName'] = 'Serveraddress'
entry['default'] = '127.0.0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'networkCooperation'
entry['type'] = 'enum'
entry['displayName'] = 'NetworkCooperation'
entry['default'] = 'FrameCooperation'
entry['addInfo'] = 'FrameCooperation:BucketPixelCooperation'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'motionBlur'
entry['type'] = 'bool'
entry['displayName'] = 'MotionBlur'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'endFrame'
entry['type'] = 'int'
entry['displayName'] = 'EndFrame'
entry['default'] = '100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'caustics'
entry['type'] = 'bool'
entry['displayName'] = 'Caustics'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxRenderSeconds'
entry['type'] = 'int'
entry['displayName'] = 'MaxRenderSeconds'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoClamp'
entry['type'] = 'bool'
entry['displayName'] = 'AoClamp'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceChannel'
entry['type'] = 'bool'
entry['displayName'] = 'IrradianceChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sssMaxSamples'
entry['type'] = 'int'
entry['displayName'] = 'SssMaxSamples'
entry['default'] = '10000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fieldCellSize'
entry['type'] = 'int'
entry['displayName'] = 'FieldCellSize'
entry['default'] = '1000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directChannel'
entry['type'] = 'bool'
entry['displayName'] = 'DirectChannel'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressiveAmbientDistance'
entry['type'] = 'float'
entry['displayName'] = 'ProgressiveAmbientDistance'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'irradianceCachePrepassPixels'
entry['type'] = 'int'
entry['displayName'] = 'IrradianceCachePrepassPixels'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directMaxError'
entry['type'] = 'float'
entry['displayName'] = 'DirectMaxError'
entry['default'] = '0.02'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'walkthroughPrepassSamples'
entry['type'] = 'int'
entry['displayName'] = 'WalkthroughPrepassSamples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'skyAnimation'
entry['type'] = 'bool'
entry['displayName'] = 'SkyAnimation'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'aoHighColor'
entry['type'] = 'color'
entry['displayName'] = 'AoHighColor'
entry['default'] = '1:1:1'
tab['entries'].append(entry)
