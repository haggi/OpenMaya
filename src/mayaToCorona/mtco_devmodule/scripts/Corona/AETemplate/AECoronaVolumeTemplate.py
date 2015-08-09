import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaVolumeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaVolumeTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        allAttributes = self.node.listAttr()
        allowedAttributes = ["attenuationColor", "attenuationDist", "opacity", "volumeScatteringAlbedo", "singleScatteringOnly", "volumeEmissionColor", "volumeEmissionDist"]
        for att in allAttributes:
            att = att.split(".")[-1]
            if not att in allowedAttributes:
                self.suppress(att)
        
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Absorption" ,collapse=0)
        self.addControl("attenuationColor", label="Color")
        self.addControl("attenuationDist", label="Distance")
        self.addControl("volumeScatteringAlbedo", label="Albedo")
        self.addControl("volumeMeanCosine", label="Directionality")
        self.addControl("singleScatteringOnly", label="Single Bounce Only")
        self.endLayout()
        self.beginLayout("Emission" ,collapse=0)
        self.addControl("volumeEmissionColor", label="Emission Color")
        self.addControl("volumeEmissionDist", label="Emission Distance")
        self.endLayout()
        