UIList = []
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'exportOnly'
entry['type'] = 'bool'
entry['displayName'] = 'Exportonly'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_saveSecondary'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_savesecondary'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_loadSecondary'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_loadsecondary'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lowThreadPriority'
entry['type'] = 'bool'
entry['displayName'] = 'Lowthreadpriority'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'vfb_showBucketOrder'
entry['type'] = 'bool'
entry['displayName'] = 'Vfb_showbucketorder'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'doShading'
entry['type'] = 'bool'
entry['displayName'] = 'Doshading'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'doAa'
entry['type'] = 'bool'
entry['displayName'] = 'Doaa'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderer'
entry['type'] = 'enum'
entry['displayName'] = 'Renderer'
entry['default'] = 'Progressive'
entry['addInfo'] = 'Progressive:Bucket:BiDir/VCM'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'accelerationStructure'
entry['type'] = 'enum'
entry['displayName'] = 'Accelerationstructure'
entry['default'] = 'Embree_bvh4_spatial'
entry['addInfo'] = 'Embree_bvh4_spatial:Embree_bvh4:BVH full SAH'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_primarySolver'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_primarysolver'
entry['default'] = 'Pathtracing'
entry['addInfo'] = 'None:Pathtracing:Photon Map:HD Cache:VPL:Irradiance Cache'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_secondarySolver'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_secondarysolver'
entry['default'] = 'HD Cache'
entry['addInfo'] = 'None:Pathtracing:Photon Map:HD Cache:VPL:Irradiance Cache'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lights_solver'
entry['type'] = 'enum'
entry['displayName'] = 'Lights_solver'
entry['default'] = 'Combined'
entry['addInfo'] = 'Combined:Photon'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'enviroSolver'
entry['type'] = 'enum'
entry['displayName'] = 'Envirosolver'
entry['default'] = 'Fast'
entry['addInfo'] = 'Fast:Fast_compensate'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'embree_triangles'
entry['type'] = 'enum'
entry['displayName'] = 'Triangles'
entry['default'] = 'Fast'
entry['addInfo'] = 'Fast:Avx'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'random_sampler'
entry['type'] = 'enum'
entry['displayName'] = 'Random_Sampler'
entry['default'] = '5d_highd'
entry['addInfo'] = '5d_highd:Shared:Maximal_value'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressive_maxPasses'
entry['type'] = 'int'
entry['displayName'] = 'Progressive_maxpasses'
entry['default'] = '0'
entry['addInfo'] = 'minmax:0:9999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressive_timeLimit'
entry['type'] = 'int'
entry['displayName'] = 'Progressive_timelimit'
entry['default'] = '60'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lights_areaSamplesMult'
entry['type'] = 'float'
entry['displayName'] = 'Lights Areasamples Mult'
entry['default'] = '2.0'
entry['addInfo'] = 'minmax:0.0001:100.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lights_env_resolution'
entry['type'] = 'int'
entry['displayName'] = 'Lights_env_resolution'
entry['default'] = '1000'
entry['addInfo'] = 'minmax:1:100000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pathtracingSamples'
entry['type'] = 'int'
entry['displayName'] = 'Pathtracingsamples'
entry['default'] = '16'
entry['addInfo'] = 'minmax:1:100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lights_areaMethod'
entry['type'] = 'enum'
entry['displayName'] = 'Lights Areamethod'
entry['default'] = 'Reproject'
entry['addInfo'] = 'Simple:Reproject'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raycaster_maxDepth'
entry['type'] = 'int'
entry['displayName'] = 'Raycaster Maxdepth'
entry['default'] = '25'
entry['addInfo'] = 'minmax:1:25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raycaster_minDepth'
entry['type'] = 'int'
entry['displayName'] = 'Raycaster Mindepth'
entry['default'] = '0'
entry['addInfo'] = 'minmax:0:25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'buffer_updateInterval'
entry['type'] = 'int'
entry['displayName'] = 'Buffer_updateinterval'
entry['default'] = '1000'
entry['addInfo'] = 'minmax:0:36000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_primaryFile'
entry['type'] = 'string'
entry['displayName'] = 'Gi_primaryfile'
entry['default'] = '"c:/primarygi.dat"'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_secondaryFile'
entry['type'] = 'string'
entry['displayName'] = 'Gi_secondaryfile'
entry['default'] = '"c:/secondarygi.dat"'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'image_bucketSize'
entry['type'] = 'int'
entry['displayName'] = 'Image_bucketsize'
entry['default'] = '32'
entry['addInfo'] = 'minmax:1:1024'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'color_exit'
entry['type'] = 'color'
entry['displayName'] = 'Exit Color'
entry['default'] = '0.0:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fb_internalResolutionMult'
entry['type'] = 'int'
entry['displayName'] = 'Fb_internalresolutionmult'
entry['default'] = '1'
entry['addInfo'] = 'minmax:1:10'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'imagefilter_width'
entry['type'] = 'float'
entry['displayName'] = 'Imagefilter_width'
entry['default'] = '1.5'
entry['addInfo'] = 'minmax:1.0:64'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'imagefilter_blurring'
entry['type'] = 'float'
entry['displayName'] = 'Imagefilter_blurring'
entry['default'] = '0.5'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'system_maxNormalDev'
entry['type'] = 'float'
entry['displayName'] = 'System_maxnormaldev'
entry['default'] = '0.55'
entry['addInfo'] = 'minmax:0.001:2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressive_recalculateEvery'
entry['type'] = 'int'
entry['displayName'] = 'Progressive_recalculateevery'
entry['default'] = '0'
entry['addInfo'] = 'minmax:0:999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'progressive_adaptivity'
entry['type'] = 'float'
entry['displayName'] = 'Progressive_adaptivity'
entry['default'] = '0.0'
entry['addInfo'] = 'minmax:0.0:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxPtSampleIntensity'
entry['type'] = 'float'
entry['displayName'] = 'Maxptsampleintensity'
entry['default'] = '20.0'
entry['addInfo'] = 'minmax:0.0:99999.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'subdivEnviroThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Subdivenvirothreshold'
entry['default'] = '0.005'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lights_texturedResolution'
entry['type'] = 'float'
entry['displayName'] = 'Lights_texturedresolution'
entry['default'] = '0.3'
entry['addInfo'] = 'minmax:0.0001:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'random_seed'
entry['type'] = 'int'
entry['displayName'] = 'Random_seed'
entry['default'] = '1234'
entry['addInfo'] = 'minmax:Int_min:Int_max'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'numThreads'
entry['type'] = 'int'
entry['displayName'] = 'Numthreads'
entry['default'] = '0'
entry['addInfo'] = 'minmax:0:128'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSolver_localFrac'
entry['type'] = 'float'
entry['displayName'] = 'Lightsolver_localfrac'
entry['default'] = '0.33'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightSolver_globalFrac'
entry['type'] = 'float'
entry['displayName'] = 'Lightsolver_globalfrac'
entry['default'] = '0.33'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'portals_sampleAmount'
entry['type'] = 'float'
entry['displayName'] = 'Portals_sampleamount'
entry['default'] = '0.75'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'shadowBias'
entry['type'] = 'float'
entry['displayName'] = 'Shadowbias'
entry['default'] = '-6.07'
entry['addInfo'] = '-8.0:-2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'resumeRendering'
entry['type'] = 'bool'
entry['displayName'] = 'Resumerendering'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'instance_minSize'
entry['type'] = 'int'
entry['displayName'] = 'Instance_minsize'
entry['default'] = '50000'
entry['addInfo'] = 'minmax:1:999999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_incrementalBuild'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_ic_incrementalbuild'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_incrementalFilename'
entry['type'] = 'string'
entry['displayName'] = 'Gi_ic_incrementalfilename'
entry['default'] = '"incrementalic.dat"'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_hemisphereSubdiv'
entry['type'] = 'int'
entry['displayName'] = 'Gi_ic_hemispheresubdiv'
entry['default'] = '7'
entry['addInfo'] = 'minmax:1:100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_precompAmount'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_precompamount'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.0:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_pathtracingCorners'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_ic_pathtracingcorners'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_maxGeomError'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_maxgeomerror'
entry['default'] = '0.6'
entry['addInfo'] = 'minmax:0.01:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_smoothing'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_smoothing'
entry['default'] = '1.8'
entry['addInfo'] = 'minmax:1.0:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_colorThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_colorthreshold'
entry['default'] = '10.0'
entry['addInfo'] = 'minmax:0.01:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_recordSpacingMin'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_recordspacingmin'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.01:500.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_recordSpacingMax'
entry['type'] = 'float'
entry['displayName'] = 'Gi_ic_recordspacingmax'
entry['default'] = '20.0'
entry['addInfo'] = 'minmax:0.1:500.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_useRotationGradient'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_ic_userotationgradient'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_useTranslationGradient'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_ic_usetranslationgradient'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_interpolationSchema'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_ic_interpolationschema'
entry['default'] = 'Tabellion'
entry['addInfo'] = 'Ward:Tabellion'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_searchStructure'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_ic_searchstructure'
entry['default'] = 'Multiple_octree'
entry['addInfo'] = 'Multiple_octree:Bvh'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_relaxedInterpolation'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_ic_relaxedinterpolation'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_vizualization'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_ic_vizualization'
entry['default'] = 'Indirect'
entry['addInfo'] = 'Off:Indirect'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_ic_minInterpSamples'
entry['type'] = 'int'
entry['displayName'] = 'Gi_ic_mininterpsamples'
entry['default'] = '2'
entry['addInfo'] = 'minmax:1:20'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_precompMult'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_precompmult'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.0:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_interpolationCount'
entry['type'] = 'int'
entry['displayName'] = 'Gi_hdcache_interpolationcount'
entry['default'] = '3'
entry['addInfo'] = '1:64'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_dirSensitivity'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_dirsensitivity'
entry['default'] = '2.0'
entry['addInfo'] = 'minmax:0.001:100.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_posSensitivity'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_possensitivity'
entry['default'] = '20.0'
entry['addInfo'] = 'minmax:0.0:100.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_normalSensitivity'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_normalsensitivity'
entry['default'] = '3.0'
entry['addInfo'] = 'minmax:0.0:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_ptSamples'
entry['type'] = 'int'
entry['displayName'] = 'Gi_hdcache_ptsamples'
entry['default'] = '256'
entry['addInfo'] = 'minmax:1:4096'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_smoothing'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_smoothing'
entry['default'] = '2.0'
entry['addInfo'] = 'minmax:1.0:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_glossyThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Gi_hdcache_glossythreshold'
entry['default'] = '0.9'
entry['addInfo'] = 'minmax:0.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_maxRecords'
entry['type'] = 'int'
entry['displayName'] = 'Gi_hdcache_maxrecords'
entry['default'] = '100000'
entry['addInfo'] = 'minmax:1000:999000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_hdCache_writePasses'
entry['type'] = 'int'
entry['displayName'] = 'Gi_hdcache_writepasses'
entry['default'] = '0'
entry['addInfo'] = 'minmax:0:9999999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_photons_emitted'
entry['type'] = 'int'
entry['displayName'] = 'Gi_photons_emitted'
entry['default'] = '5000000'
entry['addInfo'] = 'minmax:1000:100000000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_photons_storeDirect'
entry['type'] = 'bool'
entry['displayName'] = 'Gi_photons_storedirect'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_photons_depth'
entry['type'] = 'int'
entry['displayName'] = 'Gi_photons_depth'
entry['default'] = '-1'
entry['addInfo'] = 'minmax:-1:100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_photons_lookupCount'
entry['type'] = 'int'
entry['displayName'] = 'Gi_photons_lookupcount'
entry['default'] = '50'
entry['addInfo'] = 'minmax:1:4096'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_photons_0ilter'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_photons_filter'
entry['default'] = 'Linear'
entry['addInfo'] = 'Linear:Constant:Gaussian'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_vpl_emittedCount'
entry['type'] = 'int'
entry['displayName'] = 'Gi_vpl_emittedcount'
entry['default'] = '1000000'
entry['addInfo'] = 'minmax:1:9999999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_vpl_usedCount'
entry['type'] = 'int'
entry['displayName'] = 'Gi_vpl_usedcount'
entry['default'] = '150'
entry['addInfo'] = 'minmax:1:9999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_vpl_progressiveBatch'
entry['type'] = 'int'
entry['displayName'] = 'Gi_vpl_progressivebatch'
entry['default'] = '150'
entry['addInfo'] = 'minmax:1:9999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_vpl_clamping'
entry['type'] = 'float'
entry['displayName'] = 'Gi_vpl_clamping'
entry['default'] = '50.0'
entry['addInfo'] = 'minmax:0.0:999.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'gi_pathtracing_directMode'
entry['type'] = 'enum'
entry['displayName'] = 'Gi_pathtracing_directmode'
entry['default'] = 'Mis'
entry['addInfo'] = 'Sample_lights:Mis'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'buckets_initialSamples'
entry['type'] = 'int'
entry['displayName'] = 'Buckets_initialsamples'
entry['default'] = '1'
entry['addInfo'] = 'minmax:1:999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'buckets_adaptiveSteps'
entry['type'] = 'int'
entry['displayName'] = 'Buckets_adaptivesteps'
entry['default'] = '2'
entry['addInfo'] = 'minmax:1:10'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'buckets_adaptiveThreshold'
entry['type'] = 'float'
entry['displayName'] = 'Buckets Adaptive Threshold'
entry['default'] = '0.03'
entry['addInfo'] = 'minmax:0.001:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bvh_cost_iteration'
entry['type'] = 'float'
entry['displayName'] = 'Bvh_cost_iteration'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.01:1000.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bvh_cost_triangle'
entry['type'] = 'float'
entry['displayName'] = 'Bvh_cost_triangle'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.01:1000.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bvh_leafSizeMin'
entry['type'] = 'int'
entry['displayName'] = 'Bvh_leafsizemin'
entry['default'] = '2'
entry['addInfo'] = 'minmax:1:1000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bvh_leafSizeMax'
entry['type'] = 'int'
entry['displayName'] = 'Bvh_leafsizemax'
entry['default'] = '6'
entry['addInfo'] = 'minmax:2:1000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_gamma'
entry['type'] = 'float'
entry['displayName'] = 'Colormapping_gamma'
entry['default'] = '2.2'
entry['addInfo'] = 'minmax:0.01:10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_colorTemperature'
entry['type'] = 'float'
entry['displayName'] = 'Color Temperature'
entry['default'] = '6500.0'
entry['addInfo'] = 'minmax:1000.0:99999.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_useSimpleExposure'
entry['type'] = 'bool'
entry['displayName'] = 'Use Simple Exposure'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_simpleExposure'
entry['type'] = 'float'
entry['displayName'] = 'Simple Exposure'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_tint'
entry['type'] = 'color'
entry['displayName'] = 'Tint'
entry['default'] = '1.0:1.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_useContrast'
entry['type'] = 'bool'
entry['displayName'] = 'Color Mapping Use Contrast'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_contrast'
entry['type'] = 'float'
entry['displayName'] = 'Colormapping_contrast'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:1.0:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'colorMapping_highlightCompression'
entry['type'] = 'float'
entry['displayName'] = 'Highlight Compression'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.01:99.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'ppm_samplesPerIter'
entry['type'] = 'int'
entry['displayName'] = 'Ppm_samplesperiter'
entry['default'] = '1'
entry['addInfo'] = 'minmax:1:50'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'ppm_photonsPerIter'
entry['type'] = 'int'
entry['displayName'] = 'Ppm_photonsperiter'
entry['default'] = '5000'
entry['addInfo'] = 'minmax:1000:99000000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'ppm_alpha'
entry['type'] = 'float'
entry['displayName'] = 'Ppm_alpha'
entry['default'] = '0.666'
entry['addInfo'] = 'minmax:0.01:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'ppm_initialRadius'
entry['type'] = 'float'
entry['displayName'] = 'Ppm_initialradius'
entry['default'] = '2.0'
entry['addInfo'] = 'minmax:0.0001:200.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bidir_doMis'
entry['type'] = 'bool'
entry['displayName'] = 'Bidir_domis'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'vcm_mode'
entry['type'] = 'enum'
entry['displayName'] = 'Vcm_mode'
entry['default'] = 'Bidir'
entry['addInfo'] = 'Bidir:Vcm'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displace_useProjectionSize'
entry['type'] = 'bool'
entry['displayName'] = 'Displace_useprojectionsize'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displace_maxProjectSize'
entry['type'] = 'float'
entry['displayName'] = 'Displace_maxprojectsize'
entry['default'] = '2.0'
entry['addInfo'] = 'minmax:0.01:100.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displace_maxWorldSize'
entry['type'] = 'float'
entry['displayName'] = 'Displace_maxworldsize'
entry['default'] = '1.0'
entry['addInfo'] = 'minmax:0.00001:10000000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'displace_maxSubdiv'
entry['type'] = 'int'
entry['displayName'] = 'Displace_maxsubdiv'
entry['default'] = '100'
entry['addInfo'] = 'minmax:1:9999'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderstamp_use'
entry['type'] = 'bool'
entry['displayName'] = 'Renderstamp_use'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderstamp_inFile'
entry['type'] = 'bool'
entry['displayName'] = 'Save Renderstamp'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderStamp'
entry['type'] = 'string'
entry['displayName'] = 'Renderstamp'
entry['default'] = '"corona renderer alpha | %c | time: %t | passes: %p | primitives: %o | rays/s: %r"'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bgColor'
entry['type'] = 'color'
entry['displayName'] = 'Background Color'
entry['default'] = '1.0:1.0:1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'bgType'
entry['type'] = 'enum'
entry['displayName'] = 'Background'
entry['default'] = '0'
entry['addInfo'] = 'Color/Image:PhysicalSky'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyModel'
entry['type'] = 'enum'
entry['displayName'] = 'Sky Model'
entry['default'] = '0'
entry['addInfo'] = 'Preetham:Rawafake:Hosek'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyMultiplier'
entry['type'] = 'float'
entry['displayName'] = 'Sky Multiplier'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyHorizBlur'
entry['type'] = 'float'
entry['displayName'] = 'Sky Horizon Blur'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyGroundColor'
entry['type'] = 'color'
entry['displayName'] = 'Ground Color'
entry['default'] = '0.25:0.25:0.25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyAffectGround'
entry['type'] = 'bool'
entry['displayName'] = 'Sky Affect Ground'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyPreethamTurb'
entry['type'] = 'float'
entry['displayName'] = 'Turbidity'
entry['default'] = '2.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkySunFalloff'
entry['type'] = 'float'
entry['displayName'] = 'Sun Falloff'
entry['default'] = '3.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyZenith'
entry['type'] = 'color'
entry['displayName'] = 'Zenith Color'
entry['default'] = '0.1:0.1:0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkyHorizon'
entry['type'] = 'color'
entry['displayName'] = 'Horizon Color'
entry['default'] = '0.25:0.5:0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkySunGlow'
entry['type'] = 'float'
entry['displayName'] = 'Sun Glow'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkySunSideGlow'
entry['type'] = 'float'
entry['displayName'] = 'Sun Side Glow'
entry['default'] = '0.2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pSkySunBleed'
entry['type'] = 'float'
entry['displayName'] = 'Sun Bleed'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sunSizeMulti'
entry['type'] = 'float'
entry['displayName'] = 'Sun Size Multiplier'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'dumpAndResume'
entry['type'] = 'bool'
entry['displayName'] = 'Dump and Resume'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'dumpExrFile'
entry['type'] = 'string'
entry['displayName'] = 'Dump Exr File'
entry['default'] = '""'
tab['entries'].append(entry)
