import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import optimizeTextures
import Appleseed.aeNodeTemplates as aet
import Appleseed.appleseedMenu as appleseedMenu
import Appleseed.appleseedShaderTools as shaderTools
import path
import tempfile

reload(Renderer)

log = logging.getLogger("mtapLogger")

#    parentForm = pm.setParent(query = True)
#    pm.setUITemplate( "attributeEditorTemplate", pushTemplate = True)
#    pm.scrollLayout( "MantraScrollLayout", horizontalScrollBarThickness = 0)
#    pm.columnLayout("MantraColumnLayout", adjustableColumn = True)
#
#    mantraAttributes.mantraGlobalsATList.createUi(mrg)
#    
#    pm.setUITemplate( "attributeEditorTemplate", popTemplate = True)
#    pm.formLayout(parentForm, edit = True, attachForm = [ ("MantraScrollLayout", "top", 0),
#                                                          ("MantraScrollLayout", "bottom", 0),
#                                                          ("MantraScrollLayout", "left", 0),
#                                                          ("MantraScrollLayout", "right", 0)
#                                                        ])


RENDERER_NAME = "Appleseed"

class AppleseedRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg=None):
        if not AppleseedRenderer.theRendererInstance:
            AppleseedRenderer.theRendererInstance = AppleseedRenderer(RENDERER_NAME , __name__)
            #AppleseedRenderer.theRendererInstance = AppleseedRenderer(RENDERER_NAME , "mtap_initialize")
        return AppleseedRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        self.aovShaders = ["mtap_aoShader", "mtap_aoVoxelShader", "mtap_diagnosticShader", "mtap_fastSSSShader", "appleseedSurfaceShader"]
        self.rendererMenu = None
        
    def createRendererMenu(self):
        self.rendererMenu = appleseedMenu.AppleseedMenu()

    def removeRendererMenu(self):
        if self.rendererMenu is not None:
            pm.deleteUI(self.rendererMenu)
        self.rendererMenu = None
            
    def getEnumList(self, attr):
        return [(i, v) for i, v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy=None):
        print "UpdateTest", dummy             

    def addUserTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("AOVs"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Environment"))    
        
    def updateEnvironment(self):
        envDict = self.rendererTabUiDict['environment']
        envType = self.renderGlobalsNode.environmentType.get()
        
        #Simply turn all off
        for key in envDict.keys():
            envDict[key].setEnable(False)
        envDict['environmentType'].setEnable(True)

        #Constant
        if envType == 0:
            envDict['environmentColor'].setEnable(True)
        #Gradient
        if envType == 1:
            envDict['gradientHorizon'].setEnable(True)
            envDict['gradientZenit'].setEnable(True)
        #Map
        if envType == 2:
            envDict['environmentMap'].setEnable(True)
        #SphericalMap
        if envType == 3:
            envDict['environmentMap'].setEnable(True)
        #Pyhsical Sky
        if envType == 4:
            envDict['pskModel'].setEnable(True)
            envDict['pskUsePhySun'].setEnable(True)
            envDict['pskSunExitMulti'].setEnable(True)
            
            if self.renderGlobalsNode.physicalSun.get():
                try:
                    sunConnection = self.renderGlobalsNode.physicalSunConnection.listConnections()[0]
                except:
                    log.info("No asSunLight connected, searching for one.")
                    try:
                        sunConnection = pm.ls("asSunLight")[0]
                        sunConnection.message >> self.renderGlobalsNode.physicalSunConnection
                    except:
                        log.info("No light called asSunLight in scene, creating a new one.")
                        lightShape = pm.createNode("directionalLight")
                        sunConnection = lightShape.getParent()
                        sunConnection.rename("asSunLight")
                        sunConnection.message >> self.renderGlobalsNode.physicalSunConnection
                
                envDict['pskPhySun'].setText(str(sunConnection))
                
            skyModel = self.renderGlobalsNode.skyModel.get()
            if skyModel == 1: #hosek
                envDict['pskGrAlbedo'].setEnable(True)
            envDict['pskGrHShit'].setEnable(True)
            envDict['pskLumMulti'].setEnable(True)
            envDict['pskSatMulti'].setEnable(True)
            if not self.renderGlobalsNode.physicalSun.get():
                envDict['pskSunAzi'].setEnable(True)
                envDict['pskSunEle'].setEnable(True)
            envDict['pskTurb'].setEnable(True)
            envDict['pskTurbMax'].setEnable(True)
            envDict['pskTurbMin'].setEnable(True)

    def AppleseedEnvironmentCreateTab(self):
        log.debug("AppleseedEnvironmentCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "AOScrollLayout"
        envDict = {}
        self.rendererTabUiDict['environment'] = envDict
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Environment Lighting", collapsable=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".environmentType")
                        envDict['environmentType'] = pm.attrEnumOptionMenuGrp(label="Environemnt Type", at=self.renderGlobalsNodeName + ".environmentType", ei=self.getEnumList(attr)) 
                        
                with pm.frameLayout(label="Environment Colors", collapsable=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        ui = pm.floatFieldGrp(label="Environemnt Intensity:", value1=1.0, numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".environmentIntensity", index=2)                    
                        envDict['environmentColor'] = pm.attrColorSliderGrp(label="Environment Color", at=self.renderGlobalsNodeName + ".environmentColor")
                        envDict['gradientHorizon'] = pm.attrColorSliderGrp(label="Gradient Horizon", at=self.renderGlobalsNodeName + ".gradientHorizon")
                        envDict['gradientZenit'] = pm.attrColorSliderGrp(label="Gradient Zenit", at=self.renderGlobalsNodeName + ".gradientZenit")
                        envDict['environmentMap'] = pm.attrColorSliderGrp(label="Environment Map", at=self.renderGlobalsNodeName + ".environmentMap")
                        envDict['latLongHShift'] = pm.floatFieldGrp(label="LatLong Horiz Shift:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['latLongHShift'], self.renderGlobalsNodeName + ".latlongHoShift", index=2)
                        envDict['latLongVShift'] = pm.floatFieldGrp(label="LatLong Vertical Shift:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['latLongVShift'], self.renderGlobalsNodeName + ".latlongVeShift", index=2)
                    
                with pm.frameLayout(label="Physical Sky", collapsable=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".skyModel")
                        envDict['pskModel'] = pm.attrEnumOptionMenuGrp(label="Sky Model", at=self.renderGlobalsNodeName + ".skyModel", ei=self.getEnumList(attr)) 
                        pm.separator()
                        envDict['pskUsePhySun'] = pm.checkBoxGrp(label="Use Physical Sun:", value1=False, cc=pm.Callback(self.uiCallback, tab="environment"))
                        pm.connectControl(envDict['pskUsePhySun'], self.renderGlobalsNodeName + ".physicalSun", index=2)                    
                        envDict['pskPhySun'] = pm.textFieldGrp(label="Sunobject:", text="", editable=False) 
                        envDict['pskSunExitMulti'] = pm.floatFieldGrp(label="sunExitance Multiplier:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSunExitMulti'], self.renderGlobalsNodeName + ".sunExitanceMultiplier", index=2)             
                        #pm.connectControl(envDict['pskPhySun'], self.renderGlobalsNodeName + ".physicalSunConnection", index=2)                                                                   
                        pm.separator()
                        envDict['pskGrAlbedo'] = pm.floatFieldGrp(label="Ground Albedo:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskGrAlbedo'], self.renderGlobalsNodeName + ".ground_albedo", index=2)                    
                        envDict['pskGrHShit'] = pm.floatFieldGrp(label="Horizon Shift:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskGrHShit'], self.renderGlobalsNodeName + ".horizon_shift", index=2)                    
                        envDict['pskLumMulti'] = pm.floatFieldGrp(label="Luminance Multiplier:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskLumMulti'], self.renderGlobalsNodeName + ".luminance_multiplier", index=2)                    
                        envDict['pskSatMulti'] = pm.floatFieldGrp(label="Saturation Multiplier:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSatMulti'], self.renderGlobalsNodeName + ".saturation_multiplier", index=2)                    
                        envDict['pskSunAzi'] = pm.floatFieldGrp(label="Sun Azimut:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSunAzi'], self.renderGlobalsNodeName + ".sun_phi", index=2)                    
                        envDict['pskSunEle'] = pm.floatFieldGrp(label="Sun Elevation:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSunEle'], self.renderGlobalsNodeName + ".sun_theta", index=2)                    
                        envDict['pskTurb'] = pm.floatFieldGrp(label="Turbidity:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskTurb'], self.renderGlobalsNodeName + ".turbidity", index=2)                    
                        envDict['pskTurbMax'] = pm.floatFieldGrp(label="Turbidity Max:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskTurbMax'], self.renderGlobalsNodeName + ".turbidity_max", index=2)                    
                        envDict['pskTurbMin'] = pm.floatFieldGrp(label="Turbidity Min:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskTurbMin'], self.renderGlobalsNodeName + ".turbidity_min", index=2)
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.environmentType, pm.Callback(self.uiCallback, tab="environment")])        
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.skyModel, pm.Callback(self.uiCallback, tab="environment")])   
        pm.scriptJob(attributeChange=[self.renderGlobalsNode.physicalSun, pm.Callback(self.uiCallback, tab="environment")])   
        
        self.updateEnvironment()     

    def AppleseedEnvironmentUpdateTab(self):
        log.debug("AppleseedEnvironmentUpdateTab()")
        self.updateEnvironment()

    def AppleseedAOVSelectCommand(self, whichField):
        log.debug("AppleseedAOVSelectCommand")
        aovDict = self.rendererTabUiDict['aovs']
        if whichField == "source":
            pm.button(aovDict['aovButton'], edit=True, enable=True, label="Add selected Shaders")
            pm.textScrollList(aovDict['aovDestField'], edit=True, deselectAll=True) 
        if whichField == "dest":
            pm.button(aovDict['aovButton'], edit=True, enable=True, label="Remove selected Shaders")
            pm.textScrollList(aovDict['aovSourceField'], edit=True, deselectAll=True) 

    def AppleseedAOVUpdateDestList(self):
        aovDict = self.rendererTabUiDict['aovs']
        pm.textScrollList(aovDict['aovDestField'], edit=True, removeAll=True) 
        aovList = self.AppleseedGetAOVConnections()
        pm.textScrollList(aovDict['aovDestField'], edit=True, append=aovList)
        
    def AppleseedAOVButtonCommand(self, args=None):
        log.debug("AppleseedAOVButtonCommand " + str(args))
        aovDict = self.rendererTabUiDict['aovs']
        label = pm.button(aovDict['aovButton'], query=True, label=True)
        if "Add selected Shaders" in label:
            log.debug("AppleseedAOVButtonCommand: adding selected shaders")
            selectedItems = pm.textScrollList(aovDict['aovSourceField'], query=True, selectItem=True) 
            for item in selectedItems:
                log.debug("Adding " + item)
                node = pm.createNode(item)
                indices = self.renderGlobalsNode.AOVs.getArrayIndices()
                index = 0
                if len(indices) > 0:
                    index = indices[-1] + 1
                node.message >> self.renderGlobalsNode.AOVs[index]
            self.AppleseedAOVUpdateDestList()
            
        if "Remove selected Shaders" in label:
            log.debug("AppleseedAOVButtonCommand: removing selected shaders")
            selectedItems = pm.textScrollList(aovDict['aovDestField'], query=True, selectItem=True) 
            for item in selectedItems:
                shaderName = item.split(" ")[0]
                log.debug("Removing " + shaderName)
                shader = pm.PyNode(shaderName)
                attribute = shader.message.outputs(p=True)[0]
                pm.delete(shader)
                attribute.remove()
            self.AppleseedAOVUpdateDestList()
            
    def AppleseedDoubleClickCommand(self):
        log.debug("AppleseedDoubleClickCommand")
        aovDict = self.rendererTabUiDict['aovs']
        selectedItems = pm.textScrollList(aovDict['aovDestField'], query=True, selectItem=True) 
        pm.select(selectedItems[0].split(" ")[0])
            
    def AppleseedGetAOVConnections(self):
        aoList = self.renderGlobalsNode.AOVs.inputs()
        aovList = []
        for aov in aoList:
            aovList.append(aov.name() + " (" + str(aov.type()) + ")")
        return aovList
            
    def AppleseedAOVsCreateTab(self):
        log.debug("AppleseedAOVsCreateTab()")
        aovDict = {}
        self.rendererTabUiDict['aovs'] = aovDict
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "AOScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout("ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="AOVs", collapsable=True, collapse=False):
                    with pm.columnLayout():
                        with pm.paneLayout(configuration="vertical2", paneSize=(1, 25, 100)):
                            aovDict['aovSourceField'] = pm.textScrollList("AOVSource", ams=True, append=self.aovShaders, selectCommand=pm.Callback(self.AppleseedAOVSelectCommand, "source"))
                            aovList = self.AppleseedGetAOVConnections()
                            aovDict['aovDestField'] = pm.textScrollList("AOVDest", append=aovList, ams=True, dcc=self.AppleseedDoubleClickCommand, selectCommand=pm.Callback(self.AppleseedAOVSelectCommand, "dest"))
                        aovDict['aovButton'] = pm.button(label="Selection", enable=False, c=self.AppleseedAOVButtonCommand)
                        
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
                    

    def AppleseedAOVsUpdateTab(self):
        log.debug("AppleseedAOVsUpdateTab()")
            
    def AppleseedRendererCreateTab(self):
        log.debug("AppleseedRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Sampling", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        ui = pm.checkBoxGrp(label="Adaptive Sampling:", value1=False, cc=self.AppleseedRendererUpdateTab)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".adaptiveSampling", index=2)
                        self.rendererTabUiDict['minSamples'] = pm.intFieldGrp(label="Samples min:", value1=2, numberOfFields=1)
                        pm.connectControl(self.rendererTabUiDict['minSamples'], self.renderGlobalsNodeName + ".minSamples", index=2)
                        ui = pm.intFieldGrp(label="Samples max:", value1=16, numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".maxSamples", index=2)
                        self.rendererTabUiDict['maxError'] = pm.floatFieldGrp(label="Max Error:", value1=0.01, numberOfFields=1)
                        pm.connectControl(self.rendererTabUiDict['maxError'], self.renderGlobalsNodeName + ".maxError", index=2)
                        pm.separator()
                        ui = pm.checkBoxGrp(label="Motionblur:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".doMotionBlur", index=2)
                        ui = pm.checkBoxGrp(label="Depth Of Field:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".doDof", index=2)
                    
                with pm.frameLayout(label="Output", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        #attr = pm.Attribute(self.renderGlobalsNodeName + ".imageFormat")
                        #ui = pm.attrEnumOptionMenuGrp(label = "Image Format", at=self.renderGlobalsNodeName + ".imageFormat", ei = self.getEnumList(attr)) 
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".bitdepth")
                        ui = pm.attrEnumOptionMenuGrp(label="Bit Depth", at=self.renderGlobalsNodeName + ".bitdepth", ei=self.getEnumList(attr)) 
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".colorSpace")
                        ui = pm.attrEnumOptionMenuGrp(label="Color Space", at=self.renderGlobalsNodeName + ".colorSpace", ei=self.getEnumList(attr)) 
                        ui = pm.checkBoxGrp(label="Clamping:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".clamping", index=2)
                    
                with pm.frameLayout(label="Filtering", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".filtertype")
                        ui = pm.attrEnumOptionMenuGrp(label="Filter Type", at=self.renderGlobalsNodeName + ".filtertype", ei=self.getEnumList(attr)) 
                        ui = pm.intFieldGrp(label="Filter Size:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".filtersize", index=2)

                with pm.frameLayout(label="Lighting", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".lightingEngine")
                        ui = pm.attrEnumOptionMenuGrp(label="Lighting Engine", at=self.renderGlobalsNodeName + ".lightingEngine", ei=self.getEnumList(attr)) 
                        ui = pm.intFieldGrp(label="Max Trace Depth:", value1=4, numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".maxTraceDepth", index=2)
                        ui = pm.checkBoxGrp(label="Caustics:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".caustics", index=2)
                        with pm.frameLayout(label="Advanced Lighting", collapsable=True, collapse=True):
                            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                                ui = pm.intFieldGrp(label="Diffuse Depth:", value1=4, numberOfFields=1)
                                pm.connectControl(ui, self.renderGlobalsNodeName + ".diffuseDepth", index=2)
                                ui = pm.intFieldGrp(label="Glossy Depth:", value1=4, numberOfFields=1)
                                pm.connectControl(ui, self.renderGlobalsNodeName + ".glossyDepth", index=2)
                                ui = pm.intFieldGrp(label="Direct Light Samples:", value1=4, numberOfFields=1)
                                pm.connectControl(ui, self.renderGlobalsNodeName + ".directLightSamples", index=2)

                    
                with pm.frameLayout(label="Renderer", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        ui = pm.intFieldGrp(label="Threads:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".threads", index=2)
                        ui = pm.intFieldGrp(label="Verbosity:", numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".rendererVerbosity", index=2)
                        ui = pm.intFieldGrp(label="Tile Size:", value1=32, numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".tilesize", index=2)
                        ui = pm.checkBoxGrp(label="Use SBVH Acceleration for mb:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".assemblySBVH", index=2)
                    
        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        
        #pm.scriptJob(attributeChange=[self.renderGlobalsNode.samplingType, pm.Callback(self.KrayRendererUIUpdateCallback, "sampling")])

        #self.updateEnvironment()
        self.AppleseedRendererUpdateTab()

    def AppleseedRendererUpdateTab(self, dummy=None):
        self.createGlobalsNode()
        #self.updateEnvironment()
        log.debug("AppleseedRendererUpdateTab()")
        if self.renderGlobalsNode.adaptiveSampling.get():
            self.rendererTabUiDict['minSamples'].setEnable(True)
            self.rendererTabUiDict['maxError'].setEnable(True)
        else:
            self.rendererTabUiDict['minSamples'].setEnable(False)
            self.rendererTabUiDict['maxError'].setEnable(False)

    def xmlFileBrowse(self, args=None):
        print "xmlfile", args
        filename = pm.fileDialog2(fileMode=0, caption="XML Export File Name")
        if len(filename) > 0:
            print "Got filename", filename
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename[0])
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])
        
    def AppleseedTranslatorCreateTab(self):
        log.debug("AppleseedTranslatorCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "TrScrollLayout"
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Translator", collapsable=True, collapse=False):
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                    ui = pm.attrEnumOptionMenuGrp(label="Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei=self.getEnumList(attr)) 
                    attr = pm.Attribute(self.renderGlobalsNodeName + ".assemblyExportType")
                    ui = pm.attrEnumOptionMenuGrp(label="Assembly Export Type", at=self.renderGlobalsNodeName + ".assemblyExportType", ei=self.getEnumList(attr))                     
                with pm.frameLayout(label="Appleseed XML export", collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export scene XML file:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportXMLFile", index=2)
                    xmlDict = {}
                    self.rendererTabUiDict['xml'] = xmlDict
                    with pm.rowColumnLayout(nc=3, width=120):
                        pm.text(label="XMLFileName:", width=60, align="right")
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".appleseed"
                        xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath, width=60)
                        pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                        pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportXMLFileName", index=2)
                with pm.frameLayout(label="Optimize Textures", collapsable=True, collapse=False):
                    with pm.rowColumnLayout(nc=3, width=120):
                        optiDict = {}
                        pm.text(label="OptimizedTex Dir:", width=60, align="right")
                        self.rendererTabUiDict['opti'] = optiDict
                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
                        optiDict['optiField'] = pm.textField(text=self.renderGlobalsNode.optimizedTexturePath.get(), width=60)
                        pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index=2)
                with pm.frameLayout(label="Additional Settings", collapsable=True, collapse=False):
                    ui = pm.floatFieldGrp(label="Scene scale:", value1=1.0, numberOfFields=1)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index=2)
                         
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])

    def AppleseedTranslatorUpdateTab(self):
        log.debug("AppleseedTranslatorUpdateTab()")

    def uiCallback(self, **args):
        log.debug("uiCallback()")
        
        if args['tab'] == "environment":
            self.updateEnvironment()
        
    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)

    def registerNodeExtensions(self):
        """Register Appleseed specific node extensions. e.g. camera type, diaphram_blades and others
        """
        # we will have a thinlens camera only
        #pm.addExtension(nodeType="camera", longName="mtap_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtap_diaphragm_blades", attributeType="long", defaultValue=0)
        pm.addExtension(nodeType="camera", longName="mtap_diaphragm_tilt_angle", attributeType="float", defaultValue=0.0)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtap_mesh_useassembly", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="mesh", longName="mtap_ray_bias_method", attributeType="enum", enumName="none:normal:incoming_direction:outgoing_direction", defaultValue=0)
        pm.addExtension(nodeType="mesh", longName="mtap_ray_bias_distance", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="mesh", longName="mtap_standin_path", dataType="string", usedAsFilename=True)
        
        # shading group
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_bsdf", attributeType="message")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_surfaceShader", attributeType="message")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_edf", attributeType="message")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_alphaMap", usedAsColor=True, attributeType="float3")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_alphaMapR",attributeType = "float", parent="mtap_mat_alphaMap")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_alphaMapG",attributeType = "float", parent="mtap_mat_alphaMap")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_alphaMapB",attributeType = "float", parent="mtap_mat_alphaMap")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_normalMap", usedAsColor=True, attributeType="float3")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_normalMapR",attributeType = "float", parent="mtap_mat_normalMap")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_normalMapG",attributeType = "float", parent="mtap_mat_normalMap")
#        pm.addExtension(nodeType="shadingEngine", longName="mtap_mat_normalMapB",attributeType = "float", parent="mtap_mat_normalMap")


        # 
            
    def removeLogFile(self):
        logfile = pm.workspace.path + "/applelog.log"
        try:
            if os.path.exists(logfile):
                os.remove(logfile)
        except:
            pass

    def showLogFile(self):
        logfile = pm.workspace.path + "/applelog.log"
        if os.path.exists(logfile):
            lh = open(logfile, 'r')
            rl = lh.readlines()
            for l in rl:
                sys.__stdout__.write(l)
            
    def renderProcedure(self, width, height, doShadows, doGlow, camera, options):
        log.debug("renderProcedure")
        self.removeLogFile()
        print "renderProcedure", width, height, doShadows, doGlow, camera, options
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        
        if pm.about(batch=True):
            pm.mayatoappleseed()
        else:
            pm.mayatoappleseed(width=width, height=height, camera=camera)
            
        if not self.ipr_isrunning:
            self.showLogFile()
        self.postRenderProcedure()
            
        
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        print "startIprRenderProcedure", editor, resolutionX, resolutionY, camera
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        pm.mayatoappleseed(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoappleseed(stopIpr=True)
        self.postRenderProcedure()
            
    def preRenderProcedure(self):
        self.createGlobalsNode()
        if self.renderGlobalsNode.threads.get() == 0:
            #TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
            try:
                optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
            except:
                optimizedPath = path.path(tempfile.gettempdir()) / "optimizedTextures"
            if not os.path.exists(optimizedPath):
                optimizedPath.makedirs()
            self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))

        #craete optimized exr textures
        optimizeTextures.preRenderOptimizeTextures(optimizedFilePath=self.renderGlobalsNode.optimizedTexturePath.get())
        shaderTools.createAutoShaderNodes()
        
    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        shaderTools.removeAutoShaderNodes()

    def afterGlobalsNodeReplacement(self):
        log.debug("afterGlobalsNodeReplacement")        
        self.rendererTabUiDict = {}

    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AEappleseedNodeTemplate(nodeName)

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    rendererName = "Appleseed"
    aeDir = path.path(__file__).dirname() + "/" + rendererName +"/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            #log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

def loadPlugins():
    try:
        log.debug("Loading Appleseed maya plugins")
        version = pm.about(v=True).split(" ")[0]
        pluginName = "appleseedTools_maya{0}".format(version)
        log.debug("Trying to load appleseedTools: {0}".format(pluginName))
        if not pm.pluginInfo(pluginName, query=True, loaded=True):
            pm.loadPlugin(pluginName)
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Load plugins Appleseed FAILED")
    
        
def theRenderer():
    return AppleseedRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Appleseed")
        theRenderer().registerRenderer()
        loadAETemplates()
        theRenderer().createRendererMenu()
        loadPlugins()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Appleseed FAILED")
        
def unregister():
    theRenderer().removeRendererMenu()
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

def uiCallback(*args):
    theRenderer().uiCallback(args)

