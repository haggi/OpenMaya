import pymel.core as pm
import logging
import traceback
import sys
from getpass import getuser

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AECoronaLayeredTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AECoronaLayeredTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.layersUi = None
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        if getuser() != "haggi":
            self.suppress("outColor")
            self.suppress("materialEntry")
            self.suppress("materialEntryAmount")
        self.endScrollLayout()

    def addLayer(self, *args):
        log.debug("addLayer")
        numElements = self.thisNode.materialEntryMtl.numElements()
        index = 0;
        if numElements > 0:
            index = self.thisNode.materialEntryMtl.elementByPhysicalIndex(numElements-1).index() + 1
        self.thisNode.materialEntryMtl[index].get() #create by request
        self.thisNode.materialEntryAmount[index].get() #create by request
        numElements = self.thisNode.materialEntryMtl.numElements()
        self.layersReplace("none")
    
    def removeLayer(self, *args):
        layerNumber = args[0]
        log.debug("removeLayer {0}".format(layerNumber))
        
        if self.thisNode.materialEntryMtl[layerNumber].isConnected():
            input = self.thisNode.materialEntryMtl[layerNumber].inputs(p=1)[0]
            input // self.thisNode.materialEntryMtl[layerNumber]
        self.thisNode.materialEntryMtl[layerNumber].remove()
        
        if self.thisNode.materialEntryAmount[layerNumber].isConnected():
            input = self.thisNode.materialEntryAmount[layerNumber].inputs(p=1)[0]
            input // self.thisNode.materialEntryAmount[layerNumber]
        self.thisNode.materialEntryAmount[layerNumber].remove()
            
    def moveLayer(self, currentIndex, newIndex):
        currentMtlInput = None
        currentAmountInput = None
        currentAmountColor = None

        newLayerMtlInput = None
        newLayerAmountInput = None
        newLayerAmountColor = None
        
        try:
            if self.thisNode.materialEntryMtl[currentIndex].isConnected():
                currentMtlInput = self.thisNode.materialEntryMtl[currentIndex].inputs(p=1)[0]
            if self.thisNode.materialEntryAmount[currentIndex].isConnected():
                currentAmountInput = self.thisNode.materialEntryAmount[currentIndex].inputs(p=1)[0]
            else:
                currentAmountColor = self.thisNode.materialEntryAmount[currentIndex].get()
                
            if self.thisNode.materialEntryMtl[newIndex].isConnected():
                newLayerMtlInput = self.thisNode.materialEntryMtl[newIndex].inputs(p=1)[0]
            if self.thisNode.materialEntryAmount[newIndex].isConnected():
                newLayerAmountInput = self.thisNode.materialEntryAmount[newIndex].inputs(p=1)[0]
            else:
                newLayerAmountColor = self.thisNode.materialEntryAmount[newIndex].get()
                    
            # disconnect existing connections
            if currentMtlInput is not None:
                currentMtlInput // self.thisNode.materialEntryMtl[currentIndex]            
            if newLayerMtlInput is not None:
                newLayerMtlInput // self.thisNode.materialEntryMtl[newIndex]
            if currentAmountInput is not None:
                currentAmountInput // self.thisNode.materialEntryAmount[currentIndex]
            if newLayerAmountInput is not None:
                newLayerAmountInput // self.thisNode.materialEntryAmount[newIndex]

            if currentMtlInput is not None:
                currentMtlInput >> self.thisNode.materialEntryMtl[newIndex]            
            if newLayerMtlInput is not None:
                newLayerMtlInput >> self.thisNode.materialEntryMtl[currentIndex]
            if currentAmountInput is not None:
                currentAmountInput >> self.thisNode.materialEntryAmount[newIndex]
            else:
                self.thisNode.materialEntryAmount[newIndex].set(currentAmountColor)
            if newLayerAmountInput is not None:
                newLayerAmountInput >> self.thisNode.materialEntryAmount[currentIndex]
            else:
                self.thisNode.materialEntryAmount[currentIndex].set(newLayerAmountColor)
                            
        except:
            traceback.print_exc(file=sys.__stderr__)
            return
                        
    def moveLayerUp(self, *args):
        layerNumber = args[0]
        log.debug("moveLayerUp {0}".format(layerNumber))
        
        print "this node", self.thisNode
        
        firstIndex = self.thisNode.materialEntryMtl.elementByPhysicalIndex(0).index()
        if layerNumber == firstIndex:
            log.debug("Layer is first layer, cannot move up")
            return
        
        indexAbove = -1
        for entryId in range(self.thisNode.materialEntryMtl.numElements()):
            if self.thisNode.materialEntryMtl.elementByPhysicalIndex(entryId).index() == layerNumber:
                break
            indexAbove = self.thisNode.materialEntryMtl.elementByPhysicalIndex(entryId).index()
        
        if indexAbove < 0:
            log.error("Could not find index above current one.")
            return

        self.moveLayer(layerNumber, indexAbove)   
        self.layersReplace("none")
     
    
    def moveLayerDown(self, *args):
        layerNumber = args[0]
        log.debug("moveLayerDown {0}".format(layerNumber))
                
        lastIndex = self.thisNode.materialEntryMtl.elementByPhysicalIndex(self.thisNode.materialEntryMtl.numElements()-1).index()
        if layerNumber == lastIndex:
            log.debug("Layer is last layer, cannot move down.")
            return
        
        newIndex = -1
        for entryId in range(self.thisNode.materialEntryMtl.numElements()):
            if self.thisNode.materialEntryMtl.elementByPhysicalIndex(entryId).index() == layerNumber:
                newIndex = self.thisNode.materialEntryMtl.elementByPhysicalIndex(entryId + 1).index()
                break
        
        if newIndex < 0:
            log.error("Could not find index below current one.")
            return

        self.moveLayer(layerNumber, newIndex)   
        self.layersReplace("none")
    
    def layersReplace(self, attribute):
        if attribute is not "none":
            self.thisNode = pm.PyNode(attribute).node()
        log.debug("layersReplace {0}".format(attribute))
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        materialEntries = self.thisNode.materialEntryMtl.numElements()
        #print "layersReplace: node has ", self.thisNode.materialEntryMtl.numElements(), "layers"
        if self.layersUi is not None:
            pm.deleteUI(self.layersUi)
        with pm.columnLayout(adj=True, parent=self.uiParent) as self.layersUi:
            for layerNumber in range(materialEntries):
                layerIndex = self.thisNode.materialEntryMtl.elementByPhysicalIndex(layerNumber).index()
                with pm.frameLayout(label="Layer {0}".format(layerNumber), collapsable=True, collapse=False) as fl:
                    print "create layer", layerNumber
                    with pm.columnLayout(adj=True):
                        attribute = self.thisNode.materialEntryMtl[layerIndex]
                        #tf = pm.textFieldGrp(label="Shader", editable=False)
                        if attribute.isConnected():
                            pm.frameLayout(fl, edit=True, label=attribute.inputs(p=1)[0])
                        #    tf.setText(attribute.inputs(p=1)[0])
                        
                        pm.attrColorSliderGrp(label="Material", at=attribute)
                        attribute = self.thisNode.materialEntryAmount[layerIndex]
                        pm.attrColorSliderGrp(label="Amount", at=attribute)
                    with pm.columnLayout(adj=True):
                        pm.button(label="Remove Layer", c=pm.Callback(self.removeLayer, layerIndex), height=18)
                        pm.button(label="Layer Up", c=pm.Callback(self.moveLayerUp, layerIndex), height=18)
                        pm.button(label="Layer Down", c=pm.Callback(self.moveLayerDown, layerIndex), height=18)
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
        
    def layersNew(self, attribute):
        print "layersNew", attribute
        self.uiParent = pm.setParent(query=True)        
        pm.button(label="Add Layer", c=self.addLayer, parent=self.uiParent)
        self.layersReplace(attribute)
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("Base" ,collapse=0)
        self.addControl("baseMaterial", label="Base Material")
        self.endLayout()
        self.beginLayout("Layers" ,collapse=0)
        self.callCustom( self.layersNew, self.layersReplace, "materialEntryMtl")        
        self.endLayout()
#         self.beginNoOptimize()
#         self.addControl("emissionColorMultiplier", label="Multiplier")
#         self.addControl("opacity", label="Opacity")
#         self.addControl("emitLight", label="Emit Light")
#         self.addControl("emissionGlossyness", label="Directionality")
#         self.addSeparator()
#         self.addControl("iesProfile", label="IES Profile")
#         self.addControl("emissionSharpnessFake", label="Sharp Patterns")
#         #self.addControl("emissionDisableSampling", label="Disable Sampling")
#         #self.addControl("emissionSharpnessFakePoint", label="Sharpness Fake Point")
#         self.endNoOptimize()
        
        #self.beginLayout("Hardware Texturing" ,collapse=0)
        #pm.mel.eval('AEhardwareTextureTemplate "%s"' % self.nodeName + r'("diffuse emissionColor ")')
        #self.endLayout()
        