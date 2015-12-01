import pymel.core as pm
import logging
import Renderer as Renderer
import traceback
import sys
import os
import mtap_optimizeTextures
import Appleseed.aeNodeTemplates as aet
import Appleseed.appleseedMenu as appleseedMenu
import Appleseed.appleseedShaderTools as shaderTools
import path
import tempfile
import maya.cmds as cmds
import Renderer.OSLTools as OSLTools

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
            # AppleseedRenderer.theRendererInstance = AppleseedRenderer(RENDERER_NAME , "mtap_initialize")
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
        
        # Simply turn all off
        for key in envDict.keys():
            envDict[key].setEnable(False)
        envDict['environmentType'].setEnable(True)

        # Constant
        if envType == 0:
            envDict['environmentColor'].setEnable(True)
        # Gradient
        if envType == 1:
            envDict['gradientHorizon'].setEnable(True)
            envDict['gradientZenit'].setEnable(True)
        # Map
        if envType == 2:
            envDict['environmentMap'].setEnable(True)
            envDict['latlongVeShift'].setEnable(True)
            envDict['latlongHoShift'].setEnable(True)
            
        # SphericalMap
        if envType == 3:
            envDict['environmentMap'].setEnable(True)
            envDict['latlongVeShift'].setEnable(True)
            envDict['latlongHoShift'].setEnable(True)
        # Pyhsical Sky
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
            if skyModel == 1:  # hosek
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
        uiDict = envDict
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
                        self.addRenderGlobalsUIElement(attName='latlongHoShift', uiType='float', displayName='LatLong Horiz Shift:', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='latlongVeShift', uiType='float', displayName='LatLong Vertical Shift:', uiDict=uiDict)
#                         envDict['latLongHShift'] = pm.floatFieldGrp(label="LatLong Horiz Shift:", value1=1.0, numberOfFields=1)
#                         pm.connectControl(envDict['latLongHShift'], self.renderGlobalsNodeName + ".latlongHoShift", index=2)
#                         envDict['latLongVShift'] = pm.floatFieldGrp(label="LatLong Vertical Shift:", value1=1.0, numberOfFields=1)
#                         pm.connectControl(envDict['latLongVShift'], self.renderGlobalsNodeName + ".latlongVeShift", index=2)
                    
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
                        # pm.connectControl(envDict['pskPhySun'], self.renderGlobalsNodeName + ".physicalSunConnection", index=2)                                                                   
                        pm.separator()
                        envDict['pskGrAlbedo'] = pm.floatFieldGrp(label="Ground Albedo:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskGrAlbedo'], self.renderGlobalsNodeName + ".ground_albedo", index=2)                    
                        envDict['pskGrHShit'] = pm.floatFieldGrp(label="Horizon Shift:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskGrHShit'], self.renderGlobalsNodeName + ".horizon_shift", index=2)                    
                        envDict['pskLumMulti'] = pm.floatFieldGrp(label="Luminance Multiplier:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskLumMulti'], self.renderGlobalsNodeName + ".luminance_multiplier", index=2)                    
                        envDict['pskSatMulti'] = pm.floatFieldGrp(label="Saturation Multiplier:", value1=1.0, numberOfFields=1)
                        pm.connectControl(envDict['pskSatMulti'], self.renderGlobalsNodeName + ".saturation_multiplier", index=2)                    
                        # envDict['pskSunAzi'] = pm.floatFieldGrp(label="Sun Azimut:", value1=1.0, numberOfFields=1)
                        # pm.connectControl(envDict['pskSunAzi'], self.renderGlobalsNodeName + ".sun_phi", index=2)                    
                        # envDict['pskSunEle'] = pm.floatFieldGrp(label="Sun Elevation:", value1=1.0, numberOfFields=1)
                        # pm.connectControl(envDict['pskSunEle'], self.renderGlobalsNodeName + ".sun_theta", index=2)                    
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
            
    def AppleseedCommonGlobalsCreateTab(self):
        self.OpenMayaCommonGlobalsCreateTab()            

    def AppleseedCommonGlobalsUpdateTab(self):
        self.OpenMayaCommonGlobalsUpdateTab()    
                    
    def AppleseedRendererCreateTab(self):
        log.debug("AppleseedRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("renderGlobalsTemplate", pushTemplate=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')        
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict

        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                with pm.frameLayout(label="Pixel Sampler", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        # attr = pm.Attribute(self.renderGlobalsNodeName + ".pixel_renderer")
                        # ui = pm.attrEnumOptionMenuGrp(label="Pixel Sampler", at=self.renderGlobalsNodeName + ".pixel_renderer", ei=self.getEnumList(attr)) 
                        self.addRenderGlobalsUIElement(attName='pixel_renderer', uiType='enum', displayName='Pixel Sampler', default='0', uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                        self.addRenderGlobalsUIElement(attName='minSamples', uiType='int', displayName='Min Samples', default=False, uiDict=uiDict)                        
                        self.addRenderGlobalsUIElement(attName='maxSamples', uiType='int', displayName='Max Samples', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='maxError', uiType='float', displayName='Max Error', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='enable_diagnostics', uiType='bool', displayName="Diagnostic AOV's", default=False, uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='frameRendererPasses', uiType='int', displayName='Passes', uiDict=uiDict)
                        

                with pm.frameLayout(label="Filtering", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".filtertype")
                        ui = pm.attrEnumOptionMenuGrp(label="Filter Type", at=self.renderGlobalsNodeName + ".filtertype", ei=self.getEnumList(attr)) 
                        self.addRenderGlobalsUIElement(attName='filtersize', uiType='int', displayName='Filter Size', default=False, uiDict=uiDict)
                        
                with pm.frameLayout(label="Features", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='doMotionBlur', uiType='bool', displayName='Motion Blur', default=False, uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='doDof', uiType='bool', displayName='Depth Of Field', default=False, uiDict=uiDict)
                    
                with pm.frameLayout(label="Output", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".bitdepth")
                        ui = pm.attrEnumOptionMenuGrp(label="Bit Depth", at=self.renderGlobalsNodeName + ".bitdepth", ei=self.getEnumList(attr)) 
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".colorSpace")
                        ui = pm.attrEnumOptionMenuGrp(label="Color Space", at=self.renderGlobalsNodeName + ".colorSpace", ei=self.getEnumList(attr)) 
                        ui = pm.checkBoxGrp(label="Clamping:", value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".clamping", index=2)
                    
                with pm.frameLayout(label="Lighting Engine", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='lightingEngine', uiType='enum', displayName='Lighting Engine', default='0', uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                        with pm.frameLayout(label="Lighting Engine Settings", collapsable=True, collapse=False) as uiDict['LE_framelayout']:
                            with pm.columnLayout(self.rendererName + "LEColumnLayout", adjustableColumn=True, width=400) as uiDict['LE_layout']:
                                pass
                            
                with pm.frameLayout(label="Renderer", collapsable=True, collapse=True):
                    with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='threads', uiType='int', displayName='Threads:', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='rendererVerbosity', uiType='int', displayName='Verbosity:', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='tilesize', uiType='int', displayName='Tile Size:', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='texCacheSize', uiType='int', displayName='Tex Cache Size (kb):', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='assemblySBVH', uiType='bool', displayName='Use SBVH Acc. for mb:', uiDict=uiDict)                        
                    
        pm.setUITemplate("renderGlobalsTemplate", popTemplate=True)                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        
        # pm.scriptJob(attributeChange=[self.renderGlobalsNode.samplingType, pm.Callback(self.KrayRendererUIUpdateCallback, "sampling")])

        # self.updateEnvironment()
        self.AppleseedRendererUpdateTab()

    def AppleseedRendererUpdateTab(self, dummy=None):
        self.createGlobalsNode()
        log.debug("AppleseedRendererUpdateTab()")
        if not self.rendererTabUiDict.has_key('common'):
            return
        
        uiDict = self.rendererTabUiDict['common']

        if self.renderGlobalsNode.pixel_renderer.get() == 0:
            uiDict['maxSamples'].setLabel("Max Samples")
            uiDict['maxSamples'].setEnable(True)
            uiDict['minSamples'].setManage(True)
            uiDict['maxError'].setManage(True)
            
        if self.renderGlobalsNode.pixel_renderer.get() == 1:
            uiDict['minSamples'].setManage(False)
            uiDict['maxError'].setManage(False)
            uiDict['maxSamples'].setLabel("Samples")
                    
        pm.deleteUI(uiDict['LE_layout'])
        with pm.columnLayout(self.rendererName + "LEColumnLayout", adjustableColumn=True, width=400, parent=uiDict['LE_framelayout']) as uiDict['LE_layout']:                
            if self.renderGlobalsNode.lightingEngine.get() == 0:  # path tracer
                self.addRenderGlobalsUIElement(attName='enable_ibl', uiType='bool', displayName='Enable IBL', default=False, uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                self.addRenderGlobalsUIElement(attName='enable_caustics', uiType='bool', displayName='Enable Caustics', default=False, uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='enable_dl', uiType='bool', displayName='Enable Direct Light', default=False, uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                pm.separator()
                self.addRenderGlobalsUIElement(attName='environmentSamples', uiType='int', displayName='Environment Samples', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='directLightSamples', uiType='int', displayName='Direct Light Samples', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='max_ray_intensity', uiType='float', displayName='Max Ray Intensity', anno='Clamp intensity of rays (after the first bounce) to this value to reduce fireflies', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='max_path_length', uiType='float', displayName='Max Bounces', uiDict=uiDict)                        
                if not self.renderGlobalsNode.enable_ibl.get():
                    uiDict['environmentSamples'].setEnable(False)
                else:
                    uiDict['environmentSamples'].setEnable(True)
                if not self.renderGlobalsNode.enable_dl.get():
                    uiDict['directLightSamples'].setEnable(False)
                else:
                    uiDict['directLightSamples'].setEnable(True)
                
            if self.renderGlobalsNode.lightingEngine.get() == 1:  # distributed ray tracer
                self.addRenderGlobalsUIElement(attName='enable_ibl', uiType='bool', displayName='Enable IBL', default=False, uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                pm.separator()
                self.addRenderGlobalsUIElement(attName='environmentSamples', uiType='int', displayName='Environment Samples', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='directLightSamples', uiType='int', displayName='Direct Light Samples', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='maxTraceDepth', uiType='int', displayName='Max Bounces', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='rr_min_path_length', uiType='float', displayName='RR Start Bounce', anno='Consider pruning low contribution paths starting with this bounce', uiDict=uiDict)                        
                if not self.renderGlobalsNode.enable_ibl.get():
                    uiDict['environmentSamples'].setEnable(False)
                else:
                    uiDict['environmentSamples'].setEnable(True)
                    
            if self.renderGlobalsNode.lightingEngine.get() == 2:  # sppm
                self.addRenderGlobalsUIElement(attName='enable_ibl', uiType='bool', displayName='Enable IBL', default=False, uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                self.addRenderGlobalsUIElement(attName='enable_caustics', uiType='bool', displayName='Enable Caustics', default=False, uiDict=uiDict)
                pm.separator()
                self.addRenderGlobalsUIElement(attName='dl_mode', uiType='enum', displayName='Direct Lighting Mode', default='0', uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                pm.separator()
                self.addRenderGlobalsUIElement(attName='path_tracing_max_path_length', uiType='int', displayName='Max Bounces', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='light_photons_per_pass', uiType='int', displayName='Light Photons PP', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='env_photons_per_pass', uiType='int', displayName='Env Photons PP', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='max_photons_per_estimate', uiType='int', displayName='Max Photons Per Estimate', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='photon_type', uiType='enum', displayName='Photon Type', default='0', uiDict=uiDict, callback=self.AppleseedRendererUpdateTab)
                self.addRenderGlobalsUIElement(attName='photon_tracing_rr_min_path_length', uiType='int', displayName='Min Bounces', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='photon_tracing_max_path_length', uiType='int', displayName='Max Bounces', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='photons_per_pass', uiType='int', displayName='Photons Per Pass', uiDict=uiDict)
                self.addRenderGlobalsUIElement(attName='initial_radius', uiType='float', displayName='Initial Radius', anno='Initial photon gathering radius in percent of the scene diameter.', uiDict=uiDict)                        
                self.addRenderGlobalsUIElement(attName='sppmAlpha', uiType='float', displayName='Alpha', anno='Evolution rate of photon gathering radius', uiDict=uiDict)                        
            
                    
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
                
                with pm.frameLayout(label="{0} export".format(self.rendererName), collapsable=True, collapse=False):
                    ui = pm.checkBoxGrp(label="Export {0} Scene file:".format(self.rendererName), value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index=2)
#                     xmlDict = {}
#                     self.rendererTabUiDict['xml'] = xmlDict
#                     defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".appleseed"
#                     if not defaultXMLPath.dirname().exists():
#                         defaultXMLPath.dirname().makedirs()
#                     with pm.rowLayout(nc=3):
#                         xmlDict['xmlFileText'] = pm.text(label="Export to")
#                         xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath)
#                         pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
#                         pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index=2)
                        
                with pm.frameLayout(label="Optimize Textures", collapsable=True, collapse=False):
                    optiDict = {}
                    ui = pm.checkBoxGrp(label="Use Optimized Textures:", value1=False)
                    pm.connectControl(ui, self.renderGlobalsNodeName + ".useOptimizedTextures", index=2)
                    with pm.rowLayout(nc=3):
                        self.rendererTabUiDict['opti'] = optiDict
                        pm.text(label="OptimizedTex Dir:")
                        optiDict['optiField'] = pm.textField(text=self.renderGlobalsNode.optimizedTexturePath.get())
                        pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
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
        # pm.addExtension(nodeType="camera", longName="mtap_cameraType", attributeType="enum", enumName="Pinhole:Thinlens", defaultValue = 0)
        pm.addExtension(nodeType="camera", longName="mtap_diaphragm_blades", attributeType="long", defaultValue=0)
        pm.addExtension(nodeType="camera", longName="mtap_diaphragm_tilt_angle", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="camera", longName="mtap_latLongLens", attributeType="bool", defaultValue=False)
        
        # mesh
        pm.addExtension(nodeType="mesh", longName="mtap_mesh_useassembly", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="mesh", longName="mtap_ray_bias_method", attributeType="enum", enumName="none:normal:incoming_direction:outgoing_direction", defaultValue=0)
        pm.addExtension(nodeType="mesh", longName="mtap_ray_bias_distance", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="mesh", longName="mtap_standin_path", dataType="string", usedAsFilename=True)
        pm.addExtension(nodeType="mesh", longName="mtap_visibleLights", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="mesh", longName="mtap_visibleProbe", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="mesh", longName="mtap_visibleGlossy", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="mesh", longName="mtap_visibleSpecular", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="mesh", longName="mtap_visibleDiffuse", attributeType="bool", defaultValue=True)

        pm.addExtension(nodeType="spotLight", longName="mtap_cast_indirect_light", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="spotLight", longName="mtap_importance_multiplier", attributeType="float", defaultValue=1.0)
        pm.addExtension(nodeType="directionalLight", longName="mtap_cast_indirect_light", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="directionalLight", longName="mtap_importance_multiplier", attributeType="float", defaultValue=1.0)
        pm.addExtension(nodeType="pointLight", longName="mtap_cast_indirect_light", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="pointLight", longName="mtap_importance_multiplier", attributeType="float", defaultValue=1.0)
        
        pm.addExtension(nodeType="areaLight", longName="primaryVisibility", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="castsShadows", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleLights", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleProbe", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleGlossy", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleSpecular", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleDiffuse", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="areaLight", longName="mtap_visibleTransparency", attributeType="bool", defaultValue=True)
        
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

    # this method only exists because I was unable to write the logfile during rendering
    # it will be called from the plugin
    def showLogFile(self, logfile):
        log.debug("Trying to open logfile: {0}".format(logfile))
        if os.path.exists(logfile):
            lh = open(logfile, 'r')
            rl = lh.readlines()
            for l in rl:
                sys.__stdout__.write(l)
        else:
            print "Logfile", logfile, "not found"
            
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
            # TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
            # lets reduce the number of threads to stay as interactive as possible
            if self.ipr_isrunning:
                if numThreads > 4:
                    numThreads = numThreads - 1
            
        if self.renderGlobalsNode.useOptimizedTextures.get():
            if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
                try:
                    optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
                except:
                    optimizedPath = path.path(tempfile.gettempdir()) / "optimizedTextures"
                if not os.path.exists(optimizedPath):
                    optimizedPath.makedirs()
                self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
    
            # craete optimized exr textures
            mtap_optimizeTextures.preRenderOptimizeTextures(optimizedFilePath=self.renderGlobalsNode.optimizedTexturePath.get())
            shaderTools.createAutoShaderNodes()
        
        OSLTools.compileAllShaders()
        
    def postRenderProcedure(self):
        mtap_optimizeTextures.postRenderOptimizeTextures()
        shaderTools.removeAutoShaderNodes()

    def afterGlobalsNodeReplacement(self):
        log.debug("afterGlobalsNodeReplacement")        
        self.rendererTabUiDict = {}

    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AEappleseedNodeTemplate(nodeName)
        
    def connectNodeToNodeOverrideCallback(self, srcNode, destNode):
        log.debug("connectNodeToNodeOverrideCallback {0} {1}".format(srcNode, destNode))
        dn = pm.PyNode(destNode)
        sn = pm.PyNode(srcNode)
#         if dn.type() in ["CoronaSurface"]:
#             if sn.hasAttr("outColor"):
#                 sn.outColor >> dn.diffuse
#                 return 0
#             if sn.type() == "CoronaRoundCorners":
#                 sn.outNormal >> dn.roundCornersMap
#                 return 0
        return 1
    
    def createRenderNode(self, nodeType=None, postCommand=None):
        print "createRenderNode nodeType ", nodeType, "postCommand", postCommand
        log.debug("createRenderNode callback for renderer {0} with node: {1}".format(self.rendererName.lower(), nodeType))
        nodeClass = None
        rendererName = self.rendererName.lower()
        for cl in pm.getClassification(nodeType):
            if (rendererName+"/material") in cl.lower():
                nodeClass = "shader"
            if (rendererName + "/texture") in cl.lower():
                nodeClass = "texture"
                
        if nodeClass == "shader":
            mat = pm.shadingNode(nodeType, asShader=True)
            shadingGroup = pm.sets(renderable=True, noSurfaceShader=True, empty=True, name="{0}SG".format(mat))
            mat.outColor >> shadingGroup.surfaceShader
        else:
            mat = pm.shadingNode(nodeType, asTexture=True)
            
        if postCommand is not None:
            postCommand = postCommand.replace("%node", str(mat))
            postCommand = postCommand.replace("%type", '\"\"')
            pm.mel.eval(postCommand)
        return ""
        

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    rendererName = "Appleseed"
    aeDir = path.path(__file__).dirname() + "/" + rendererName + "/AETemplate/"
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            # log.debug("load aeTemplate: " + templateName + " : " + melCommand)
            pm.mel.eval(melCommand)

def loadPlugins():
    # temporarily decativated
    return
#     try:
#         log.debug("Loading Appleseed maya plugins")
#         version = pm.about(v=True).split(" ")[0]
#         pluginName = "appleseedTools_maya{0}".format(version)
#         log.debug("Trying to load appleseedTools: {0}".format(pluginName))
#         if not pm.pluginInfo(pluginName, query=True, loaded=True):
#             pm.loadPlugin(pluginName)
#     except:
#         traceback.print_exc(file=sys.__stderr__)
#         log.error("Load plugins Appleseed FAILED")
    
        
def theRenderer():
    return AppleseedRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Appleseed")
        theRenderer().registerRenderer()
        if not pm.about(batch=True):
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

