import pymel.core as pm
import logging
import os

log = logging.getLogger("mtapLogger")

class BinaryMesh(object):
    def __init__(self, meshList = None):
        self.meshList = meshList
        self.path = pm.optionVar.get('mtap_binMeshExportPath', pm.workspace.path + "/geo/export.binarymesh")
        self.prefix = pm.optionVar.get('mtap_binMeshExportPathPrefix', "prefix")
        self.doProxy = pm.optionVar.get('mtap_binMeshCreateProxy', True)
        self.percentage = pm.optionVar.get('mtap_binMeshPercentage', 0.1)
        self.createStandin = pm.optionVar.get('mtap_binMeshCreateStandin', True)
        self.oneFilePerMesh = pm.optionVar.get('mtap_binMeshOneFilePerMesh', False)
        self.useTransform = pm.optionVar.get('mtap_binMeshUseTransform', False)
    
    def loadStandins(self):
        meshName = self.path.split("/")[-1].replace(".binarymesh", "")
        print "creating stdin node for mesh", meshName
        standInMesh = pm.createNode("mesh")
        standInMesh.getParent().rename(meshName + "_standIn")
        standInMeshNode = pm.createNode("mtap_standinMeshNode")
        standInMeshNode.rename(meshName + "_standInCreator")
        print "Setting binmesh path to", self.path
        standInMeshNode.binMeshFile.set(self.path)
        standInMeshNode.outputMesh >> standInMesh.inMesh

    def writeStandins(self):
        pass

class StandinOptions(pm.ui.Window):
    def __init__(self):
        self.title = "Standin Options"
        self.rtf = True
        self.pathUI = None
        self.prefixUI = None
        self.createProxyUI = None
        self.nthPolyUI = None
        self.percentageUI = None
        self.createStdInUI = None
        self.oneFilePerMeshUI = None
        self.doTransformUI = None
        
        self.initUI()

    def updateUI(self):
        pass
    
    def getShadingGroups(self, mesh):
        shadingGroups = mesh.outputs(type="shadingEngine")
        return shadingGroups

    def doit(self, *args):        
        self.getData()
        self.perform()
        
    def getData(self, *args):        
        path = pm.textFieldButtonGrp(self.pathUI, query=True, text=True)
        prefix = pm.textFieldGrp(self.prefixUI, query=True, text=True)
        doProxy = pm.checkBoxGrp(self.createProxyUI, query=True, value1=True)
        percentage = pm.floatFieldGrp(self.percentageUI, query=True, value1=True)
        createStdin = pm.checkBoxGrp(self.createStdInUI, query=True, value1=True)
        oneFilePerMesh = pm.checkBoxGrp(self.oneFilePerMeshUI, query=True, value1=True)
        useTransform = pm.checkBoxGrp(self.doTransformUI, query=True, value1=True)
                
        pm.optionVar['mtap_binMeshExportPath'] = path
        pm.optionVar['mtap_binMeshExportPathPrefix'] = prefix
        pm.optionVar['mtap_binMeshCreateProxy'] = doProxy
        pm.optionVar['mtap_binMeshPercentage'] = percentage
        pm.optionVar['mtap_binMeshCreateStandin'] = createStdin
        pm.optionVar['mtap_binMeshOneFilePerMesh'] = oneFilePerMesh
        pm.optionVar['mtap_binMeshUseTransform'] = useTransform
    
    def perform(self, *args):        
        selection = []
        for object in pm.ls(sl=True):
            selection.extend(object.getChildren(ad=True, type="mesh"))
            
        if len(selection) == 0:
            log.error("Export standins: No meshes selected.")
            return

        path = pm.optionVar.get('mtap_binMeshExportPath', pm.workspace.path + "/geo/export.binarymesh")
        prefix = pm.optionVar.get('mtap_binMeshExportPathPrefix', "prefix")
        doProxy = pm.optionVar.get('mtap_binMeshCreateProxy', True)
        percentage = pm.optionVar.get('mtap_binMeshPercentage', 0.1)
        createStandin = pm.optionVar.get('mtap_binMeshCreateStandin', True)
        oneFilePerMesh = pm.optionVar.get('mtap_binMeshOneFilePerMesh', False)
        useTransform = pm.optionVar.get('mtap_binMeshUseTransform', False)
                
        if not os.path.exists(path):
            os.makedirs(path)
        
        bpath = path + "/" + prefix  + ".binarymesh"
#TODO: convert namespace to clean name
        print "pm.binMeshWriterCmd({0}, doProxy = {1}, path={2}, doTransform = {3}, percentage={4}, oneFilePerMesh={5})".format(selection, doProxy, bpath, useTransform, percentage, oneFilePerMesh)

        pm.binMeshWriterCmd(selection, doProxy = doProxy, path=bpath, doTransform = useTransform, percentage=percentage, oneFilePerMesh=oneFilePerMesh)

        if not doProxy:
            log.debug("No proxy creation.")
            return

        # we we write multiple meshes into one file, we should create one standin mesh only
        if not oneFilePerMesh and len(selection) > 1:
            selection = [bpath]
        for mesh in selection:
            meshName = mesh.split("/")[-1].replace(".binarymesh", "")
            print "creating stdin node for mesh", meshName
            standInMesh = pm.createNode("mesh")
            standInMesh.getParent().rename(meshName + "_standIn")
            standInMeshNode = pm.createNode("mtap_standinMeshNode")
            standInMeshNode.rename(meshName + "_standInCreator")
            bpath = path + "/" + prefix + meshName  + ".binarymesh"
            print "Setting binmesh path to", bpath
            standInMeshNode.binMeshFile.set(bpath)
            standInMeshNode.outputMesh >> standInMesh.inMesh
                    
        self.cancel()
        
    def cancel(self, *args):
        self.delete()
        
    def fileBrowser(self, *args):
        erg = pm.fileDialog2(dialogStyle=2, fileMode=2)
        print "Result", erg
        if len(erg) > 0:
            if len(erg[0]) > 0:
                exportPath = erg[0]
                pm.textFieldButtonGrp(self.pathUI, edit=True, text=exportPath)
            
        
    def initUI(self):
        pm.setUITemplate("DefaultTemplate", pushTemplate=True)
        form = pm.formLayout()

        binMeshExportPath = pm.optionVar.get('mtap_binMeshExportPath', pm.workspace.path + "/geo/export.binarymesh")
        prefix = pm.optionVar.get('mtap_binMeshExportPathPrefix', "prefix")
        createProxy = pm.optionVar.get('mtap_binMeshCreateProxy', True)
        percentage = pm.optionVar.get('mtap_binMeshPercentage', 0.1)
        createStandin = pm.optionVar.get('mtap_binMeshCreateStandin', True)
        oneFilePerMesh = pm.optionVar.get('mtap_binMeshOneFilePerMesh', False)
        useTransform = pm.optionVar.get('mtap_binMeshUseTransform', False)

        with pm.columnLayout('StandinLayout') as StandinLayout:
            with pm.frameLayout('StandinLayout', label="Standin export options", collapsable=False):
                with pm.columnLayout('StandinColumnLayout'):
                    self.pathUI = pm.textFieldButtonGrp(label="Standin directory", text=binMeshExportPath, buttonLabel="File", buttonCommand=self.fileBrowser)
                    self.prefixUI = pm.textFieldGrp(label="Prefix", text=prefix)
                    self.createProxyUI = pm.checkBoxGrp(label="Create proxy", value1=createProxy)
                    self.percentageUI = pm.floatFieldGrp(label="Percentage", value1=percentage)
                    self.createStdInUI = pm.checkBoxGrp(label="Create StandIn", value1=createStandin)
                    self.oneFilePerMeshUI = pm.checkBoxGrp(label="One File Per Mesh", value1=oneFilePerMesh)
                    self.doTransformUI = pm.checkBoxGrp(label="Use Transform", value1=useTransform)
            with pm.rowColumnLayout(numberOfColumns=2):
                pm.button(label="Create BinaryMesh", c=self.doit)
                pm.button(label="Cancel", c=self.cancel)
            
        pm.formLayout(form, edit=True, attachForm=[(StandinLayout, 'top', 5), (StandinLayout, 'bottom', 5), (StandinLayout, 'right', 5), (StandinLayout, 'left', 5)])
        pm.setUITemplate("DefaultTemplate", popTemplate=True)
            
def createStandin(*args):
    print "create standin", args
    sio = StandinOptions()
    sio.perform()

def createStandinOptions(*args):
    print "create standin options", args
    sio = StandinOptions()
    sio.show()

def readStandin(*args):
    log.debug("read standin")
    ff = "*.binarymesh"
    filename = pm.fileDialog2(fileMode=1, caption="Select binarymesh", fileFilter = ff)
    if len(filename) > 0:
        print "Reading binarymesh", filename
        bm = BinaryMesh()
        bm.path = filename[0]
        bm.loadStandins()


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
            pm.menuItem(label="Read Standin", annotation="Standin creation", command=readStandin, stp="python")

        