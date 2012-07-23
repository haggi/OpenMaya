import maya.OpenMaya as om
import mtm_attribute as attribute
reload(attribute)

filterNames = ["Unit Box Filter","Gauss", "Bartlett", "Catmull Rom", "Hanning", "Blackman", "Sinc (sharpening)", "Closest Sample Filtering", "Farthest Samples Filtering", "Disable edge Antialiasing", "Object with most coverage", "Object with most-nofilter"]
realFilterNames = ["box", "gauss", "bartlett", "catrom", "hanning", "blackman", "sinc", "minmax min", "minmax max", "minmax edge", "minmax ocover", "minmax idcover"]
textureFilterNames = ["box", "gauss", "bartlett", "catrom", "hanning", "blackman", "mitchell"]
bitDepthNames = ['8 bit', '16 bit', '16 bit float', '32 bit float']
realBitDepthNames = ['8', '16', '', 'float']
mbtypes = ["Leading Blur","Center Blur","Trailing Blur"]
renderEngines = ["Micropolygon Renderer","Raytracing","Micropolygon PBR","PBR","Photon Map Generation"]
outputTypes = ['Render To MPlay', 'Render To File']
accelTypes = ["KT Tree", "BVH"]
lightModelNames = ["phong", "blinn", "cone"]
lightTypes = ['Point', 'Line', 'Grid', 'Disk', 'Sphere', 'Geometry', 'Distant', 'Sun', 'Environment']
areaLightTypes = ['Grid', 'Line', 'Disk', 'Sphere', 'Geometry']
envMapSpace = ["Object Space", "Camera Space", "World Space", "Environment Null"]
attenuationType = ["No Attenuation", "Half Distance Attenuation", "Physically correct"]
colorSpaces = ["Linear", "Gamma 2.2"]
rayLimitBehaviours = ["Black Background", "Direct Lighting BG"]
shadowTypes = ["None", "Raytraced Shadows", "DepthMap Shadows"]

kColor = attribute.kColor
kEnum = attribute.kEnum
kMessage= attribute.kMessage
MAPTO_IGNORE = attribute.MAPTO_IGNORE
MAPTO_NONE = attribute.MAPTO_NONE

MAP_API_ATTR = attribute.MAP_API_ATTR

mantraGlobalsATList = attribute.AttributeCollection()

# Settings
mantraGlobalsATList.addAttr(an='output', dn='Output Type', tp=kEnum, values=outputTypes, default=0, kat="Settings")
mantraGlobalsATList.addAttr(an='singleFileOutput', dn='SingleFileAnim', tp=om.MFnNumericData.kBoolean, default=False, kat="Settings")
mantraGlobalsATList.addAttr(an='scaleFactor', dn='Scale Factor', tp=om.MFnNumericData.kFloat, default=0.1, kat="Settings")

# Motion Blur
mantraGlobalsATList.addAttr(an='motionblur', dn='Enable Motionblur', tp=om.MFnNumericData.kBoolean, default=False, kat="Motionblur", affectAtt=["imagemotionblur", "xftimesamples", "geotimesamples", "mbtype", 'motionfactor'])
mantraGlobalsATList.addAttr(an='imagemotionblur', dn='Allow Image Mb', tp=om.MFnNumericData.kBoolean, default=True, kat="Motionblur")
mantraGlobalsATList.addAttr(an='xftimesamples', dn='XForm Time Samples', tp=om.MFnNumericData.kInt, default=2, kat="Motionblur")
mantraGlobalsATList.addAttr(an='geotimesamples', dn='Geometry Time Samples', tp=om.MFnNumericData.kInt, default=1, kat="Motionblur")
mantraGlobalsATList.addAttr(an='motionfactor', dn='Motion Factor', tp=om.MFnNumericData.kFloat, default=0, kat="Motionblur")
mantraGlobalsATList.addAttr(an='mbtype', dn='Motionblur Type', tp=kEnum, values=mbtypes, default=1, kat="Motionblur")

# Sampling
mantraGlobalsATList.addAttr(an='dof', dn='Enable Depth of Field', tp=om.MFnNumericData.kBoolean, default=False, kat="Sampling")
mantraGlobalsATList.addAttr(an='samples', dn='Samples', tp=om.MFnNumericData.k2Int, default=[3,3], kat="Sampling")
mantraGlobalsATList.addAttr(an='samplelock', dn='Sample Lock', tp=om.MFnNumericData.kBoolean, default=True, kat="Sampling")
mantraGlobalsATList.addAttr(an='jitter', dn='Jitter', tp=om.MFnNumericData.kFloat, default=1.0, kat="Sampling")
mantraGlobalsATList.addAttr(an='noiselevel', dn='Noise Level', tp=om.MFnNumericData.kFloat, default=.05, kat="Sampling")
mantraGlobalsATList.addAttr(an='randomseed', dn='RandomSeed', tp=om.MFnNumericData.kInt, default=0, kat="Sampling")
mantraGlobalsATList.addAttr(an='rayvaryance', dn='Ray Variance Antialiasing', tp=om.MFnNumericData.kBoolean, default=False, kat="Sampling")
mantraGlobalsATList.addAttr(an='minraysamples', dn='Min Ray Samples', tp=om.MFnNumericData.kInt, default=1, kat="Sampling")
mantraGlobalsATList.addAttr(an='maxraysamples', dn='Max Ray Samples', tp=om.MFnNumericData.kInt, default=1, kat="Sampling")
mantraGlobalsATList.addAttr(an='volumestepsize', dn='Volume Step Size', tp=om.MFnNumericData.kFloat, default=.1, kat="Sampling")
mantraGlobalsATList.addAttr(an='decoupleshadowstep', dn='Decouple Shadow Step Size', tp=om.MFnNumericData.kBoolean, default=False, kat="Sampling")
mantraGlobalsATList.addAttr(an='shadowstepsize', dn='Shadow Step Size', tp=om.MFnNumericData.kFloat, default=1.0, kat="Sampling")

# Filter
mantraGlobalsATList.addAttr(an='filtertype', dn='Pixel Filter Type', tp=kEnum, values=filterNames, default=0, kat="Filter")
mantraGlobalsATList.addAttr(an='filtersize', dn='FilterSize', tp=om.MFnNumericData.k2Int, default=[3,3], kat="Filter")

# Frame Buffer
mantraGlobalsATList.addAttr(an='bitdepth', dn='BitDepth', tp=kEnum, values=bitDepthNames, default=0, kat="FrameBuffer")

# Render Engine
mantraGlobalsATList.addAttr(an='renderengine', dn='Render Engine', tp=kEnum, values=renderEngines, default=0, kat="Render Engine")
mantraGlobalsATList.addAttr(an='tilesize', dn='Tile Size', tp=om.MFnNumericData.kInt, default=16, kat="Render Engine")
mantraGlobalsATList.addAttr(an='opacitylimit', dn='Opacity Limit', tp=om.MFnNumericData.kFloat, default=.98, kat="Render Engine")
mantraGlobalsATList.addAttr(an='verbosity', dn='Verbosity', tp=om.MFnNumericData.kString, default="2a", kat="Render Engine")
mantraGlobalsATList.addAttr(an='translatorVerbosity', dn='Translator Verbosity', tp=om.MFnNumericData.kInt, default=2, kat="Render Engine")
mantraGlobalsATList.addAttr(an='usemaxproc', dn='Use Max Processors', tp=om.MFnNumericData.kBoolean, default=True, kat="Render Engine")
mantraGlobalsATList.addAttr(an='threadcount', dn='Number of Threads', tp=om.MFnNumericData.kInt, default=1, kat="Render Engine")
mantraGlobalsATList.addAttr(an='accerationtype', dn='Acceleration Type', tp=kEnum, values=accelTypes, default=0, kat="Render Engine")
mantraGlobalsATList.addAttr(an='kdmemfac', dn='KD Memory Factor', tp=om.MFnNumericData.kFloat, default=1.0, kat="Render Engine")
mantraGlobalsATList.addAttr(an='geocachesize', dn='Geometry cache Size (MB)', tp=om.MFnNumericData.kInt, default=256, kat="Render Engine")
mantraGlobalsATList.addAttr(an='texcachesize', dn='Texture cache Size (MB)', tp=om.MFnNumericData.kInt, default=256, kat="Render Engine")
mantraGlobalsATList.addAttr(an='renderviewcam', dn='Render View Cam', tp=om.MFnNumericData.kBoolean, default=True, kat="Render Engine")
mantraGlobalsATList.addAttr(an='autogenenvmap', dn='Auto generate EnvMap', tp=om.MFnNumericData.kBoolean, default=True, kat="Render Engine")
mantraGlobalsATList.addAttr(an='autogensmap', dn='Auto generate SMap', tp=om.MFnNumericData.kBoolean, default=True, kat="Render Engine")

# Shading
mantraGlobalsATList.addAttr(an='reflectlimit', dn='Reflect Limit', tp=om.MFnNumericData.kInt, default=10, kat="Shading")
mantraGlobalsATList.addAttr(an='refractlimit', dn='Refract Limit', tp=om.MFnNumericData.kInt, default=10, kat="Shading")
mantraGlobalsATList.addAttr(an='glossylimit', dn='Glossy Limit', tp=om.MFnNumericData.kInt, default=1, kat="Shading")
mantraGlobalsATList.addAttr(an='diffuselimit', dn='Diffuse Limit', tp=om.MFnNumericData.kInt, default=0, kat="Shading")
mantraGlobalsATList.addAttr(an='volumelimit', dn='Volume Limit', tp=om.MFnNumericData.kInt, default=0, kat="Shading")
mantraGlobalsATList.addAttr(an='raytracebias', dn='Raytracing Bias', tp=om.MFnNumericData.kFloat, default=.01, kat="Shading")
mantraGlobalsATList.addAttr(an='biasalongnormal', dn='Bias along Normal', tp=om.MFnNumericData.kBoolean, default=False, kat="Shading")
mantraGlobalsATList.addAttr(an='colorspace', dn='Color Space', tp=kEnum, values=colorSpaces, default=0, kat="Shading")
mantraGlobalsATList.addAttr(an='raylimit', dn='At Ray Limit Use', tp=kEnum, values=rayLimitBehaviours, default=0, kat="Shading")
mantraGlobalsATList.addAttr(an='smoothgrid', dn='Smooth Grid Colors', tp=om.MFnNumericData.kBoolean, default=True, kat="Shading")

# pbr
mantraGlobalsATList.addAttr(an='colorlimit', dn='Color Limit', tp=om.MFnNumericData.kFloat, default=10.0, kat="PBR")
mantraGlobalsATList.addAttr(an='minreflectratio', dn='Min Reflection Ratio', tp=om.MFnNumericData.kFloat, default=10.0, kat="PBR")

# paths
mantraGlobalsATList.addAttr(an='baseName', dn='Base Name', tp=om.MFnNumericData.kString, default="", kat="Paths")
mantraGlobalsATList.addAttr(an='imageDir', dn='Image Dir', tp=om.MFnNumericData.kString, default="", kat="Paths")
mantraGlobalsATList.addAttr(an='imageName', dn='Image Name', tp=om.MFnNumericData.kString, default="", kat="Paths")

# common light attributeList
mantraCommonLightATList = attribute.AttributeCollection()   
mantraCommonLightATList.addAttr(an='light_type', dn='Type', tp=kEnum, mapto=MAPTO_IGNORE, values=lightTypes, default=0, kat=None) 
mantraCommonLightATList.addAttr(an='lightcolor', dn='Color', tp=kColor, mapto='color', values=[], default=[1.0, 1.0, 1.0], kat=None)
mantraCommonLightATList.addAttr(an='lightintensity', dn='Intensity', tp=om.MFnNumericData.kFloat, mapto='intensity', default=1.0, kat=None)
mantraCommonLightATList.addAttr(an='__nondiffuse', dn='Emit Diffuse', tp=om.MFnNumericData.kBoolean, mapto='emitDiffuse', default=False, kat=None)
mantraCommonLightATList.addAttr(an='__nonspecular', dn='Emit Specular', tp=om.MFnNumericData.kBoolean, mapto='emitSpecular', default=False, kat=None)
mantraCommonLightATList.addAttr(an='samplingquality', dn='Sampling Quality', tp=om.MFnNumericData.kFloat, default=1.0, kat=None)
mantraCommonLightATList.addAttr(an='surfaceshaders', dn='Use Surface Shaders', tp=om.MFnNumericData.kBoolean, default=True, kat=None)

# Attenuation
mantraCommonLightATList.addAttr(an='attentype', dn='Attenuation Type', tp=kEnum, values=attenuationType, default=0, kat='Attenuation')
mantraCommonLightATList.addAttr(an='attendist', dn='Half Distance', tp=om.MFnNumericData.kFloat,default=10.0, kat='Attenuation')
mantraCommonLightATList.addAttr(an='attenstart', dn='Attenuation Start', tp=om.MFnNumericData.kFloat,default=10.0, kat='Attenuation')
mantraCommonLightATList.addAttr(an='activeradius', dn='Active Radius', tp=om.MFnNumericData.kFloat,default=0.0, kat='Attenuation')


# Shadow
mantraLightShadowATList = attribute.AttributeCollection() 
mantraLightShadowATList.addAttr(an='shadow_type', dn='Shadow Type', tp=kEnum, values=shadowTypes, default=1, kat='Shadow')
mantraLightShadowATList.addAttr(an='shadowI', dn='Shadow Intensity', tp=om.MFnNumericData.kFloat, default=1.0, kat='Shadow')

# pointLight AttributeList
mantraPointLightATList = attribute.AttributeCollection() 

# spotLight AttributeList
mantraSpotLightATList = attribute.AttributeCollection() 
mantraSpotLightATList.addAttr(an='light_fov', dn='Cone Angle', tp=om.MFnNumericData.kFloat, mapto='coneAngle', default=45.0)
mantraSpotLightATList.addAttr(an='projmap', dn='Projection Map', tp=om.MFnData.kString, default="")
mantraSpotLightATList.addAttr(an='near', dn='Clip Near', tp=om.MFnNumericData.kFloat, default=.1)
mantraSpotLightATList.addAttr(an='far', dn='Clip Far', tp=om.MFnNumericData.kBoolean, default=32025.0)

# directionalLight AttributeList
mantraDirectionalLightATList = attribute.AttributeCollection() 
mantraDirectionalLightATList.addAttr(an='orthowidth', dn='Ortographic Width', tp=om.MFnNumericData.kFloat, default=1.0)

# areaLight AttributeList
mantraAreaLightATList = attribute.AttributeCollection() 
mantraAreaLightATList.addAttr(an='area', dn='Area', tp=om.MFnNumericData.k2Float, default=[1.0, 1.0])
mantraAreaLightATList.addAttr(an='areashape', dn='Area Shape', values=areaLightTypes, tp=kEnum, default=0)
mantraAreaLightATList.addAttr(an='normalizearea', dn='Normalize Intensity', tp=om.MFnNumericData.kBoolean, default=True)
mantraAreaLightATList.addAttr(an='singlesided', dn='Single Sided', tp=om.MFnNumericData.kBoolean, default=False)
mantraAreaLightATList.addAttr(an='reverse', dn='Reverse Direction',tp=om.MFnNumericData.kBoolean, default=False)
mantraAreaLightATList.addAttr(an='areamap', dn='Environment Map', tp=om.MFnData.kString, default="")
mantraAreaLightATList.addAttr(an='areamapspace', dn='Env Space', tp=kEnum, values=envMapSpace, default=0)
mantraAreaLightATList.addAttr(an='light_texture', dn='Textrue Map', tp=om.MFnData.kString, default="", kat='LightTexture')
mantraAreaLightATList.addAttr(an='edgeenable', dn='Enable Texture Falloff',tp=om.MFnNumericData.kBoolean, default=False, kat='LightTexture')
mantraAreaLightATList.addAttr(an='edgewidth', dn='Edge Width', tp=om.MFnNumericData.kFloat, default=.1, kat='LightTexture')
mantraAreaLightATList.addAttr(an='edgerolloff', dn='Edge Rolloff', tp=om.MFnNumericData.kFloat, default=1.0, kat='LightTexture')
mantraAreaLightATList.addAttr(an='areageometry', dn='Object', tp=kMessage, default="", kat='GeometryLight')
mantraAreaLightATList.addAttr(an='intothisobject', dn='Transform into this obj',tp=om.MFnNumericData.kBoolean, default=False, kat='GeometryLight')


mantraLightTypeAttrList = {'areaLight':mantraAreaLightATList,
                           'pointLight':mantraPointLightATList,
                           'spotLight':mantraSpotLightATList,
                           'directionalLight':mantraDirectionalLightATList}

#    ray_property object displace opdef:/Shop/c_mantraSurface diff_int 1 sss_pcname "c_mantraSurface1_SSS.pc" ior_in 1.25300002098 ior_out 1.01800000668 refr_enable 1 refr_int 0.985000014305 refr_min 0.0120000001043 refr_clr 1 0.998799979687 0.927999973297 refr_angle 0.019999999553 refr_aniso 0.0289999991655 refr_qual 1.02999997139 refr_thin 1 atten_enable 1 atten_den 0.503000020981 atten_clr 0.931999981403 0.937666654587 1 enableDispMap 1 displacementMap "D:/mudtmp/Textures/vectorDisplacement_1_vdm.exr" ogl_alpha 1 ogl_diff 1 1 1 ogl_spec 1 1 1 ogl_specmap "" ogl_rough 0 ogl_opacitymap "" ogl_bumpmap "" ogl_normalmap "" ogl_envmap ""
#    ray_property object surface opdef:/Shop/c_mantraSurface diff_int 1 sss_pcname "c_mantraSurface1_SSS.pc" ior_in 1.25300002098 ior_out 1.01800000668 refr_enable 1 refr_int 0.985000014305 refr_min 0.0120000001043 refr_clr 1 0.998799979687 0.927999973297 refr_angle 0.019999999553 refr_aniso 0.0289999991655 refr_qual 1.02999997139 refr_thin 1 atten_enable 1 atten_den 0.503000020981 atten_clr 0.931999981403 0.937666654587 1 enableDispMap 1 displacementMap "D:/mudtmp/Textures/vectorDisplacement_1_vdm.exr" ogl_alpha 1 ogl_diff 1 1 1 ogl_spec 1 1 1 ogl_specmap "" ogl_rough 0 ogl_opacitymap "" ogl_bumpmap "" ogl_normalmap "" ogl_envmap ""

mantraMantraSurfaceShaderATList = attribute.AttributeCollection() 

mantraMantraSurfaceShaderATList.addAttr(an='diff_enable',dn='Diffuse Enable',default=True,tp=om.MFnNumericData.kBoolean,kat='Diffuse')
mantraMantraSurfaceShaderATList.addAttr(an='diff_int',dn='Diffuse Intensity',default=0.5,tp=om.MFnNumericData.kFloat,kat='Diffuse')
mantraMantraSurfaceShaderATList.addAttr(an='diff_rough',dn='Diffuse Roughness',default=0.0,tp=om.MFnNumericData.kFloat,kat='Diffuse')
mantraMantraSurfaceShaderATList.addAttr(an='baseColor',dn='Base Color',default=[0.5, 0.5, 0.5],tp=kColor,kat='Diffuse')
mantraMantraSurfaceShaderATList.addAttr(an='useColorMap',dn='Use Color Map',default=False,tp=om.MFnNumericData.kBoolean,kat='Diffuse|ColorMap')
mantraMantraSurfaceShaderATList.addAttr(an='baseColorMap',dn='Base Color Map',default="",tp=om.MFnData.kString,kat='Diffuse|ColorMap')
mantraMantraSurfaceShaderATList.addAttr(an='colorMapIntensity',dn='Color Map Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Diffuse|ColorMap')
mantraMantraSurfaceShaderATList.addAttr(an='colorMapfilter',dn='Color Map Filter',values=textureFilterNames,default=0,tp=kEnum,kat='Diffuse|ColorMap')
mantraMantraSurfaceShaderATList.addAttr(an='colorMapWidth',dn='Color Map Filter',default=1.0,tp=om.MFnNumericData.kFloat,kat='Diffuse|ColorMap')

# Subsurfacescatter
mantraMantraSurfaceShaderATList.addAttr(an='sss_enable',dn='Subsurface Scatter Enable',default=False,tp=om.MFnNumericData.kBoolean,kat='Subsurfacescatter')
mantraMantraSurfaceShaderATList.addAttr(an='sss_int',dn='Subsurface Scatter Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Subsurfacescatter')
mantraMantraSurfaceShaderATList.addAttr(an='baseSSSColor',dn='Subsurface Scatter BaseColor',default=[1.0, 1.0, 1.0],tp=kColor,kat='Subsurfacescatter')

# Specular
mantraMantraSurfaceShaderATList.addAttr(an='refl_lights',dn='Reflect Lights',default=False,tp=om.MFnNumericData.kBoolean,kat='Specular')
mantraMantraSurfaceShaderATList.addAttr(an='spec_model',dn='Specular Model',values=lightModelNames,default=0,tp=kEnum,kat='Specular')
mantraMantraSurfaceShaderATList.addAttr(an='spec_int',dn='Specular Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Specular')
mantraMantraSurfaceShaderATList.addAttr(an='specColor1',dn='Specular Color',default=[1.0, 1.0, 1.0],tp=kColor,kat='Specular')
mantraMantraSurfaceShaderATList.addAttr(an='useSpecMap1',dn='Use Specular Map',default=False,tp=om.MFnNumericData.kBoolean,kat='Specular|SpecularMap')
mantraMantraSurfaceShaderATList.addAttr(an='specMap1',dn='Specular Map',default="",tp=om.MFnData.kString,kat='Specular|SpecularMap')
mantraMantraSurfaceShaderATList.addAttr(an='reflectMapIntensity',dn='Specular Map Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Specular|SpecularMap')
mantraMantraSurfaceShaderATList.addAttr(an='specMapFilter1',dn='Spec Map Filter',values=textureFilterNames,default=0,tp=kEnum,kat='Specular|SpecularMap')
mantraMantraSurfaceShaderATList.addAttr(an='reflectMapWidth1',dn='Specular Map Width',default=1.0,tp=om.MFnNumericData.kFloat,kat='Specular|SpecularMap')
mantraMantraSurfaceShaderATList.addAttr(an='spec_angle',dn='Specular Angle',default=10.0,tp=om.MFnNumericData.kFloat,kat='Specular')
mantraMantraSurfaceShaderATList.addAttr(an='spec_aniso',dn='Specular Anisotropy',default=0.0,tp=om.MFnNumericData.kFloat,kat='Specular')

# Reflections
mantraMantraSurfaceShaderATList.addAttr(an='refl_objs',dn='Reflect Objects',default=True,tp=om.MFnNumericData.kBoolean,kat='Reflections')
mantraMantraSurfaceShaderATList.addAttr(an='refl_qual',dn='Reflection Quality',default=1.0,tp=om.MFnNumericData.kFloat,kat='Reflections')
mantraMantraSurfaceShaderATList.addAttr(an='refl_sep',dn='Seperate Objects Reflection',default=False,tp=om.MFnNumericData.kBoolean,kat='Reflections')
mantraMantraSurfaceShaderATList.addAttr(an='refl_int',dn='Reflection Intensity',default=0.5,tp=om.MFnNumericData.kFloat,kat='Reflections')
mantraMantraSurfaceShaderATList.addAttr(an='refl_clr',dn='Reflection Color',default=[1.0, 1.0, 1.0],tp=kColor,kat='Reflections')
mantraMantraSurfaceShaderATList.addAttr(an='refl_angle',dn='Reflection Angle',default=0.0,tp=om.MFnNumericData.kFloat,kat='Reflections')

# Refractions
mantraMantraSurfaceShaderATList.addAttr(an='refr_enable',dn='Refractions Enable',default=False,tp=om.MFnNumericData.kBoolean,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='ior_in',dn='IOR In',default=1.2,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='ior_out',dn='IOR out',default=1.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_model',dn='Refraction Model',values=lightModelNames,default=0,tp=kEnum,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_int',dn='Refraction Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_min',dn='Refraction Minimum',default=0.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_clr',dn='Refraction Color',default=[1.0, 1.0, 1.0],tp=kColor,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_angle',dn='Refraction Angle',default=0.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_aniso',dn='Refraction Anisotropy',default=0.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_qual',dn='Refraction Quality',default=1.0,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_lights',dn='Refract Lights',default=True,tp=om.MFnNumericData.kBoolean,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_objs',dn='Refract Objects',default=True,tp=om.MFnNumericData.kBoolean,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='refr_thin',dn='Thin Film Refractions',default=False,tp=om.MFnNumericData.kBoolean,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='atten_enable',dn='Attenuation Enable',default=False,tp=om.MFnNumericData.kBoolean,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='atten_den',dn='Attenuation Density',default=0.5,tp=om.MFnNumericData.kFloat,kat='Refractions')
mantraMantraSurfaceShaderATList.addAttr(an='atten_clr',dn='Attenuation Color',default=[1.0, 1.0, 1.0],tp=kColor,kat='Refractions')

# Emission
mantraMantraSurfaceShaderATList.addAttr(an='emit_enable',dn='Emission Enable',default=False,tp=om.MFnNumericData.kBoolean,kat='Emission')
mantraMantraSurfaceShaderATList.addAttr(an='emit_int',dn='Emission Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Emission')
mantraMantraSurfaceShaderATList.addAttr(an='emit_clr',dn='Emission Color',default=[1.0, 1.0, 1.0],tp=kColor,kat='Emission')
mantraMantraSurfaceShaderATList.addAttr(an='emit_illum',dn='Emission Illuminates Objects',default=False,tp=om.MFnNumericData.kBoolean,kat='Emission')

# Opacity
mantraMantraSurfaceShaderATList.addAttr(an='opac_int',dn='Opacity Intensity',default=1.0,tp=om.MFnNumericData.kFloat,kat='Opacity')

