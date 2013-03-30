UIList = []
tab = {}
UIList.append(tab)
tab['name'] = 'common'
tab['entries'] = []
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'subdivAA'
entry['type'] = 'int'
entry['displayName'] = 'Antialias Subdivs'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'adaptiveSteps'
entry['type'] = 'int'
entry['displayName'] = 'Adaptive Steps'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'adaptiveThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Adaptive Threshold'
entry['default'] = '0.03'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'AASampler'
entry['type'] = 'enum'
entry['displayName'] = 'Sampler'
entry['default'] = '0'
entry['addInfo'] = 'Adaptive:None'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'accStruct'
entry['type'] = 'enum'
entry['displayName'] = 'Acceleration Structure'
entry['default'] = '2'
entry['addInfo'] = 'None:BVH full SAH:Embree BVH2:Embree BVH Spatial:Embree BVH4:Embree BVH4 Spatial'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'embreeTris'
entry['type'] = 'enum'
entry['displayName'] = 'Embree Triangles'
entry['default'] = '0'
entry['addInfo'] = 'Fast:Precise:Low Memory'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'minInstSaving'
entry['type'] = 'int'
entry['displayName'] = 'Min. Inst Saving'
entry['default'] = '50000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxDepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Depth'
entry['default'] = '25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'exitColor'
entry['type'] = 'color'
entry['displayName'] = 'Ray Exit Color'
entry['default'] = '0:0:0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxNormalDiff'
entry['type'] = 'float'
entry['displayName'] = 'Max Normal Diff'
entry['default'] = '0.55'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sensorWidth'
entry['type'] = 'float'
entry['displayName'] = 'Sensor Width'
entry['default'] = '35.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bokehUse'
entry['type'] = 'bool'
entry['displayName'] = 'Use Bokeh'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bokehBlades'
entry['type'] = 'int'
entry['displayName'] = 'Blades'
entry['default'] = '6'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bokehRot'
entry['type'] = 'float'
entry['displayName'] = 'Rotation'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bokehCustomShape'
entry['type'] = 'message'
entry['displayName'] = 'Custom Shape'
entry['default'] = '-'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bokehCustomShapeRes'
entry['type'] = 'int'
entry['displayName'] = 'Custom Shape Resolution'
entry['default'] = '500'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbXform'
entry['type'] = 'bool'
entry['displayName'] = 'Transformation Blur'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbDeform'
entry['type'] = 'bool'
entry['displayName'] = 'Deformation Blur'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbDuration'
entry['type'] = 'float'
entry['displayName'] = 'Duration'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbFrameOffset'
entry['type'] = 'float'
entry['displayName'] = 'Frame Offset'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbTransformSegments'
entry['type'] = 'int'
entry['displayName'] = 'Transform Segments'
entry['default'] = '16'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mbDeformSegments'
entry['type'] = 'int'
entry['displayName'] = 'Deform Segments'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mode'
entry['type'] = 'enum'
entry['displayName'] = 'Subiv Mode'
entry['default'] = '0'
entry['addInfo'] = 'World:Camera'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'edgeLen'
entry['type'] = 'float'
entry['displayName'] = 'Edge Length'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'timeH'
entry['type'] = 'int'
entry['displayName'] = 'Hours'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'timeM'
entry['type'] = 'int'
entry['displayName'] = 'Minutes'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'timeS'
entry['type'] = 'int'
entry['displayName'] = 'Seconds'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'numberOfPasses'
entry['type'] = 'int'
entry['displayName'] = 'Number of Passes'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'adaptivity'
entry['type'] = 'float'
entry['displayName'] = 'Adaptivity'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'realcENP'
entry['type'] = 'int'
entry['displayName'] = 'Recalc Each N Pass'
entry['default'] = '999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'showVfb'
entry['type'] = 'bool'
entry['displayName'] = 'Use Corona Frame Buffer'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pixelFilter'
entry['type'] = 'enum'
entry['displayName'] = 'Pixel Filter'
entry['default'] = '1'
entry['addInfo'] = 'None:Box:Tent:Cone:Gauss'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filterSize'
entry['type'] = 'float'
entry['displayName'] = 'Filer Size'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'blurSize'
entry['type'] = 'float'
entry['displayName'] = 'Blur Size'
entry['default'] = '0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'infoStamp'
entry['type'] = 'string'
entry['displayName'] = 'Info Stamp'
entry['default'] = 'Time %t'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'multiThreading'
entry['type'] = 'bool'
entry['displayName'] = 'Use Multi Threading'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lowThreadPrio'
entry['type'] = 'bool'
entry['displayName'] = 'Low Thread Prio'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'exportSceneOnly'
entry['type'] = 'bool'
entry['displayName'] = 'Export Scene Only'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'overrideMaterial'
entry['type'] = 'string'
entry['displayName'] = 'Override Material'
entry['default'] = '""'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'allObjectsHaveSolidAlpha'
entry['type'] = 'bool'
entry['displayName'] = 'All Solid Alpha'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'doShading'
entry['type'] = 'bool'
entry['displayName'] = 'Do Shading'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filterTexture'
entry['type'] = 'bool'
entry['displayName'] = 'Filter Textures'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderer'
entry['type'] = 'enum'
entry['displayName'] = 'Renderer'
entry['default'] = '0'
entry['addInfo'] = 'Progressive:LightTracer:Bucket'
tab['entries'].append(entry)
tab = {}
UIList.append(tab)
tab['name'] = 'GI'
tab['entries'] = []
entry = {}
entry['name'] = 'primatrySolver'
entry['type'] = 'enum'
entry['displayName'] = 'Primary Solver'
entry['default'] = '0'
entry['addInfo'] = 'IrradianceCache:PathTracing:PhotonMapping:HD Cache'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'secondarySolver'
entry['type'] = 'enum'
entry['displayName'] = 'Secondary Solver'
entry['default'] = '2'
entry['addInfo'] = 'PathTracing:PhotonMapping:HD Cache'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pathTracingSamples'
entry['type'] = 'int'
entry['displayName'] = 'Path Tracing Samples'
entry['default'] = '16'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxSampleIntensity'
entry['type'] = 'float'
entry['displayName'] = 'Max Sample Intensity'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'loadSecondaryGI'
entry['type'] = 'bool'
entry['displayName'] = 'Load Secondary GI'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'saveSecondaryGI'
entry['type'] = 'bool'
entry['displayName'] = 'Save Secondary GI'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'secondaryGIFile'
entry['type'] = 'string'
entry['displayName'] = 'Secondary GI File'
entry['default'] = '""'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'interpolation'
entry['type'] = 'enum'
entry['displayName'] = 'Interpolation'
entry['default'] = '0'
entry['addInfo'] = 'Tabellion&Larmorette:Ward et. al.'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxGeoError'
entry['type'] = 'float'
entry['displayName'] = 'Max Geometric Error'
entry['default'] = '0.6'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'smoothing'
entry['type'] = 'float'
entry['displayName'] = 'Smoothing'
entry['default'] = '1.8'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorThreshold'
entry['type'] = 'float'
entry['displayName'] = 'ColorThreshold'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxInterpSamples'
entry['type'] = 'int'
entry['displayName'] = 'Max Interpolation Samples'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gradientRot'
entry['type'] = 'bool'
entry['displayName'] = 'Gradient Rotation'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gradientTrans'
entry['type'] = 'bool'
entry['displayName'] = 'Gradient Translation'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'relaxedInterp'
entry['type'] = 'bool'
entry['displayName'] = 'Relaxed Interpolation'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'hemisphericalSubd'
entry['type'] = 'int'
entry['displayName'] = 'Hemispherical Subdivs'
entry['default'] = '7'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'validityMin'
entry['type'] = 'float'
entry['displayName'] = 'Validity Min'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'validityMax'
entry['type'] = 'float'
entry['displayName'] = 'Validity Max'
entry['default'] = '20.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'finalRes'
entry['type'] = 'int'
entry['displayName'] = 'Final Resolution'
entry['default'] = '-1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'iterations'
entry['type'] = 'int'
entry['displayName'] = 'Iterations'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'usePathTraceCorners'
entry['type'] = 'bool'
entry['displayName'] = 'Use Path Tracing Corners'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'incrementalBuild'
entry['type'] = 'bool'
entry['displayName'] = 'Incremental Build'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'previz'
entry['type'] = 'enum'
entry['displayName'] = 'Previz'
entry['default'] = '0'
entry['addInfo'] = 'Indirect Lighting:Dots:Off'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'serachStruct'
entry['type'] = 'enum'
entry['displayName'] = 'Seracrh Struct'
entry['default'] = '0'
entry['addInfo'] = 'MultiRef Octree:MultiRef KD Tree:BVH'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'storeDirectLight'
entry['type'] = 'bool'
entry['displayName'] = 'Store Direct Lighting'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'emittedPhotons'
entry['type'] = 'int'
entry['displayName'] = 'Emitted Photons'
entry['default'] = '5000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lookUpCount'
entry['type'] = 'int'
entry['displayName'] = 'Lookup Count'
entry['default'] = '50'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'photonStoreDirectLight'
entry['type'] = 'bool'
entry['displayName'] = 'Photon Store Direct'
entry['default'] = 'true'
tab['entries'].append(entry)
tab = {}
UIList.append(tab)
tab['name'] = 'lights'
tab['entries'] = []
entry = {}
entry['name'] = 'lightSolver'
entry['type'] = 'enum'
entry['displayName'] = 'Light Solver'
entry['default'] = '3'
entry['addInfo'] = 'Simple:Globally Adaptive:Locally Adaptive:Combined'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSamplingMode'
entry['type'] = 'enum'
entry['displayName'] = 'Light SamplingMode'
entry['default'] = '0'
entry['addInfo'] = 'MIS Both:BSDF Sampling:Off'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSamplesMultiplier'
entry['type'] = 'float'
entry['displayName'] = 'Light Samples Multiplier'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSubdivThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Light Subdiv Threshold'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightTextureRes'
entry['type'] = 'float'
entry['displayName'] = 'Light Texture Resolution'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSpotThresh'
entry['type'] = 'float'
entry['displayName'] = 'Light Spot Threshold'
entry['default'] = '100.0'
tab['entries'].append(entry)
tab = {}
UIList.append(tab)
tab['name'] = 'env'
tab['entries'] = []
entry = {}
entry['name'] = 'useCorona'
entry['type'] = 'bool'
entry['displayName'] = 'Use Corona'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directVisibleOverride'
entry['type'] = 'color'
entry['displayName'] = 'Direct Visible Override'
entry['default'] = '0:0:0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'reflectionsOverride'
entry['type'] = 'color'
entry['displayName'] = 'Reflections Override'
entry['default'] = '0:0:0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'refractionsOverride'
entry['type'] = 'color'
entry['displayName'] = 'Refractions Override'
entry['default'] = '0:0:0'
tab['entries'].append(entry)
tab = {}
UIList.append(tab)
tab['name'] = 'post'
tab['entries'] = []
entry = {}
entry['name'] = 'sceneWhitepoint'
entry['type'] = 'float'
entry['displayName'] = 'Scene Whitepoint[K]'
entry['default'] = '6500.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displayWhitepoint'
entry['type'] = 'float'
entry['displayName'] = 'Display Whitepoint[K]'
entry['default'] = '6500.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'catColorSpace'
entry['type'] = 'enum'
entry['displayName'] = 'Cat Color Space'
entry['default'] = '0'
entry['addInfo'] = 'RGB:None'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorChannelOutput'
entry['type'] = 'color'
entry['displayName'] = 'ColorChannelOutput'
entry['default'] = '1:1:1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'intensityMult'
entry['type'] = 'float'
entry['displayName'] = 'Intensity Multiplier'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'highLightBurn'
entry['type'] = 'float'
entry['displayName'] = 'Hightlight Burn'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gamma'
entry['type'] = 'float'
entry['displayName'] = 'Gamma'
entry['default'] = '2.2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'contrast'
entry['type'] = 'float'
entry['displayName'] = 'Contrast'
entry['default'] = '1.0'
tab['entries'].append(entry)
