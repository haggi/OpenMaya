import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

        

class AEasDisneyMaterialTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasDisneyMaterialTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()

    def bumpNew(self, attribute):
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)        
        attName = attribute
        self.bumpCtrl = pm.attrNavigationControlGrp(attribute=attribute, label="Bump Map") 
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
                    
    def bumpReplace(self, attribute):
        if self.bumpCtrl is not None:
            pm.attrNavigationControlGrp(self.bumpCtrl, edit=True, attribute=attribute) 
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("ShaderSettings" ,collapse=0)
        
        self.addControl("BaseColor", label="Base Color")
        self.addControl("Subsurface", label="Subsurface")
        self.addControl("Metallic", label="Metallic")
        self.addControl("Specular", label="Specular")
        self.addControl("SpecularTint", label="SpecularTint")
        self.addControl("Anisotropic", label="Anisotropic")
        self.addControl("Roughness", label="Roughness")
        self.addControl("Sheen", label="Sheen")
        self.addControl("SheenTint", label="SheenTint")
        self.addControl("Clearcoat", label="Clearcoat")
        self.addControl("ClearcoatGloss", label="Clearcoat Gloss")
        self.callCustom(self.bumpNew, self.bumpReplace,"normalCamera")

        self.endLayout()
        