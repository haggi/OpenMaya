import sys
import pymel.core as pm
import logging
import traceback
from getpass import getuser

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

        

class AEasLayeredShaderTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        log.debug("AEasLayeredShaderTemplate")
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.layersUi = None
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        if getuser() != "haggi":
            self.suppress("outColor")
            self.suppress("materialEntryMtl")
            self.suppress("materialEntryMsk")        
            self.suppress("materialEntryMode")        
        self.endScrollLayout()

    def addLayer(self, *args):
        log.debug("addLayer")
        numElements = self.thisNode.materialEntryMtl.numElements()
        index = 0;
        if numElements > 0:
            index = self.thisNode.materialEntryMtl.elementByPhysicalIndex(numElements-1).index() + 1
        self.thisNode.materialEntryMtl[index].get() #create by request
        self.thisNode.materialEntryMsk[index].get() #create by request
        numElements = self.thisNode.materialEntryMtl.numElements()
        self.layersReplace("none")
    
    def removeLayer(self, *args):
        layerNumber = args[0]
        log.debug("removeLayer {0}".format(layerNumber))
        
        if self.thisNode.materialEntryMtl[layerNumber].isConnected():
            input = self.thisNode.materialEntryMtl[layerNumber].inputs(p=1)[0]
            input // self.thisNode.materialEntryMtl[layerNumber]
        self.thisNode.materialEntryMtl[layerNumber].remove()
        
        if self.thisNode.materialEntryMsk[layerNumber].isConnected():
            input = self.thisNode.materialEntryMsk[layerNumber].inputs(p=1)[0]
            input // self.thisNode.materialEntryMsk[layerNumber]
        self.thisNode.materialEntryMsk[layerNumber].remove()
            
    def moveLayer(self, currentIndex, newIndex):
        currentMtlInput = None
        currentMskInput = None
        currentMskColor = None

        newLayerMtlInput = None
        newLayerMskInput = None
        newLayerMskColor = None
        
        try:
            if self.thisNode.materialEntryMtl[currentIndex].isConnected():
                currentMtlInput = self.thisNode.materialEntryMtl[currentIndex].inputs(p=1)[0]
            if self.thisNode.materialEntryMsk[currentIndex].isConnected():
                currentMskInput = self.thisNode.materialEntryMsk[currentIndex].inputs(p=1)[0]
            else:
                currentMskColor = self.thisNode.materialEntryMsk[currentIndex].get()
                
            if self.thisNode.materialEntryMtl[newIndex].isConnected():
                newLayerMtlInput = self.thisNode.materialEntryMtl[newIndex].inputs(p=1)[0]
            if self.thisNode.materialEntryMsk[newIndex].isConnected():
                newLayerMskInput = self.thisNode.materialEntryMsk[newIndex].inputs(p=1)[0]
            else:
                newLayerMskColor = self.thisNode.materialEntryMsk[newIndex].get()
                    
            # disconnect existing connections
            if currentMtlInput is not None:
                currentMtlInput // self.thisNode.materialEntryMtl[currentIndex]            
            if newLayerMtlInput is not None:
                newLayerMtlInput // self.thisNode.materialEntryMtl[newIndex]
            if currentMskInput is not None:
                currentMskInput // self.thisNode.materialEntryMsk[currentIndex]
            if newLayerMskInput is not None:
                newLayerMskInput // self.thisNode.materialEntryMsk[newIndex]

            if currentMtlInput is not None:
                currentMtlInput >> self.thisNode.materialEntryMtl[newIndex]            
            if newLayerMtlInput is not None:
                newLayerMtlInput >> self.thisNode.materialEntryMtl[currentIndex]
            if currentMskInput is not None:
                currentMskInput >> self.thisNode.materialEntryMsk[newIndex]
            else:
                self.thisNode.materialEntryMsk[newIndex].set(currentMskColor)
            if newLayerMskInput is not None:
                newLayerMskInput >> self.thisNode.materialEntryMsk[currentIndex]
            else:
                self.thisNode.materialEntryMsk[currentIndex].set(newLayerMskColor)
                            
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
        if self.layersUi is not None and pm.columnLayout(self.layersUi, q=True, exists=True):
            pm.deleteUI(self.layersUi)
        with pm.columnLayout(adj=True, parent=self.uiParent) as self.layersUi:
            for layerNumber in range(materialEntries):
                layerIndex = self.thisNode.materialEntryMtl.elementByPhysicalIndex(layerNumber).index()
                with pm.frameLayout(label="Layer {0}".format(layerNumber), collapsable=True, collapse=False, bv=True) as fl:
                    log.debug("create layers UI {0}".format(self.thisNode.materialEntryMtl[layerIndex]))
                    with pm.columnLayout(adj=True):
                        attribute = self.thisNode.materialEntryMtl[layerIndex]
                        if attribute.isConnected():
                            pm.frameLayout(fl, edit=True, label=attribute.inputs(p=1)[0])
                        pm.attrColorSliderGrp(label="Material", at=attribute)
                        attribute = self.thisNode.materialEntryMsk[layerIndex]
                        pm.attrFieldSliderGrp(label="Mask", at=attribute)
                        attribute = self.thisNode.materialEntryMode[layerIndex]
                        pm.attrEnumOptionMenuGrp(label="Mode", at=attribute)
                    with pm.columnLayout(adj=True):
                        pm.button(label="Remove Layer", c=pm.Callback(self.removeLayer, layerIndex), height=18)
                        pm.button(label="Layer Up", c=pm.Callback(self.moveLayerUp, layerIndex), height=18)
                        pm.button(label="Layer Down", c=pm.Callback(self.moveLayerDown, layerIndex), height=18)
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
        
    def layersNew(self, attribute):
        log.debug("layersNew {0}".format(attribute))
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