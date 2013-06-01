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
entry['name'] = 'renderer'
entry['type'] = 'enum'
entry['displayName'] = 'Renderer'
entry['default'] = '0'
entry['addInfo'] = 'sampler:hybridsampler:hybridsppm'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'hsConfigFile'
entry['type'] = 'string'
entry['displayName'] = 'HSConfig'
entry['default'] = '""'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'hsOclPlatformIndex'
entry['type'] = 'int'
entry['displayName'] = 'OCL Platform Index'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'hsOclGpuUse'
entry['type'] = 'bool'
entry['displayName'] = 'OCL Gpu Use'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'hsOclWGroupSize'
entry['type'] = 'int'
entry['displayName'] = 'OCL WorkgroupSize'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sampler'
entry['type'] = 'enum'
entry['displayName'] = 'Sampler'
entry['default'] = '0'
entry['addInfo'] = 'random:low discrepancy:metropolis'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'noiseaware'
entry['type'] = 'bool'
entry['displayName'] = 'Noise Aware'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pixelSampler'
entry['type'] = 'enum'
entry['displayName'] = 'Pixel Sampler'
entry['default'] = '2'
entry['addInfo'] = 'hilbert:linear:vegas:lowdiscrepancy:tile:random'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'numSamples'
entry['type'] = 'int'
entry['displayName'] = 'Samples'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxconsecrejects'
entry['type'] = 'int'
entry['displayName'] = 'Max Consec Rejects'
entry['default'] = '512'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'largemutationprob'
entry['type'] = 'float'
entry['displayName'] = 'Large Mutation Probability'
entry['default'] = '0.4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mutationrange'
entry['type'] = 'float'
entry['displayName'] = 'Mutation Range'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'usevariance'
entry['type'] = 'bool'
entry['displayName'] = 'Use Variance'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'usecooldown'
entry['type'] = 'bool'
entry['displayName'] = 'Use Cooldown'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'initSamples'
entry['type'] = 'int'
entry['displayName'] = 'Init Samples'
entry['default'] = '100000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'chainLength'
entry['type'] = 'int'
entry['displayName'] = 'Chain Length'
entry['default'] = '2000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mutationRange'
entry['type'] = 'float'
entry['displayName'] = 'Mutation Range'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'usePhysicalSky'
entry['type'] = 'bool'
entry['displayName'] = 'Use Physical Sky'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'physicalSun'
entry['type'] = 'bool'
entry['displayName'] = 'Use Physical Sun'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'physicalSunConnection'
entry['type'] = 'message'
entry['displayName'] = 'Physical Sun connection'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sunGain'
entry['type'] = 'float'
entry['displayName'] = 'Sun Gain'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'turbidity'
entry['type'] = 'float'
entry['displayName'] = 'Turbidity'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'skySamples'
entry['type'] = 'int'
entry['displayName'] = 'Sky Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sunRelSize'
entry['type'] = 'float'
entry['displayName'] = 'Sun Relative Size'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'imageFormat'
entry['type'] = 'enum'
entry['displayName'] = 'Image Format'
entry['default'] = '1'
entry['addInfo'] = 'Exr:Png:Tga'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'premultiplyAlpha'
entry['type'] = 'bool'
entry['displayName'] = 'Premultiply Alpha'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'haltspp'
entry['type'] = 'int'
entry['displayName'] = 'Halt Samples PP'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'halttime'
entry['type'] = 'int'
entry['displayName'] = 'Max Rendertime'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'uiupdateinterval'
entry['type'] = 'float'
entry['displayName'] = 'Update Interval'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pixelfilter'
entry['type'] = 'enum'
entry['displayName'] = 'Pixel Filter'
entry['default'] = '3'
entry['addInfo'] = 'box:trianlge:gauss:mitchell:sinc'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filterWidth'
entry['type'] = 'float'
entry['displayName'] = 'Width'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filterHeight'
entry['type'] = 'float'
entry['displayName'] = 'Height'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'filterAlpha'
entry['type'] = 'float'
entry['displayName'] = 'Alpha'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'B'
entry['type'] = 'float'
entry['displayName'] = 'MitchellB'
entry['default'] = '.33'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'C'
entry['type'] = 'float'
entry['displayName'] = 'MitchellC'
entry['default'] = '.33'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'mSupersample'
entry['type'] = 'bool'
entry['displayName'] = 'Supersample'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sincTau'
entry['type'] = 'float'
entry['displayName'] = 'Tau'
entry['default'] = '3.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'accelerator'
entry['type'] = 'enum'
entry['displayName'] = 'Accelerator'
entry['default'] = '0'
entry['addInfo'] = 'kdtree:qbvh:grid (not thread-safe):unsafekdtree (not thread-safe):bvh (not thread-safe):none'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'kdIntersectcost'
entry['type'] = 'int'
entry['displayName'] = 'IntersectorCost'
entry['default'] = '80'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'kdTraversalcost'
entry['type'] = 'int'
entry['displayName'] = 'TraversalCost'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'kdEmptybonus'
entry['type'] = 'float'
entry['displayName'] = 'Empty Bonus'
entry['default'] = '0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'kdMaxprims'
entry['type'] = 'int'
entry['displayName'] = 'Max Prims'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'kdMaxdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Depth'
entry['default'] = '-1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxprimsperleaf'
entry['type'] = 'int'
entry['displayName'] = 'Max Prims Per Leaf'
entry['default'] = '4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'fullsweepthreshold'
entry['type'] = 'int'
entry['displayName'] = 'Full Sweep Thresh'
entry['default'] = '4 * 4'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'skipfactor'
entry['type'] = 'int'
entry['displayName'] = 'Skip Factor'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'refineimmediately'
entry['type'] = 'bool'
entry['displayName'] = 'Refine Immediately'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'treetype'
entry['type'] = 'enum'
entry['displayName'] = 'Tree Type'
entry['default'] = '0'
entry['addInfo'] = 'binary:quadtree:octree'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'costsamples'
entry['type'] = 'int'
entry['displayName'] = 'Cost Samples'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'surfaceIntegrator'
entry['type'] = 'enum'
entry['displayName'] = 'Surface Integrator'
entry['default'] = '0'
entry['addInfo'] = 'bidirectional (default):path:exphotonmap:directlighting:igi:distributedpath:sppm'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightStrategy'
entry['type'] = 'enum'
entry['displayName'] = 'Light Strategy'
entry['default'] = '2'
entry['addInfo'] = 'one:all:auto:importance:powerimp:allpowerimp:logpowerimp'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'shadowraycount'
entry['type'] = 'int'
entry['displayName'] = 'Shadow Rays'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'eyedepth'
entry['type'] = 'int'
entry['displayName'] = 'Eye Depth'
entry['default'] = '8'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightdepth'
entry['type'] = 'int'
entry['displayName'] = 'Light Depth'
entry['default'] = '8'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'eyerrthreshold'
entry['type'] = 'float'
entry['displayName'] = 'Eye RR Thresh'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lightrrthreshold'
entry['type'] = 'float'
entry['displayName'] = 'Light RR Thresh'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pathMaxdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Depth'
entry['default'] = '16'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rrstrategy'
entry['type'] = 'enum'
entry['displayName'] = 'RR Strategy'
entry['default'] = '2'
entry['addInfo'] = 'none:probability:efficiency'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'rrcontinueprob'
entry['type'] = 'float'
entry['displayName'] = 'RR Continue'
entry['default'] = '.65'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'includeenvironment'
entry['type'] = 'bool'
entry['displayName'] = 'Include Env'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directlightsampling'
entry['type'] = 'bool'
entry['displayName'] = 'Direct Light Sampling'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'dlightMaxdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Depth'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phRenderingmode'
entry['type'] = 'enum'
entry['displayName'] = 'Rendering Mode'
entry['default'] = '0'
entry['addInfo'] = 'directlighting:path'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phCausticphotons'
entry['type'] = 'int'
entry['displayName'] = 'Caustic Photons'
entry['default'] = '20000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phIndirectphotons'
entry['type'] = 'int'
entry['displayName'] = 'Indirect Photons'
entry['default'] = '200000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phDirectphotons'
entry['type'] = 'int'
entry['displayName'] = 'Direct Photons'
entry['default'] = '200000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phRadiancephotons'
entry['type'] = 'int'
entry['displayName'] = 'Radiance Photons'
entry['default'] = '200000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phNphotonsused'
entry['type'] = 'int'
entry['displayName'] = 'Irradiance Photons'
entry['default'] = '50'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phMaxphotondist'
entry['type'] = 'float'
entry['displayName'] = 'Max Photon Dist'
entry['default'] = '0.5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phMaxdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Specular Bounces'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phMaxphotondepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Trace Depth'
entry['default'] = '10'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phFinalgather'
entry['type'] = 'bool'
entry['displayName'] = 'Use Direct Lighing'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phFinalgathersamples'
entry['type'] = 'int'
entry['displayName'] = 'Final Gather Samples'
entry['default'] = '32'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phGatherangle'
entry['type'] = 'float'
entry['displayName'] = 'Gather Angle'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phRrstrategy'
entry['type'] = 'enum'
entry['displayName'] = 'RR Strategy'
entry['default'] = '2'
entry['addInfo'] = 'none:probability:efficiency'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phRrcontinueprob'
entry['type'] = 'float'
entry['displayName'] = 'Cont Prob RR'
entry['default'] = '0.65'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phDistancethreshold'
entry['type'] = 'float'
entry['displayName'] = 'Dist Thresold'
entry['default'] = '1.25'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'phPhotonmapsfile'
entry['type'] = 'string'
entry['displayName'] = 'Photon File'
entry['default'] = '""'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'renderingmode'
entry['type'] = 'enum'
entry['displayName'] = 'Rendering Mode'
entry['default'] = '0'
entry['addInfo'] = 'directlighting:path'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'strategy'
entry['type'] = 'enum'
entry['displayName'] = 'Strategy'
entry['default'] = '0'
entry['addInfo'] = 'auto:all:one'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directsampleall'
entry['type'] = 'bool'
entry['displayName'] = 'Include diffuse direct'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directsamples'
entry['type'] = 'int'
entry['displayName'] = 'Directlight Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'indirectsampleall'
entry['type'] = 'bool'
entry['displayName'] = 'Use Diffuse Indirect'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'indirectsamples'
entry['type'] = 'int'
entry['displayName'] = 'Num Indirect light Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffusereflectdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Diffuse Reflection'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffusereflectsamples'
entry['type'] = 'int'
entry['displayName'] = 'Num Reflection Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffuserefractdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Diffuse Refraction'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffuserefractsamples'
entry['type'] = 'int'
entry['displayName'] = 'Num Refraction Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directdiffuse'
entry['type'] = 'bool'
entry['displayName'] = 'Use Diffuse Direct'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'indirectdiffuse'
entry['type'] = 'bool'
entry['displayName'] = 'Include Diffuse Indirect'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyreflectdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Glossy Reflection'
entry['default'] = '2'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyreflectsamples'
entry['type'] = 'int'
entry['displayName'] = 'Num Glossy Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyrefractdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Glossy Refraction'
entry['default'] = '5'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyrefractsamples'
entry['type'] = 'int'
entry['displayName'] = 'Num Glossy Refraction Samples'
entry['default'] = '1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'directglossy'
entry['type'] = 'bool'
entry['displayName'] = 'Include Glossy Direct Light Samples'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'indirectglossy'
entry['type'] = 'bool'
entry['displayName'] = 'Include Glossy Indirect Light Samples'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'specularreflectdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Spec Refl Depth'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'specularrefractdepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Spec Refr Depth'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffusereflectreject'
entry['type'] = 'bool'
entry['displayName'] = 'Rejection for Diffuse Reflection'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffuserefractreject'
entry['type'] = 'bool'
entry['displayName'] = 'Rejection for Diffuse Refraction'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffusereflectreject_threshold'
entry['type'] = 'int'
entry['displayName'] = 'Average Reflect Reject Threshold'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'diffuserefractreject_threshold'
entry['type'] = 'int'
entry['displayName'] = 'Average Refract Reject Threshold'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyreflectreject'
entry['type'] = 'bool'
entry['displayName'] = 'Rejection for Glossy Reflection'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyrefractreject'
entry['type'] = 'bool'
entry['displayName'] = 'Rejection for Glossy Refraction'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyreflectreject_threshold'
entry['type'] = 'int'
entry['displayName'] = 'Average Glossy Reflect Threshold'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossyrefractreject_threshold'
entry['type'] = 'int'
entry['displayName'] = 'Average Glossy Refract Threshold'
entry['default'] = '10.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'None'
entry['type'] = 'separator'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxeyedepth'
entry['type'] = 'int'
entry['displayName'] = 'Max Eye path len'
entry['default'] = '16'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'maxphotondepth'
entry['type'] = 'int'
entry['displayName'] = 'Max photon path len'
entry['default'] = '16'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'photonperpass'
entry['type'] = 'int'
entry['displayName'] = 'Photons per Pass'
entry['default'] = '1000000'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'startk'
entry['type'] = 'int'
entry['displayName'] = 'Min Photons'
entry['default'] = '0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'alpha'
entry['type'] = 'float'
entry['displayName'] = 'Decrement Tight Serach Radius'
entry['default'] = '0.7'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'glossythreshold'
entry['type'] = 'float'
entry['displayName'] = 'Maximum Store PDF'
entry['default'] = '100'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'lookupaccel'
entry['type'] = 'enum'
entry['displayName'] = 'Hitpoint Structure'
entry['default'] = '0'
entry['addInfo'] = 'hybridhashgrid:kdtree:grid:hashgrid:parallelhashgrid'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sppmpixelsampler'
entry['type'] = 'enum'
entry['displayName'] = 'Eye Pass Sampling Pattern'
entry['default'] = '0'
entry['addInfo'] = 'hilbert:linear:tile:vegas'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'photonsampler'
entry['type'] = 'enum'
entry['displayName'] = 'Photon paths Method'
entry['default'] = '0'
entry['addInfo'] = 'halton:amc'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sppmincludeenvironment'
entry['type'] = 'bool'
entry['displayName'] = 'Show Env Light Sources'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'parallelhashgridspare'
entry['type'] = 'float'
entry['displayName'] = 'Parallel Hashgrid Spare'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'startradius'
entry['type'] = 'float'
entry['displayName'] = 'Start Search Radius'
entry['default'] = '2.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sppmdirectlightsampling'
entry['type'] = 'bool'
entry['displayName'] = 'Do Direct Lighting Pass'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'useproba'
entry['type'] = 'bool'
entry['displayName'] = 'Use Probea'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'wavelengthstratification'
entry['type'] = 'int'
entry['displayName'] = 'Wavelen Passes'
entry['default'] = '8'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'debug'
entry['type'] = 'bool'
entry['displayName'] = 'Debug mode'
entry['default'] = 'false'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'storeglossy'
entry['type'] = 'bool'
entry['displayName'] = 'Storing Glossy Hit Points'
entry['default'] = 'false'
tab['entries'].append(entry)
