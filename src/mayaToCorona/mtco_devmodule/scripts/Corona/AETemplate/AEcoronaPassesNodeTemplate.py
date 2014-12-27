import pymel.core as pm
import logging
import Corona.passes

log = logging.getLogger("ui")

class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class TextListWindow(pm.ui.Window):    
    def __init__(self, title, node):
        self.title = title
        self.componentNode = node
        self.rtf = True
        self.witdh = 150
        self.scrollList = None
        self.buildUI()
        self.show()

    def getComponentList(self):
        return ["Emission", 
                "DiffuseDirect", "TranslucencyDirect", "ReflectDirect", "RefractDirect",
                "DiffuseIndirect", "TranslucencyIndirect", "ReflectIndirect", "RefractIndirect"]

    def componentClicked(self):
        componentString = self.componentNode.components.get()
        item = pm.textScrollList(self.scrollList, query=True, selectItem=True)[0]
        if componentString is None or len(componentString) == 0:
            componentString = item
        else:
            if not item in componentString:
                componentString += "," + item
        self.componentNode.components.set(componentString)
        #pm.select(cl=True)
        pm.select(self.componentNode)
    
    def buildUI(self):
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)                
        with pm.frameLayout(label="Components", collapsable=False):
            with pm.columnLayout(adj=True):
                self.scrollList = pm.textScrollList(doubleClickCommand=self.componentClicked)
                pm.textScrollList(self.scrollList, edit=True, append=self.getComponentList())
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
        
class AEcoronaPassesNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self,nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.componentsButton = None
        self.componentsListUI = None
        self.uiParent = None
        pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()

    def showComponentList(self, node):
        TextListWindow("Components", node)
    
    def removeComponent(self, node, component):
        components = node.components.get().split(",")
        components.pop(components.index(component))
        node.components.set(",".join(components))
        pm.select(node)
    
    def componentsListRebuild(self, node):
        if node.passType.get() == 2:
            self.componentsButton = pm.button(label="Add Component", c=pm.Callback(self.showComponentList, node), parent=self.uiParent)
        componentString = node.components.get()
        if componentString is None or len(componentString) == 0:
            return
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        with pm.frameLayout(labelVisible=False, collapsable=False, w=240, parent=self.uiParent) as self.componentsListUI:
            with pm.rowColumnLayout(nc=2, cw=((1,200), (2, 30))):
                for c in componentString.split(","):
                    pm.text(label=c, align="right")
                    pm.symbolButton(image="smallTrash.png", c=pm.Callback(self.removeComponent, node, c))
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)                
            
    def componentsNew(self, attribute):
        self.uiParent = pm.setParent(query=True)
        self.thisNode = pm.Attribute(attribute).node()
        if self.thisNode.passType.get() != 2:
            return
        self.componentsListRebuild(self.thisNode)
        
    def componentsReplace(self, attribute):
        self.uiParent = pm.setParent(query=True)
        self.thisNode = pm.Attribute(attribute).node()
        if self.componentsListUI is not None:
            pm.deleteUI(self.componentsListUI)
            self.componentsListUI = None
        if self.componentsButton is not None:
            pm.deleteUI(self.componentsButton)
            self.componentsButton = None            
        self.componentsListRebuild(self.thisNode)
    
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        pa = Corona.passes.getAOVs()
        typeid = self.thisNode.passType.get()
        self.passTypeName = pa[typeid]
        self.beginLayout("Corona Pass" ,collapse=0)
        self.beginNoOptimize()
        self.addControl("mapChannel", "Map Channel")
        #pm.popupMenu()
        #pm.menuItem()
        self.addControl("elementType", "Element Type")
        self.addControl("minDepth", "Min Depth")
        self.addControl("maxDepth", "Max Depth")
        self.callCustom( self.componentsNew, self.componentsReplace, "components")
        self.addSeparator()
        self.addControl("dataType", "Data Type")
        self.endNoOptimize()
        self.endLayout()
        