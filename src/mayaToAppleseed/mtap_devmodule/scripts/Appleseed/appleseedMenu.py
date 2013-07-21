import pymel.core as pm
import logging

log = logging.getLogger("mtapLogger")


class StandinOptions(pm.ui.Window):
    def __init__(self):
        self.title = "Standin Options"
        self.rtf = True
        self.pathUI = None
        self.createProxyUI = None
        self.nthPolyUI = None
        self.percentageUI = None
        self.initUI()
        self.show()

    def updateUI(self):
        pass
    
    def getShadingGroups(self, mesh):
        shadingGroups = mesh.outputs(type="shadingEngine")
        return shadingGroups
    
    def perform(self, *args):
        #get selected meshes
        selection = pm.ls(sl=True)
        meshes = []
        for s in selection:
            allChildren = s.getChildren(ad=True)
            for child in allChildren:
                if child.type() == "mesh":
                    meshes.append(child)
        
        print "Found", len(meshes), "meshes"
        path = pm.textFieldButtonGrp(self.pathUI, query=True, text=True)
        doProxy = pm.checkBoxGrp(self.createProxyUI, query=True, value1=True)
        nthPoly = pm.intFieldGrp(self.nthPolyUI, query=True, value1=True)
        percentage = pm.floatFieldGrp(self.percentageUI, query=True, value1=True)
        
        for mesh in meshes:
            bpath = path + "/" + str(mesh) + ".binarymesh"
#TODO: convert namespace to clean name
            #print  "pm.meshToBinarymesh(pa=",bpath,", m=",mesh,", p=",percentage,", n=",nthPoly,")"
            pm.meshToBinarymesh(pa=bpath, m=mesh, p=percentage, n=nthPoly)
            
            if pm.checkBoxGrp(self.createStdInUI, query=True, value1=True):
                print "creating stind node"
                standInMesh = pm.createNode("mesh")
                standInMesh.getParent().rename(str(mesh) + "_standIn")
                standInMeshNode = pm.createNode("mtap_standinMeshNode")
                standInMeshNode.rename(str(mesh) + "_standInCreator")
                standInMeshNode.binMeshFile.set(bpath)
                standInMeshNode.outputMesh >> standInMesh.inMesh
                
                # get input shading groups
                shadingGroups = self.getShadingGroups(mesh)
                pm.sets(shadingGroups[0], forceElement=standInMesh)
                numGroups = len(shadingGroups)
                numFaces = mesh.numFaces()
                selectFaces = numFaces / numGroups
                
                for sgId in range(1, len(shadingGroups)):
                    startF = selectFaces * sgId
                    endF = selectFaces * (sgId + 1)
                    if endF > numFaces:
                        endF = numFaces
                    faceSelect = mesh.f[startF : endF]
                    pm.sets(shadingGroups[sgId], forceElement=faceSelect)
                    
        self.cancel()
        
    def cancel(self, *args):
        self.delete()
        
    def fileBrowser(self, *args):
        erg = pm.fileDialog2(dialogStyle=2, fileMode=2)
        print erg
        if len(erg) > 0:
            if len(erg[0]) > 0:
                pm.textFieldButtonGrp(self.pathUI, edit=True, text=erg[0])
        
    def initUI(self):
        pm.setUITemplate("DefaultTemplate", pushTemplate=True)
        form = pm.formLayout()

        with pm.columnLayout('StandinLayout') as StandinLayout:
            with pm.frameLayout('StandinLayout', label="Standin export options", collapsable=False):
                with pm.columnLayout('StandinColumnLayout'):
                    self.pathUI = pm.textFieldButtonGrp(label="Standin path", text='', buttonLabel="File", buttonCommand=self.fileBrowser)
                    self.createProxyUI = pm.checkBoxGrp(label="Create proxy", value1=True)
                    self.nthPolyUI = pm.intFieldGrp(label="nthPolygon", value1=100)
                    self.percentageUI = pm.floatFieldGrp(label="Percentage", value1=0.1)
                    self.createStdInUI = pm.checkBoxGrp(label="Create StandIn", value1=True)
            with pm.rowColumnLayout(numberOfColumns=2):
                pm.button(label="Create Standin", c=self.perform)
                pm.button(label="Cancel", c=self.cancel)
            
        pm.formLayout(form, edit=True, attachForm=[(StandinLayout, 'top', 5), (StandinLayout, 'bottom', 5), (StandinLayout, 'right', 5), (StandinLayout, 'left', 5)])
        pm.setUITemplate("DefaultTemplate", popTemplate=True)
            
def createStandin(*args):
    print "create standin", args
    

def createStandinOptions(*args):
    print "create standin options", args
    StandinOptions()
    
    
#class StandinOptions(pm.ui.OptionBox):
#    commandName = "createStandin"
#    options = []
#    factorySettings = []
#    apply = staticmethod(createStandin)
#
#    @classmethod
#    def getFlags(cls):
#        pass
#    
#    def initUI(self):
#        self.modeCtrl = pm.radioButtonGrp(label="Mode:", nrb=3, la3=["Reference", "Import", "Export"])
#        self.setUIControl("Mode", self.modeCtrl, "select", 1)
#        pm.separator()
#        with pm.columnLayout() as self.importLayout:
#            self.setUIControl("Shot", AbFieldGrp(), "path")
#            self.setUIControl("KeepExisting", pm.checkBoxGrp(label="Keep existing camera:"), "value1")
#            self.setUIControl("dummy", pm.checkBoxGrp(label="", manage=0), "value1") # was "useGate"
#            self.setUIControl("SetTime", pm.checkBoxGrp(label="Set timeline to shot range:"), "value1")
#        pm.separator()
#        with pm.columnLayout() as self.exportLayout:
#            self.setUIControl("Comment", pm.textFieldGrp(label="Check-in comment:"), "text")
#            self.setUIControl("Selected", pm.checkBoxGrp(label="Export selected:"), "value1")
#            self.setUIControl("Reference", pm.checkBoxGrp(label="Create reference:"), "value1")
#        self.modeCtrl.changeCommand = self.postSetup
#
#    def postSetup(self, *args):
#        self.importLayout.enable = (self.modeCtrl.select != 3)
#        self.exportLayout.enable = (self.modeCtrl.select == 3)


class AppleseedMenu(pm.ui.Menu):
    """Custom Appleseed menu"""
    def __new__(cls, *args, **kwargs):
        name = "appleseedMenu"
        parent = kwargs.pop("parent", "MayaWindow")
        if pm.menu(name, query=True, exists=True):
            pm.deleteUI(name)
        return super(AppleseedMenu, cls).__new__(cls, name, label="Appleseed", parent=parent, familyImage="appleseed", tearOff=True, allowOptionBoxes=True)
    
    def __init__(self):
        pm.setParent(self, menu=True)
        with pm.menuItem(label="Standins", subMenu=True, tearOff=True):
            pm.menuItem(label="Create Standin", annotation="Standin creation", command=createStandin, stp="python")
            pm.menuItem(optionBox=True, label="Create Standin Options", command=createStandinOptions, stp="python")

        