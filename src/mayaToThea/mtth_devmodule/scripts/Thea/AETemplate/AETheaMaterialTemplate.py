import pymel.core as pm
import logging

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AETheaMaterialTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AETheaMaterialTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()

    def layerCreate(self, attribute):
        print "layerCreate attr", attribute
        pmAttr = pm.Attribute(attribute)
        self.thisNode = pmAttr.node()
        parent = pm.setParent(query=True)
        with pm.columnLayout(adjustableColumn=True, parent=parent):                    
            pm.button(label="Create New Layer")
            with pm.columnLayout(adjustableColumn=True):                    
                for i in range(pmAttr.numElements()):
                    with pm.frameLayout(collapsable=False, label="Layer {0}".format(i)):
                        with pm.columnLayout(adjustableColumn=True):                    
                            nameCtrl = pm.textFieldGrp(label="Name")
                            pm.connectControl(nameCtrl, pmAttr[i].layerName, index = 2)
                            weightCtrl = pm.floatFieldGrp(label="Weight", value1=0.0)
                            pm.connectControl(weightCtrl, pmAttr[i].layerWeight, index = 2)
                            texCtrl = pm.attrColorSliderGrp(at=pmAttr[i].layerTexture, label="Texture");
                            shdCtrl = pm.attrColorSliderGrp(at = pmAttr[i].layerShader, label="Shader");
                        with pm.columnLayout(adjustableColumn=True):
                            with pm.rowLayout(nc=3):
                                pm.button(label="Up")
                                pm.button(label="Delete")
                                pm.button(label="Down")
                                               
                
        
    def layerUpdate(self, attribute):
        pmAttr = pm.Attribute(attribute)
        self.thisNode = pmAttr.node()
        parent = pm.setParent(query=True)
        print "layerUpdate attr", attribute
            
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Base Layer" ,collapse=0)
        self.addControl("bsdf", label="BSDF")
        self.endLayout()
        self.beginLayout("Emitter Settings" ,collapse=1)
        self.addControl("emitter", label="Emitter")
        self.addControl("emitterMinRays", label="EmitterMinRays")
        self.addControl("emitterMaxRays", label="EmitterMaxRays")
        self.addControl("emitterAccuracy", label="EmitterAccuracy")
        self.addControl("ambientEmitter", label="AmbientEmitter")
        self.addControl("passiveEmitter", label="PassiveEmitter")
        self.endLayout()
        self.beginLayout("Layers" ,collapse=0)
        self.callCustom(self.layerCreate, self.layerUpdate, 'layers')
        self.endLayout()
        self.beginLayout("Material General Settings" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("twosided", label="Twosided")
        self.addControl("shadowCatcher", label="ShadowCatcher")
        self.addControl("activeDirt", label="ActiveDirt")
        self.endNoOptimize()
        self.addControl("perceptualLevel", label="PerceptualLevel")
        self.addControl("ambientLevel", label="AmbientLevel")
        self.addControl("tracingDepth", label="TracingDepth")
        self.addSeparator()
        self.endLayout()
        