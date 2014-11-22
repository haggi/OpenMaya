import pymel.core as pm
import passes

class PassesUI(object):    
    def __init__(self, parent=None):
        self.parent = parent
        if self.parent == None:
            self.parent = pm.window()
        self.title = "RenderPasses"
        self.rtf = True
        self.witdh = 150
        self.passesNode = None
        self.uiBasicLayout = None
        self.availableAOVsListUI = None
        self.activeAOVsListUI = None
        self.activeAOVS = []
        self.buildUI()
        if parent == None:
            self.parent.show()
                
    def getAOVs(self):
        return [
                "Alpha",
                "SourceColor",
                "Components",
                "Shadows",
                "Albedo",
                "RawComponent",
                
                "Normals",
                "NormalsDotProduct",
                "NormalsDiscrepancy",
                
                
                "PrimitiveCoords",
                "MapCoords",
                "Velocity",
                "ZDepth",
                "WorldPosition",
                "Id",
                "Texmap"
                ]
        
    def getActiveAOVs(self):
        aovs = pm.PyNode("coronaGlobals").AOVs
        return aovs.inputs()

    def addAOVAttributes(self, passNode):
        passName = self.getAOVs()[passNode.passType.get()]
        passNode.addAttr("components", dt="string", keyable=1)
        if passName == "MapCoords":
            passNode.addAttr("mapChannel", at="long", softMaxValue=10, minValue=0, keyable=1)
        if passName in ["RawComponent"]:
            passNode.addAttr("elementType", at="enum", keyable=1, enumName="diffuse:translucency:reflect:refract")
        if passName in ["SourceColor"]:
            passNode.addAttr("elementType", at="enum", keyable=1, enumName="diffuse:translucency:reflect:refract:opacity")
        if passName in ["Normals"]:
            passNode.addAttr("elementType", at="enum", keyable=1, enumName="shading:geometry")                
        if passName in ["Id"]:
            passNode.addAttr("elementType", at="enum", keyable=1, enumName="primitive:instance:geometryGroup:material")                
        if passName == "ZDepth":
            passNode.addAttr("minDepth", at="float", dv=0.01, keyable=1)
            passNode.addAttr("maxDepth", at="float", dv=100000.0, keyable=1)
                    
    def AOVdoubleClicked(self):
        item = pm.textScrollList(self.availableAOVsListUI, query=True, selectItem=True)[0] 
        passesNode = pm.createNode("coronaPassesNode", n=item)
        passesNode.passType.set(pm.textScrollList(self.availableAOVsListUI, query=True, sii=True)[0] - 1)
        ne = pm.PyNode("coronaGlobals").AOVs.numElements()     
        if ne > 0:      
            index = pm.PyNode("coronaGlobals").AOVs.elementByPhysicalIndex(ne - 1).logicalIndex()
            for i in range(index + 2):
                if not pm.PyNode("coronaGlobals").AOVs[i].isConnected():                
                    passesNode.message >> pm.PyNode("coronaGlobals").AOVs[i]
                    break
        else:
            passesNode.message >> pm.PyNode("coronaGlobals").AOVs[0]
        self.addAOVAttributes(passesNode)
        self.updateExistingAOVList()
        
    def exsistingAOVdoubleClicked(self):
        item = pm.textScrollList(self.activeAOVsListUI, query=True, selectItem=True)[0] 
        passNode = pm.PyNode(item)
        output = passNode.outputs(p=True)[-1]
        id = output.split(".")[-1].replace("AOVs[", "")[:-1]
        passNode.message // output
        output.remove()
        pm.delete(passNode)
        self.updateExistingAOVList()
        
    def exsistingAOVclicked(self):
        item = pm.textScrollList(self.activeAOVsListUI, query=True, selectItem=True)[0] 
        pm.select(item)

    def updateExistingAOVList(self):
        aovs = self.getActiveAOVs()
        pm.textScrollList(self.activeAOVsListUI, edit=True, ra=True)
        pm.textScrollList(self.activeAOVsListUI, edit=True, append=aovs)
              
    def nameChangedCallback(self):
        self.updateExistingAOVList()
                      
    def buildUI(self):
        
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)        
        
        self.passesParent = pm.setParent(query=True)
        frameLayout = None
        with pm.frameLayout(label="Passes", collapsable=False, parent=self.parent) as frameLayout:
            with pm.columnLayout(adj=True):
                with pm.rowLayout(adj=True, nc=2, cw=((1,200), (2, 200)), h=350):
                    with pm.frameLayout(label="Existing Passes", collapsable=False, h=350):
                        self.availableAOVsListUI = pm.textScrollList(doubleClickCommand=self.AOVdoubleClicked)
                        pm.textScrollList(self.availableAOVsListUI, edit=True, append=self.getAOVs())
                              
                    with pm.frameLayout(label="Active Passes", collapsable=False, h=350):
                        self.activeAOVsListUI = pm.textScrollList( selectCommand=self.exsistingAOVclicked, doubleClickCommand=self.exsistingAOVdoubleClicked)
                        self.updateExistingAOVList()
                
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.scriptJob(event=("NameChanged", self.nameChangedCallback), parent=frameLayout)
