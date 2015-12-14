import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):

    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
    
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

class AEuberShaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self, nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
    
    def update(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)        
        backLight = self.thisNode.Kb.get()
        dim = backLight > 0.0
        self.dimControl(nodeName, "Kb_color" , not dim)        
        self.dimControl(nodeName, "matteColor" , not self.thisNode.matteEnable.get())
    
    def checkSpecularBrdf(self, nodeName):
        specAtt = pm.Attribute(nodeName + ".specular_brdf")
        specAtt2 = pm.Attribute(nodeName + ".specular_brdf2")
        brdfValue = specAtt.get()
        brdfValue2 = specAtt2.get()
        
        dim = (brdfValue != 1)
        self.dimControl(nodeName, "specular_anisotropy" , dim)
        self.dimControl(nodeName, "specular_rotation" , dim)
        
        dim = (brdfValue2 != 1)
        self.dimControl(nodeName, "specular_anisotropy2" , dim)
        self.dimControl(nodeName, "specular_rotation2" , dim)
    
    def checkSpecularFresnel(self, nodeName):
        dimSpec1 = not self.thisNode.enable_spec1.get()
        dimSpec2 = not self.thisNode.enable_spec2.get()
        
        specAtt = pm.Attribute(nodeName + ".specular_Fresnel")
        specAtt2 = pm.Attribute(nodeName + ".specular_Fresnel2")
        iorAtt2 = pm.Attribute(nodeName + ".Fresnel_use_IOR")
        specFres = specAtt.get()
        specFres2 = specAtt2.get()
        fresIorValue = iorAtt2.get()
        
        dim = (specFres is False) or (fresIorValue is True) or dimSpec1
        pm.editorTemplate(dimControl=(nodeName, "Ksn", dim))
        dim = (specFres2 is False) or (fresIorValue is True) or dimSpec2
        pm.editorTemplate(dimControl=(nodeName, "Ksn2", dim))
        
    def checkFresnelUseIOR(self, nodeName):
        fullAttr = '%s.%s'%(nodeName, "Fresnel_use_IOR")
        fresIorValue = pm.getAttr(fullAttr)
    
        fullAttr = '%s.%s'%(nodeName, "specular_Fresnel")
        specFresValue = pm.getAttr(fullAttr)
        dim = (specFresValue is False) or (fresIorValue is True)
        pm.editorTemplate(dimControl=(nodeName, "Ksn", dim))
        
        fullAttr = '%s.%s'%(nodeName, "Fresnel")
        refFresValue = pm.getAttr(fullAttr)
        dim = (refFresValue is False) or (fresIorValue is True)
        pm.editorTemplate(dimControl=(nodeName, "Krn", dim))
    
    def updateSpec(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        dimSpec1 = not self.thisNode.enable_spec1.get()
        dimSpec2 = not self.thisNode.enable_spec2.get()
        
        if dimSpec1 or dimSpec2:
            self.dimControl(nodeName, "spec_mix" , True)
        else:
            self.dimControl(nodeName, "spec_mix" , False)
        
        
        self.dimControl(nodeName, "Ks_color" , dimSpec1)
        self.dimControl(nodeName, "Ks" , dimSpec1)
        self.dimControl(nodeName, "specular_roughness" , dimSpec1)
        self.dimControl(nodeName, "specular_brdf" , dimSpec1)
        self.dimControl(nodeName, "specular_anisotropy" , dimSpec1)
        self.dimControl(nodeName, "specular_rotation" , dimSpec1)
        self.dimControl(nodeName, "specular_Fresnel" , dimSpec1)
        self.dimControl(nodeName, "Ksn" , dimSpec1)
        self.dimControl(nodeName, "spec_enableIndirect" , dimSpec1)
        self.dimControl(nodeName, "specularFresnel" , dimSpec1)
        
        self.dimControl(nodeName, "Ks_color2" , dimSpec2)
        self.dimControl(nodeName, "Ks2" , dimSpec2)
        self.dimControl(nodeName, "specular_roughness2" , dimSpec2)
        self.dimControl(nodeName, "specular_brdf2" , dimSpec2)
        self.dimControl(nodeName, "specular_anisotropy2" , dimSpec2)
        self.dimControl(nodeName, "specular_rotation2" , dimSpec2)
        self.dimControl(nodeName, "specular_Fresnel2" , dimSpec2)
        self.dimControl(nodeName, "Ksn2" , dimSpec2)
        self.dimControl(nodeName, "spec_enableIndirect2" , dimSpec2)
        self.dimControl(nodeName, "specularFresnel2" , dimSpec2)
        
        #self.checkSpecularBrdf(nodeName)
        self.checkSpecularFresnel(nodeName)
        
    def enableSSS(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        dimSSS = not self.thisNode.enable_sss.get()
        dimSSS2 = not self.thisNode.enable_sss2.get()
        dimSSS3 = not self.thisNode.enable_sss3.get()
        
        self.dimControl(nodeName, "Ksss_color" , dimSSS)
        self.dimControl(nodeName, "Ksss" , dimSSS)
        self.dimControl(nodeName, "sss_radius" , dimSSS)
        self.dimControl(nodeName, "sssRadiusMultiplier" , dimSSS)
        
        self.dimControl(nodeName, "Ksss_color2" , dimSSS2)
        self.dimControl(nodeName, "Ksss2" , dimSSS2)
        self.dimControl(nodeName, "sss_radius2" , dimSSS2)
        self.dimControl(nodeName, "sssRadiusMultiplier2" , dimSSS2)
        
        self.dimControl(nodeName, "Ksss_color3" , dimSSS3)
        self.dimControl(nodeName, "Ksss3" , dimSSS3)
        self.dimControl(nodeName, "sss_radius3" , dimSSS3)
        self.dimControl(nodeName, "sssRadiusMultiplier3" , dimSSS3)
    
    def bumpNew(self, attrName):
        pm.setUITemplate('attributeEditorTemplate', pst=True)
        pm.attrNavigationControlGrp('bumpControl', label="Bump Mapping", at=attrName)
        pm.setUITemplate(ppt=True)
    
    def bumpReplace(self, attrName):
        pm.attrNavigationControlGrp('bumpControl', edit=True, at=attrName)
    
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.templateUI = pm.setParent(query=True)
        
        self.beginLayout("Matte", collapse=True)
        self.addControl("matteEnable", label="Enable Matte", changeCommand=self.update)
        self.addControl("matteColor", label="Matte Color")
        self.endLayout()
        
        self.beginLayout("Common Attributes", collapse=False)
        self.addControl("Kd_color", label="Color", annotation="Diffuse Color")
        self.addControl("Kd", label="Weight")
        self.addControl("diffuse_roughness", label="Roughness")
        self.addSeparator()
        self.addControl("Kb", label="Translucency", changeCommand=self.update)
        self.addControl("Kb_color", label="Translucence Color")
        self.addSeparator()
        self.addControl("opacity", label="Opacity")
        self.addControl("shadowOpacity", label="Shadow Opacity")
        self.addControl("alphaGain", label="Alpha Gain")
        #self.addControl("Fresnel_affect_diff", label="Fresnel affects Diffuse")
        self.addSeparator()
        self.addControl("incandescence", label="Incandescence")
        
        self.beginLayout("Bump", collapse=True)
        self.callCustom( self.bumpNew, self.bumpReplace, "normalCamera")
        self.addControl("diff_bump_multiplier", label="Bump Diffuse Scale")
        self.addControl("spec_bump_multiplier", label="Bump Specular Scale")
        self.addControl("spec_bump_multiplier2", label="Bump Specular Scale2")
        self.addControl("refr_bump_multiplier", label="Bump Refraction Scale")
        self.endLayout()
        
        self.beginLayout("Extended Controls", collapse=True)
        self.addControl("direct_diffuse", label="Direct Diffuse Scale")
        self.addControl("indirect_diffuse", label="Indirect Diffuse Scale")
        self.endLayout()
        self.endLayout()#End Diffuse Layout
        
        self.beginLayout("Specular Reflection", collapse=False)
        self.beginNoOptimize()
        self.addControl("enable_spec1", label="Enable Specular1", changeCommand=self.updateSpec)
        self.addControl("enable_spec2", label="Enable Specular2", changeCommand=self.updateSpec)
        self.endNoOptimize()
        self.addControl("spec_mix", label="Specular Mix", changeCommand=self.updateSpec)
        
        self.beginLayout("Specular 1", collapse=False)
        self.addControl("Ks_color", label="Color")
        self.addControl("Ks", label="Weight")
        #self.addControl("specular_brdf", changeCommand=self.checkSpecularBrdf, label="BRDF")
        self.addControl("specular_roughness", label="Roughness")
        self.addControl("specular_anisotropy", label="Anisotropy")
        self.addControl("specular_rotation", label="Rotation")
        self.addControl("spec_enableIndirect", label="Enable reflections")
        self.addSeparator()
        self.addControl("specular_Fresnel", changeCommand=self.checkSpecularFresnel, label="Fresnel")
        self.addControl("Ksn", label="Reflectance at 0 Deg")
        
        #self.beginLayout("Extended Controls", collapse=True)
        #self.addControl("direct_specular", label="Direct Specular Scale")
        #self.addControl("indirect_specular", label="Indirect Specular Scale")
        #self.endLayout()
        self.endLayout()# End Specular 1 Layout
        
        self.beginLayout("Specular 2", collapse=True)
        self.addControl("Ks_color2", label="Color")
        self.addControl("Ks2", label="Weight")
        #self.addControl("specular_brdf2", changeCommand=self.checkSpecularBrdf, label="BRDF")
        self.addControl("specular_roughness2", label="Roughness")
        self.addControl("specular_anisotropy2", label="Anisotropy")
        self.addControl("specular_rotation2", label="Rotation")
        self.addControl("spec_enableIndirect2", label="Enable reflections")
        self.addSeparator()
        self.addControl("specular_Fresnel2", changeCommand=self.checkSpecularFresnel, label="Fresnel")
        self.addControl("Ksn2", label="Reflectance at 0 Deg")
        
        #self.beginLayout("Extended Controls", collapse=True)
        #self.addControl("direct_specular2", label="Direct Specular Scale")
        #self.addControl("indirect_specular2", label="Indirect Specular Scale")
        #self.endLayout()
        self.endLayout()# End Specular 2 Layout
        self.endLayout()# End Specular Layout
        
        # self.beginLayout("Reflection", collapse=True)
        # self.addControl("Kr_color", label="Color")
        # self.addControl("Kr", label="Weight")
        # self.addControl("enable_internal_reflections", label="Enable Internal Reflections")
        # self.addSeparator()
        # self.addControl("Fresnel", changeCommand=self.checkReflectionFresnel, label="Fresnel")
        # self.addSeparator()
        # self.addControl("Krn", label="Reflectance at 0 Deg")
        # self.beginLayout("Exit Color", collapse=True)
        # self.addControl("reflection_exit_use_environment", label="Use Environment")
        # self.addControl("reflection_exit_color", label="Color")
        # self.endLayout() # End Exit Color Layout
        # self.endLayout() # End Reflection Layout
        
        self.beginLayout("Refraction", collapse=True)
        self.addControl("Kt_color", label="Color")
        self.addControl("Kt", label="Weight")
        self.addControl("IOR", label="IOR")
        self.addControl("refraction_roughness", label="Roughness")
        self.addControl("Fresnel_use_IOR", changeCommand=self.checkFresnelUseIOR, label="Fresnel use IOR")
        self.addControl("transmittance", label="Transmittance")
        self.addControl("transmittanceMultiplier", label="Transmittance Multiplier")
        self.beginLayout("Exit Color", collapse=True)
        self.addControl("refraction_exit_use_environment", label="Use Environment")
        self.addControl("refraction_exit_color", label="Color")
        self.endLayout() # End Exit Color Layout
        self.endLayout() # End Refraction Layout
        
        self.beginLayout("Sub-Surface Scattering", collapse=True)
        self.addControl("enable_sss", label="Enable", changeCommand=self.enableSSS)
        self.addControl("Ksss_color", label="Color")
        self.addControl("Ksss", label="Weight")
        self.addControl("sss_radius", label="Radius per Color")
        self.addControl("sssRadiusMultiplier", label="Radius Multiplier")
        self.addSeparator()
        self.addControl("enable_sss2", label="Enable", changeCommand=self.enableSSS)
        self.addControl("Ksss_color2", label="Color2")
        self.addControl("Ksss2", label="Weight2")
        self.addControl("sss_radius2", label="Radius per Color2")
        self.addControl("sssRadiusMultiplier2", label="Radius Multiplier2")
        self.addSeparator()
        self.addControl("enable_sss3", label="Enable", changeCommand=self.enableSSS)
        self.addControl("Ksss_color3", label="Color3")
        self.addControl("Ksss3", label="Weight3")
        self.addControl("sss_radius3", label="Radius per Color3")
        self.addControl("sssRadiusMultiplier3", label="Radius Multiplier3")
        self.endLayout() # End SSS Layout
        
        self.beginLayout("Caustics", collapse=True)
        self.beginNoOptimize()
        self.addControl("enable_glossy_caustics", label="Enable Glossy Caustics")
        self.addControl("enable_reflective_caustics", label="Enable Reflective Caustics")
        self.addControl("enable_refractive_caustics", label="Enable Refractive Caustics")
        self.endNoOptimize()
        self.endLayout() # End Caustics Layout
        
        self.beginLayout("Advanced", collapse=True)
        self.addControl("bounce_factor", label="Bounce Factor")
        self.beginNoOptimize()
        self.addControl("noIncandForGI", label="No Incandescence for GI Rays")
        self.addControl("diffuseOnlyForGI", label="Diffuse only for GI Rays")
        self.addControl("diffuseOnlyForRefracted", label="Diffuse only for Refr Rays")
        self.addControl("diffuseOnlyForGlossy", label="Diffuse only for Glossy Rays")
        self.addControl("useGeoOpacity", label="Use geo opacity")
        self.endNoOptimize()
        self.addControl("dummyEval", label="Dummy Connection")
        self.endLayout() # End Advanced Layout
        
        self.beginLayout("Hardware Texturing", collapse=True)
        pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("Kd_color incandescence ")')
        self.endLayout()
        
        #self.addAOVLayout(aovReorder = ['direct_diffuse', 'indirect_diffuse', 'direct_specular', 'indirect_specular', 'direct_specular2', 'indirect_specular2', 'reflection', 'refraction', 'emission', 'sss'])
        
        self.update(nodeName)