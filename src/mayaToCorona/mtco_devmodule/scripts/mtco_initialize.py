import pymel.core as pm
import logging
import traceback
import sys
import os
import path
import time
import Renderer as Renderer
import optimizeTextures
import Corona.aeNodeTemplates as aet
import Corona.passesUI
import Corona.menu as coronaMenu

reload(Renderer)

log = logging.getLogger("mtCoronaLogger")


RENDERER_NAME = "Corona"

class CoronaRenderer(Renderer.MayaToRenderer):
    theRendererInstance = None
    @staticmethod
    def theRenderer(arg=None):
        if not CoronaRenderer.theRendererInstance:
            CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , __name__)
            # CoronaRenderer.theRendererInstance = CoronaRenderer(RENDERER_NAME , "mtco_initialize")
        return CoronaRenderer.theRendererInstance
    
    def __init__(self, rendererName, moduleName):
        Renderer.MayaToRenderer.__init__(self, rendererName, moduleName)
        self.rendererTabUiDict = {}
        if not pm.pluginInfo("coronaPassesNode.py", q=True, loaded=True):
            pm.loadPlugin("coronaPassesNode.py")
        
    def logoCallbackProcedure(self):
        pass
         
    def createRendererMenu(self):
        if not pm.about(batch=True):
            self.rendererMenu = coronaMenu.CoronaMenu()
    
    def removeRendererMenu(self):
        if self.rendererMenu is not None:
            pm.deleteUI(self.rendererMenu)
        self.rendererMenu = None
    
    def getEnumList(self, attr):
        return [(i, v) for i, v in enumerate(attr.getEnums().keys())]

    def updateTest(self, dummy=None):
        pass

    def updateEnvironment(self, dummy=None):
        pass

    def CoronaCommonGlobalsCreateTab(self):
        self.OpenMayaCommonGlobalsCreateTab()            

    def CoronaCommonGlobalsUpdateTab(self):
        self.OpenMayaCommonGlobalsUpdateTab()            

    def disConnectGlobalVolume(self, *args):
        if not self.rendererTabUiDict.has_key('common'):
            return        
        uiDict = self.rendererTabUiDict['common']    
        
        if self.renderGlobalsNode.globalVolume.isConnected():
            inConnection = self.renderGlobalsNode.globalVolume.inputs(p=True)[0]
            inConnection // self.renderGlobalsNode.globalVolume
        self.updateGlobalVolume()

    def connectGlobalVolume(self, *args):
        if not self.rendererTabUiDict.has_key('common'):
            return        
        uiDict = self.rendererTabUiDict['common']    
         
        volumeShader = pm.createNode("CoronaVolume")
        volumeShader.outColor >> self.renderGlobalsNode.globalVolume         
        self.updateGlobalVolume()
        
    def selectGlobalVolume(self, *args):
        if self.renderGlobalsNode.globalVolume.isConnected():
            pm.select(self.renderGlobalsNode.globalVolume.inputs()[0])
            
        
    def updateGlobalVolume(self):
        if not self.rendererTabUiDict.has_key('common'):
            return        
        uiDict = self.rendererTabUiDict['common']    
        
        uiDict['globalVolumePopUp'].deleteAllItems()
        if self.renderGlobalsNode.globalVolume.isConnected():
            inputNode = self.renderGlobalsNode.globalVolume.inputs()[0]
            uiDict['globalVolumeText'].setText(inputNode)
            uiDict['globalVolumePopUpConnect'] = pm.menuItem(label="Disconnect" + inputNode, parent=uiDict['globalVolumePopUp'], command = self.disConnectGlobalVolume)
            uiDict['globalVolumePopUpSelect'] = pm.menuItem(label="Select VolumeShader", parent=uiDict['globalVolumePopUp'], command = self.selectGlobalVolume) 
        else:
            uiDict['globalVolumeText'].setText("")
            uiDict['globalVolumePopUpConnect'] = pm.menuItem(label="Connect new Volume Shader", parent=uiDict['globalVolumePopUp'], command = self.connectGlobalVolume)
            
#             uiDict['globalVolumeButton'].setLabel("Disconnect {0}".format(inputNode))
#         else:
#             uiDict['globalVolumeButton'].setLabel("Connect Volume Shader.")
    def dumpFramebuffer(self, dummy): 
        log.debug("dumpFramebuffer()")
        filter = "*.exr"
        filename = pm.fileDialog2(fileMode=0, ff=filter, caption="Export Corona Framebuffer...")
        if filename and len(filename) > 0:
            filename = filename[0]
            print "Filename:", filename
            self.renderGlobalsNode.dumpExrFile.set(filename)
            uiDict = self.rendererTabUiDict['common']
            pm.mayatoCorona(usrEvent=1, usrDataString=filename)
                
    def resumeFramebuffer(self, dummy): 
        log.debug("resumeFramebuffer()")
        filter = "*.exr"
        filename = pm.fileDialog2(fileMode=1, ff=filter, caption="Export Corona Framebuffer...")
        if filename and len(filename) > 0:
            filename = filename[0]
            print "Filename:", filename
            self.renderGlobalsNode.dumpExrFile.set(filename)
            uiDict = self.rendererTabUiDict['common']
        
    def CoronaSceneCreateTab(self):
        log.debug("CoronaRendererCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')        
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400):                
                with pm.frameLayout(label="Renderer", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='renderer', uiType='enum', displayName='Renderer', default='Progressive', data='Progressive:Bucket', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        pm.separator()
                        with pm.columnLayout(self.rendererName + 'ColumnLayoutPRO', adjustableColumn=True, width=400) as uiDict['progressiveCL'] :                        
                            self.addRenderGlobalsUIElement(attName='progressive_maxPasses', uiType='int', displayName='Maxpasses', default='50', data='minmax:0:9999', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='progressive_timeLimit', uiType='int', displayName='Timelimit', default='60', uiDict=uiDict)
                        with pm.columnLayout(self.rendererName + 'ColumnLayoutBUCK', adjustableColumn=True, width=400) as uiDict['bucketCL'] :                        
                            self.addRenderGlobalsUIElement(attName='image_bucketSize', uiType='int', displayName='Tile Size', default=64, uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='buckets_initialSamples', uiType='int', displayName='Initialsamples', default='1', data='minmax:1:999', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='buckets_adaptiveSteps', uiType='int', displayName='Adaptivesteps', default='2', data='minmax:1:10', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='buckets_adaptiveThreshold', uiType='float', displayName='Adaptive Threshold', default='0.03', data='minmax:0.001:1.0', uiDict=uiDict)
                        with pm.columnLayout(self.rendererName + 'ColumnLayoutVCM', adjustableColumn=True, width=400) as uiDict['VCMCL'] :                        
                            self.addRenderGlobalsUIElement(attName='vcm_mode', uiType='enum', displayName='VCM Mode', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='bidir_doMis', uiType='bool', displayName='Use MIS', default='true', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='ppm_photonsPerIter', uiType='int', displayName='Photons Per Iteration(k)', default='5000', data='minmax:1000:99000000', uiDict=uiDict)
                            # self.addRenderGlobalsUIElement(attName = 'ppm_samplesPerIter', uiType = 'int', displayName = 'Ppm_samplesperiter', default='1', data='minmax:1:50', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='ppm_initialRadius', uiType='float', displayName='Initial Radius', default='2.0', data='minmax:0.0001:200.0', uiDict=uiDict)
                            self.addRenderGlobalsUIElement(attName='ppm_alpha', uiType='float', displayName='Alpha (radius reduction)', default='0.666', data='minmax:0.01:1.0', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'dumpAndResume', uiType = 'bool', displayName = 'Dump and Resume', default='false', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        with pm.columnLayout(self.rendererName + 'ColumnLayoutDUMP', rs=3, adjustableColumn=True, width=400) as uiDict['ColumnLayoutDUMP'] :                        
                            uiDict['dumpFramebuffer'] = pm.button(label="Dump Framebuffer...", c=self.dumpFramebuffer)
                            uiDict['resumeFramebuffer'] = pm.button(label="Resume from Dump...", c=self.resumeFramebuffer)
                            self.addRenderGlobalsUIElement(attName = 'dumpExrFile', uiType = 'string', displayName = 'Saved Render File', default='""', uiDict=uiDict)
                            #uiDict['dumpFilePath']=pm.textFieldGrp(label="Resume from:", editable=False)
                            uiDict['dumpExrFile'].setEditable(False)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'useGlobalMaterialOverride', uiType = 'bool', displayName = 'Use Material Override', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'globalMaterialOverride', uiType = 'color', displayName = 'Global Material Override', uiDict=uiDict)
                        
                with pm.frameLayout(label="Camera & Color Mapping", collapsable=True, collapse=True):                    
                    with pm.columnLayout(self.rendererName + 'ColumnLayoutCCMC', adjustableColumn=True, width=400):
                        with pm.frameLayout(label="Camera", collapsable=True, collapse=False):                                        
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName = 'exposure_type', uiType = 'enum', displayName = 'Exposure Type', default='0', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                                with pm.columnLayout(self.rendererName + 'sexColumnLayout', adjustableColumn=True, width=400) as uiDict["simpleExposureLO"]:
                                    self.addRenderGlobalsUIElement(attName = 'colorMapping_simpleExposure', uiType = 'float', displayName = 'Simple Exposure', default='1.0', uiDict=uiDict)
                                with pm.columnLayout(self.rendererName + 'pexColumnLayout', adjustableColumn=True, width=400) as uiDict["photographicExposureLO"]:
                                    self.addRenderGlobalsUIElement(attName = 'colorMapping_fStop', uiType = 'float', displayName = 'FStop', uiDict=uiDict)
                                    self.addRenderGlobalsUIElement(attName = 'colorMapping_iso', uiType = 'float', displayName = 'ISO', uiDict=uiDict)
                                    self.addRenderGlobalsUIElement(attName = 'colorMapping_shutterSpeed', uiType = 'float', displayName = 'Shutter Speed', uiDict=uiDict)
                                with pm.columnLayout(self.rendererName + 'dofMbColumnLayout', adjustableColumn=True, width=400):
                                    self.addRenderGlobalsUIElement(attName='doDof', uiType='bool', displayName='Depth of Field', default='True', uiDict=uiDict)
                                    self.addRenderGlobalsUIElement(attName='doMotionBlur', uiType='bool', displayName='Motion Blur', default='True', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                                    with pm.columnLayout(self.rendererName + 'mbsetColumnLayout', adjustableColumn=True, width=400) as uiDict['mbSettingsFrame']:
                                        self.addRenderGlobalsUIElement(attName='xftimesamples', uiType='int', displayName='Transform Samples:', default='2', uiDict=uiDict)
                                        self.addRenderGlobalsUIElement(attName='geotimesamples', uiType='int', displayName='Geo Samples:', default='2', uiDict=uiDict)
                                    
                        with pm.frameLayout(label="Color Mapping", collapsable=True, collapse=False):                                        
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                #self.addRenderGlobalsUIElement(attName = 'colorMapping_gamma', uiType = 'float', displayName = 'Colormapping_gamma', default='2.2', data='minmax:0.01:10.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'colorMapping_highlightCompression', uiType = 'float', displayName = 'Highlight Compression', default='1.0', data='minmax:0.01:99.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'colorMapping_colorTemperature', uiType = 'float', displayName = 'Color Temperature', default='6500.0', data='minmax:1000.0:99999.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'colorMapping_tint', uiType = 'color', displayName = 'Tint', default='1.0:1.0:1.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'colorMapping_contrast', uiType = 'float', displayName = 'Contrast', default='1.0', data='minmax:1.0:99.0', uiDict=uiDict)

                with pm.frameLayout(label="Environment", collapsable=True, collapse=True):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName=  'bgColor', uiType='color', displayName='Main Environment', default='0.4:0.4:1.0', uiDict=uiDict)                        
                        self.addRenderGlobalsUIElement(attName = 'useGlobalDirectOverride', uiType = 'bool', displayName = 'Use Direct Override', default='false', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'globalDirectOverride', uiType = 'color', displayName = 'Direct Override', default='0.0:0.0:0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'useGlobalReflectionOverride', uiType = 'bool', displayName = 'Use Reflection Override', default='false', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'globalReflectionOverride', uiType = 'color', displayName = 'Reflection Override', default='0.0:0.0:0.0', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'useGlobalRefractionOverride', uiType = 'bool', displayName = 'Use Refraction Override', default='false', uiDict=uiDict, callback=self.CoronaSceneUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'globalRefractionOverride', uiType = 'color', displayName = 'Refraction Override', default='0.0:0.0:0.0', uiDict=uiDict)

                        uiDict['globalVolumeText'] = pm.textFieldGrp(label = "Global Volume", editable=False)
                        uiDict['globalVolumePopUp'] = pm.popupMenu(button=1)
                        self.updateGlobalVolume()


#                 with pm.frameLayout(label="Features", collapsable=True, collapse=True):
#                     with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
#                         self.addRenderGlobalsUIElement(attName='doShading', uiType='bool', displayName='Do Shading', default='true', uiDict=uiDict)
#                         # self.addRenderGlobalsUIElement(attName = 'random_sampler', uiType = 'enum', displayName = 'Random_Sampler', default='5d_highd', data='5d_highd:Shared:Maximal_value', uiDict=uiDict)
#                         self.addRenderGlobalsUIElement(attName = 'useCoronaVFB', uiType = 'bool', displayName = 'Use Corona VFB', uiDict=uiDict)
                    
                    
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.CoronaSceneUpdateTab()

    def CoronaSceneUpdateTab(self, dummy=None):
        log.debug("Update corona renderer tab")
        self.createGlobalsNode()
        self.updateEnvironment()

        if not self.rendererTabUiDict.has_key('common'):
            return
        
        uiDict = self.rendererTabUiDict['common']    

        # progressive/bucket
        renderer = self.renderGlobalsNode.renderer.get()
        
        if self.renderGlobalsNode.dumpAndResume.get():
            uiDict['dumpFramebuffer'].setManage(True)
            uiDict['resumeFramebuffer'].setManage(True)
            uiDict['dumpExrFile'].setManage(True)
        else:
            uiDict['dumpFramebuffer'].setManage(False)
            uiDict['resumeFramebuffer'].setManage(False)
            uiDict['dumpExrFile'].setManage(False)
        
        if renderer == 0:
            uiDict['progressiveCL'].setManage(True)
            uiDict['bucketCL'].setManage(False)
            uiDict['VCMCL'].setManage(False)
        if renderer == 1:
            uiDict['progressiveCL'].setManage(False)
            uiDict['bucketCL'].setManage(True)
            uiDict['VCMCL'].setManage(False)
        if renderer == 2:
            uiDict['progressiveCL'].setManage(True)
            uiDict['bucketCL'].setManage(False)
            uiDict['VCMCL'].setManage(True)

        if self.renderGlobalsNode.doMotionBlur.get():
            uiDict['mbSettingsFrame'].setEnable(True)
        else:
            uiDict['mbSettingsFrame'].setEnable(False)
        
        if self.renderGlobalsNode.exposure_type.get() == 0: #simple exposure
            uiDict["simpleExposureLO"].setManage(True)
            uiDict["photographicExposureLO"].setManage(False)
        else:
            uiDict["simpleExposureLO"].setManage(False)
            uiDict["photographicExposureLO"].setManage(True)
                            
        uiDict['colorMapping_simpleExposure'].setEnable(False)
        if self.renderGlobalsNode.colorMapping_useSimpleExposure.get():
            uiDict['colorMapping_simpleExposure'].setEnable(True)
                        
        uiDict['globalMaterialOverride'].setEnable(False)
        uiDict['globalDirectOverride'].setEnable(False)
        uiDict['globalReflectionOverride'].setEnable(False)
        uiDict['globalRefractionOverride'].setEnable(False)
        if self.renderGlobalsNode.useGlobalDirectOverride.get():
            uiDict['globalDirectOverride'].setEnable(True)
        if self.renderGlobalsNode.useGlobalReflectionOverride.get():
            uiDict['globalReflectionOverride'].setEnable(True)
        if self.renderGlobalsNode.useGlobalRefractionOverride.get():
            uiDict['globalRefractionOverride'].setEnable(True)
        if self.renderGlobalsNode.useGlobalMaterialOverride.get():
            uiDict['globalMaterialOverride'].setEnable(True)
            
    def xmlFileBrowse(self, args=None):
        filename = pm.fileDialog2(fileMode=0, caption="Export Corona File Name")
        if len(filename) > 0:
            filename = filename[0]
            if not filename.endswith(".igs"):
                filename += ".igs"
            self.rendererTabUiDict['xml']['xmlFile'].setText(filename)
    
    def dirBrowse(self, args=None):
        dirname = pm.fileDialog2(fileMode=3, caption="Select dir")
        if len(dirname) > 0:
            self.rendererTabUiDict['opti']['optiField'].setText(dirname[0])

    def editSun(self, *args):
        if not self.rendererTabUiDict.has_key('common'):
            return
        
        uiDict = self.rendererTabUiDict['common']    
        
        if self.renderGlobalsNode.useSunLightConnection.get():
            if len(self.renderGlobalsNode.sunLightConnection.inputs()) > 0:
                # maybe check the connection?
                pass
            else:
                suns = pm.ls("CoronaSun")
                sun = None
                if len(suns) > 0:
                    sun = suns[0]
                else:
                    sun = pm.createNode("directionalLight")
                    sun = sun.getParent()
                    sun.rename("CoronaSun")
                sun.message >> self.renderGlobalsNode.sunLightConnection
                       
    def CoronaPassesCreateTab(self):
        log.debug("CoronaPassesCreateTab()")
        self.createGlobalsNode()
        parentForm = pm.setParent(query=True)
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "PassScrollLayout"

        if self.rendererTabUiDict.has_key('passes'):
            self.rendererTabUiDict.pop('passes')        
        uiDict = {}
        self.rendererTabUiDict['passes'] = uiDict
                
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayout", adjustableColumn=True, width=400) as passColumn:
                Corona.passesUI.PassesUI(parent=passColumn)
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])        
        self.CoronaPassesUpdateTab()

        
    def CoronaPassesUpdateTab(self):
        self.createGlobalsNode()

        if not self.rendererTabUiDict.has_key('passes'):
            return
        uiDict = self.rendererTabUiDict['passes']            
    
#     def CoronaTranslatorCreateTab(self):
#         log.debug("CoronaTranslatorCreateTab()")
#         self.createGlobalsNode()
#         parentForm = pm.setParent(query=True)
#         pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
#         scLo = self.rendererName + "TrScrollLayout"
#         
#         with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
#             with pm.columnLayout(self.rendererName + "TrColumnLayout", adjustableColumn=True, width=400):
#                 with pm.frameLayout(label="Translator", collapsable=True, collapse=False):
#                     attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
#                     ui = pm.attrEnumOptionMenuGrp(label="Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei=self.getEnumList(attr)) 
# 
#                 with pm.frameLayout(label="{0} export".format(self.rendererName), collapsable=True, collapse=False):
#                     ui = pm.checkBoxGrp(label="Export {0} Scene file (no rendering):".format(self.rendererName), value1=False)
#                     pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index=2)
#                     xmlDict = {}
#                     self.rendererTabUiDict['xml'] = xmlDict
#                     defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Corona"
#                     if not defaultXMLPath.dirname().exists():
#                         defaultXMLPath.dirname().makedirs()
#                     with pm.rowLayout(nc=3):
#                         xmlDict['xmlFileText'] = pm.text(label="Export to")
#                         xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath)
#                         pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
#                         pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index=2)
#                         
#                 with pm.frameLayout(label="Optimize Textures", collapsable=True, collapse=False):
#                     optiDict = {}
#                     ui = pm.checkBoxGrp(label="Use Optimized Textures:", value1=False)
#                     with pm.rowLayout(nc=3):
#                         self.rendererTabUiDict['opti'] = optiDict
#                         pm.text(label="OptimizedTex Dir:")
#                         optiDict['optiField'] = pm.textField(text=self.renderGlobalsNode.optimizedTexturePath.get())
#                         pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
#                         pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index=2)
#                 
#                 with pm.frameLayout(label="Additional Settings", collapsable=True, collapse=False):
#                     ui = pm.floatFieldGrp(label="Scene scale:", value1=1.0, numberOfFields=1)
#                     pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index=2)
#                          
#         pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
#         pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
# 
#     def CoronaTranslatorUpdateTab(self):
#         log.debug("CoronaTranslatorUpdateTab()")

    def CoronaPerformanceCreateTab(self):
        log.debug("CoronaPerformanceCreateTab()")
        self.createGlobalsNode()
        tabname = "performance"
        parentForm = pm.setParent(query=True)                
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key(tabname):
            self.rendererTabUiDict.pop(tabname)
        uiDict = {}
        self.rendererTabUiDict[tabname] = uiDict

        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayoutGI", adjustableColumn=True, width=400) as uiDict['giCLout']:                
                with pm.frameLayout(label="Global Illumination", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName='gi_primarySolver', uiType='enum', displayName='Primary Solver', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='gi_secondarySolver', uiType='enum', displayName='Secondary Solver', uiDict=uiDict, callback=self.CoronaPerformanceUpdateTab)
                UHDFrame = None
                HDFrame = None
                with pm.frameLayout(label="Secondary GI ", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        with pm.frameLayout(label="UHD Cache Settings ", collapsable=True, collapse=False) as UHDFrame:
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName='uhdPrecision', uiType='float', displayName='Precision', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName='uhdCacheType', uiType='enum', displayName='Mode', uiDict=uiDict)
                        with pm.frameLayout(label="HD Cache Settings ", collapsable=True, collapse=False) as HDFrame:
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_precompMult', uiType = 'float', displayName = 'Precom Density', default='1.0', data='minmax:0.0:99.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_interpolationCount', uiType = 'int', displayName = 'Interpolation Count', default='3', data='1:64', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_ptSamples', uiType = 'int', displayName = 'Record Quality', default='256', data='minmax:1:4096', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_smoothing', uiType = 'float', displayName = 'Smoothing', default='2.0', data='minmax:1.0:10.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_maxRecords', uiType = 'int', displayName = 'Max Records', default='100000', data='minmax:1000:999000', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_glossyThreshold', uiType = 'float', displayName = 'Glossy Threshold', default='0.9', data='minmax:0.0:1.0', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName = 'gi_hdCache_writePasses', uiType = 'int', displayName = 'Write # of Passes', default='0', data='minmax:0:9999999', uiDict=uiDict)
                                with pm.frameLayout(label="Interpolation accuracy ", collapsable=True, collapse=False):
                                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_dirSensitivity', uiType = 'float', displayName = 'Direction', default='2.0', data='minmax:0.001:100.0', uiDict=uiDict)
                                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_posSensitivity', uiType = 'float', displayName = 'Position', default='20.0', data='minmax:0.0:100.0', uiDict=uiDict)
                                        self.addRenderGlobalsUIElement(attName = 'gi_hdCache_normalSensitivity', uiType = 'float', displayName = 'Normal', default='3.0', data='minmax:0.0:10.0', uiDict=uiDict)
                        with pm.frameLayout(label="Load/Save", collapsable=True, collapse=False) as uiDict['loadSaveFrame']:
                            with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                                self.addRenderGlobalsUIElement(attName='gi_hdCache_precalcMode', uiType='enum', displayName='Precalc Mode', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName='gi_saveSecondary', uiType = 'bool', displayName = 'Save Cache After Render', default='false', uiDict=uiDict)
                                self.addRenderGlobalsUIElement(attName='gi_secondaryFile', uiType='string', displayName='HD Cache File', default='"c:/secondarygi.dat"', uiDict=uiDict)
                uiDict['UHDFrame'] = UHDFrame
                uiDict['HDFrame'] = HDFrame
                
            with pm.frameLayout(label="Performance", collapsable=True, collapse=False):
                with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                    self.addRenderGlobalsUIElement(attName='threads', uiType='int', displayName='Threads', default=8, uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='lockSamplingPattern', uiType = 'bool', displayName = 'Lock Sampling Pattern', default='false', uiDict=uiDict)
                    pm.separator()
                    self.addRenderGlobalsUIElement(attName = 'pathtracingSamples', uiType = 'int', displayName = 'GI/AA balance', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName = 'lights_areaSamplesMult', uiType = 'float', displayName = 'Light Samples Multiplier', uiDict=uiDict)                        
                    pm.separator()
                    self.addRenderGlobalsUIElement(attName='maxPtSampleIntensity', uiType='float', displayName='Max Sample Intensity', default='0.0', data='minmax:0.0:99999.0', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='raycaster_maxDepth', uiType='int', displayName='Max Ray Depth', default='25', data='minmax:1:25', uiDict=uiDict)
    
            with pm.frameLayout(label="Displacement", collapsable=True, collapse=False):
                with pm.columnLayout(self.rendererName + 'DisplColumnLayout', adjustableColumn=True, width=400):
                    #self.addRenderGlobalsUIElement(attName='displace_maxSubdiv', uiType='int', displayName='Displace_maxsubdiv', default='100', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='displace_useProjectionSize', uiType='bool', displayName='View Adaptive', default='true', uiDict=uiDict, callback=self.CoronaPerformanceUpdateTab)
                    self.addRenderGlobalsUIElement(attName='displace_maxProjectSize', uiType='float', displayName='Max Edgelen (Pixels)', default='2.0', uiDict=uiDict)
                    self.addRenderGlobalsUIElement(attName='displace_maxWorldSize', uiType='float', displayName='Max Edgelen (Worldspace)', default='1.0', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.updateEnvironment()
        self.CoronaPerformanceUpdateTab()
        
    def CoronaPerformanceUpdateTab(self, dummy=None):
        if dummy:
            print dummy
        log.debug("CoronaPerformanceUpdateTab()")
        self.createGlobalsNode()
        if not self.rendererTabUiDict.has_key('performance'):
            return        
        uiDict = self.rendererTabUiDict['performance']    
        if self.renderGlobalsNode.gi_secondarySolver.get() < 2:
            uiDict['UHDFrame'].setManage(False)
            uiDict['HDFrame'].setManage(False)
            uiDict['loadSaveFrame'].setManage(False)
        if self.renderGlobalsNode.gi_secondarySolver.get() > 1:
            uiDict['loadSaveFrame'].setManage(True)
        if self.renderGlobalsNode.gi_secondarySolver.get() == 2: #hd cache
            uiDict['UHDFrame'].setManage(False)
            uiDict['HDFrame'].setManage(True)
        if self.renderGlobalsNode.gi_secondarySolver.get() == 3: #hd cache
            uiDict['UHDFrame'].setManage(True)
            uiDict['HDFrame'].setManage(False)
            
        uiDict['displace_maxWorldSize'].setManage(False)
        uiDict['displace_maxProjectSize'].setManage(False)
        if self.renderGlobalsNode.displace_useProjectionSize.get():
            uiDict['displace_maxProjectSize'].setManage(True)
        else:
            uiDict['displace_maxWorldSize'].setManage(True)

    def CoronaSystemCreateTab(self):
        log.debug("CoronaSystemCreateTab()")
        self.createGlobalsNode()
        tabname = "system"
        parentForm = pm.setParent(query=True)                
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        scLo = self.rendererName + "ScrollLayout"
        if self.rendererTabUiDict.has_key(tabname):
            self.rendererTabUiDict.pop(tabname)
        uiDict = {}
        self.rendererTabUiDict[tabname] = uiDict

        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0):
            with pm.columnLayout(self.rendererName + "ColumnLayoutGI", adjustableColumn=True, width=400) as uiDict['giCLout']:                
                with pm.frameLayout(label="Framebuffer", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        self.addRenderGlobalsUIElement(attName = 'useCoronaVFB', uiType = 'bool', displayName = 'Use Corona VFB', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName = 'renderstamp_use', uiType = 'bool', displayName = 'Use Renderstamp', default='true', uiDict=uiDict, callback=self.CoronaSystemUpdateTab)
                        self.addRenderGlobalsUIElement(attName = 'renderstamp_inFile', uiType = 'bool', displayName = 'Save File with Stamp', default='false', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName = 'renderStamp', uiType = 'string', displayName = 'Renderstamp', default='"corona renderer alpha | %c | time: %t | passes: %p | primitives: %o | rays/s: %r"', uiDict=uiDict)
                        pm.separator()
                        self.addRenderGlobalsUIElement(attName='filtertype', uiType='enum', displayName='Imagefilter', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='imagefilter_width', uiType='float', displayName='Filter Width', default='1.5', data='minmax:1.0:64', uiDict=uiDict)
                        self.addRenderGlobalsUIElement(attName='imagefilter_blurring', uiType='float', displayName='Blur', default='0.5', data='minmax:0.0:1.0', uiDict=uiDict)

                with pm.frameLayout(label="Translator", collapsable=True, collapse=False):
                    with pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn=True, width=400):
                        attr = pm.Attribute(self.renderGlobalsNodeName + ".translatorVerbosity")
                        ui = pm.attrEnumOptionMenuGrp(label="Translator Verbosity", at=self.renderGlobalsNodeName + ".translatorVerbosity", ei=self.getEnumList(attr)) 

                        ui = pm.checkBoxGrp(label="Export {0} Scene file (no rendering):".format(self.rendererName), value1=False)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".exportSceneFile", index=2)
                        xmlDict = {}
                        self.rendererTabUiDict['xml'] = xmlDict
                        defaultXMLPath = pm.workspace.path + "/" + pm.sceneName().basename().split(".")[0] + ".Corona"
                        if not defaultXMLPath.dirname().exists():
                            defaultXMLPath.dirname().makedirs()
                        with pm.rowLayout(nc=3):
                            xmlDict['xmlFileText'] = pm.text(label="Export to")
                            xmlDict['xmlFile'] = pm.textField(text=defaultXMLPath)
                            pm.symbolButton(image="navButtonBrowse.png", c=self.xmlFileBrowse)
                            pm.connectControl(xmlDict['xmlFile'], self.renderGlobalsNodeName + ".exportSceneFileName", index=2)
                        
#                         ui = pm.checkBoxGrp(label="Use Optimized Textures:", value1=False)
#                         with pm.rowLayout(nc=3):
#                             self.rendererTabUiDict['opti'] = optiDict
#                             pm.text(label="OptimizedTex Dir:")
#                             optiDict['optiField'] = pm.textField(text=self.renderGlobalsNode.optimizedTexturePath.get())
#                             pm.symbolButton(image="navButtonBrowse.png", c=self.dirBrowse)
#                             pm.connectControl(optiDict['optiField'], self.renderGlobalsNodeName + ".optimizedTexturePath", index=2)
                
                        ui = pm.floatFieldGrp(label="Scene scale:", value1=1.0, numberOfFields=1)
                        pm.connectControl(ui, self.renderGlobalsNodeName + ".sceneScale", index=2)
                        
        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[ (scLo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ])
        self.CoronaSystemUpdateTab()
                        

    def CoronaSystemUpdateTab(self):
        log.debug("CoronaSystemUpdateTab()")
        
        log.debug("CoronaPerformanceUpdateTab()")
        self.createGlobalsNode()
        if not self.rendererTabUiDict.has_key('system'):
            return        
        uiDict = self.rendererTabUiDict['system']    

        uiDict['renderstamp_inFile'].setEnable(False)
        if self.renderGlobalsNode.renderstamp_use.get():
            uiDict['renderstamp_inFile'].setEnable(True)
        

    def createImageFormats(self):
        if self.renderGlobalsNode:
            iList = self.renderGlobalsNode.imageFormat.getEnums()
            self.imageFormats = []
            self.imageFormats.extend(iList)

    # overwrite automatic tab creation because we want a non default naming convention
    def addTabs(self):
        self.useRendererTabPrefix = False
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("CommonGlobals"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Scene"))
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Performance"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("System"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Passes"))    


    def registerNodeExtensions(self):
        """Register Corona specific node extensions. e.g. camera type, diaphram_blades and others
        """
        pm.addExtension(nodeType="camera", longName="mtco_iso", attributeType="float", defaultValue=100.0)
        pm.addExtension(nodeType="camera", longName="mtco_shutterSpeed", attributeType="float", defaultValue=125.0, minValue=0.001, softMaxValue=2048.0)
        pm.addExtension(nodeType="camera", longName="mtco_overrideRenderSettings", attributeType="bool", defaultValue=False)
        
        pm.addExtension(nodeType="camera", longName="mtco_useBokeh", attributeType="bool", defaultValue=False)
        pm.addExtension(nodeType="camera", longName="mtco_circularBlades", attributeType="bool", defaultValue=True)
        pm.addExtension(nodeType="camera", longName="mtco_blades", attributeType="long", defaultValue=6)
        pm.addExtension(nodeType="camera", longName="mtco_bladeRotation", attributeType="float", defaultValue=0.0)
        pm.addExtension( nodeType="camera", longName="mtco_bokehBitmap", usedAsColor=True, attributeType='float3')
        pm.addExtension( nodeType='camera', longName='mtco_bokehBitmapR', attributeType='float', parent='mtco_bokehBitmap', defaultValue = 0.0 )
        pm.addExtension( nodeType='camera', longName='mtco_bokehBitmapG', attributeType='float', parent='mtco_bokehBitmap', defaultValue = 0.0 )
        pm.addExtension( nodeType='camera', longName='mtco_bokehBitmapB', attributeType='float', parent='mtco_bokehBitmap', defaultValue = 0.0 )  
        pm.addExtension( nodeType='camera', longName='mtco_cameraType', attributeType='enum', enumName="Perspective:Cylindrical:Spherical", defaultValue = 0.0)
        
        # exponent for sun light
        pm.addExtension(nodeType="directionalLight", longName="mtco_sun_multiplier", attributeType="float", defaultValue=1.0)
        pm.addExtension(nodeType="directionalLight", longName="mtco_useAsSun", attributeType="bool", defaultValue=False)
        pm.addExtension( nodeType='areaLight', longName='mtco_envPortal', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType='areaLight', longName='mtco_areaVisible', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='areaLight', longName='mtco_castShadows', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType='areaLight', longName='mtco_doubleSided', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType='areaLight', longName='mtco_visibleInReflection', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='areaLight', longName='mtco_visibleInRefraction', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='areaLight', longName='mtco_noOSL', attributeType='bool', defaultValue=False) 

        # displacement shader        
        pm.addExtension(nodeType="displacementShader", longName="mtco_displacementMin", attributeType="float", defaultValue=0.0)
        pm.addExtension(nodeType="displacementShader", longName="mtco_displacementMax", attributeType="float", defaultValue=0.01)
        pm.addExtension(nodeType="displacementShader", longName="mtco_displacementAdaptive", attributeType='bool', defaultValue=False) 
        
        #bump map because the maya implementation is too strange
        pm.addExtension( nodeType='bump2d', longName='normalMap', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='bump2d', longName='normalMapR', attributeType='float', parent='normalMap' )
        pm.addExtension( nodeType='bump2d', longName='normalMapG', attributeType='float', parent='normalMap' )
        pm.addExtension( nodeType='bump2d', longName='normalMapB', attributeType='float', parent='normalMap' )        
        
        # gamma
        pm.addExtension( nodeType='file', longName='fromSRGB', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='file', longName='toSRGB', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType='file', longName='textureFilter', attributeType='enum', enumName="Bilinear:Bicubic:Nearest", defaultValue = 0.0)
        pm.addExtension( nodeType='file', longName='textureBlur', attributeType='float', defaultValue=0.0)
        pm.addExtension( nodeType='file', longName='textureSblur', attributeType='float', defaultValue=0.0)
        pm.addExtension( nodeType='file', longName='textureTblur', attributeType='float', defaultValue=0.0)
        pm.addExtension( nodeType='file', longName='textureFilterWidth', attributeType='float', defaultValue=1.0)
        pm.addExtension( nodeType='file', longName='environmentMappingType', attributeType='enum', enumName="Spherical:Screen", defaultValue = 0.0)
        pm.addExtension( nodeType='file', longName='mtco_noOSL', attributeType='bool', defaultValue=False) 

        #mesh
        pm.addExtension( nodeType='mesh', longName='mtco_visibleInGI', attributeType='bool', defaultValue=True) 
        pm.addExtension( nodeType='mesh', longName='mtco_envPortal', attributeType='bool', defaultValue=False) 
        pm.addExtension( nodeType="mesh", longName="mtco_shadowCatcherMode", attributeType="enum", enumName="Off::Final:Composite", defaultValue = 0.0)

        #test for roundCorners        
        pm.addExtension( nodeType='CoronaSurface', longName='roundCorners', attributeType='message', multi=False)
        
        pm.addExtension( nodeType='samplerInfo', longName='outDPdu', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduR', attributeType='float', parent='outDPdu' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduG', attributeType='float', parent='outDPdu' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPduB', attributeType='float', parent='outDPdu' )        
        
        pm.addExtension( nodeType='samplerInfo', longName='outDPdv', usedAsColor=True, attributeType='float3' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvR', attributeType='float', parent='outDPdv' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvG', attributeType='float', parent='outDPdv' )
        pm.addExtension( nodeType='samplerInfo', longName='outDPdvB', attributeType='float', parent='outDPdv' )        

        pm.addExtension( nodeType='coronaGlobals', longName='AOVs', attributeType='message', multi=True)
        
        pm.addExtension( nodeType='objectSet', longName='mtco_mtlOverride', attributeType='message')

            
    def removeLogFile(self):
        pass

    def showLogFile(self):
        pass
    
    def connectNodeToNodeOverrideCallback(self, srcNode, destNode):
        log.debug("connectNodeToNodeOverrideCallback {0} {1}".format(srcNode, destNode))
        dn = pm.PyNode(destNode)
        sn = pm.PyNode(srcNode)
        if dn.type() in ["CoronaSurface"]:
            if sn.hasAttr("outColor"):
                sn.outColor >> dn.diffuse
                return 0
            if sn.type() == "CoronaRoundCorners":
                sn.message >> dn.roundCorners
                return 0
        return 1
    
    def createRenderNode(self, nodeType=None, postCommand=None):
        log.debug("createRenderNode callback for renderer {0} with node: {1}".format(self.rendererName.lower(), nodeType))
        nodeClass = None
        for cl in pm.getClassification(nodeType):
            if "corona/material" in cl:
                nodeClass = "shader"
            if "corona/texture" in cl:
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
    
    def renderProcedure(self, width, height, doShadows, doGlow, camera, options):
        log.debug("renderProcedure")
        
        renderState = pm.mayatoCorona(s=True)
        if  renderState in ["rstaterendering", "rstatetranslating"]:
            log.error("Rendering is in progress, cannot proceed. Please stop rendering first.")
            return

        self.removeLogFile()
        log.debug("renderProcedure {0} {1} {2} {3} {4} {5}".format(width, height, doShadows, doGlow, camera, options))
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        
        if pm.about(batch=True):
            pm.mayatoCorona()
        else:
            pm.mayatoCorona(width=width, height=height, camera=camera)
            
        if not self.ipr_isrunning:
            self.showLogFile()
        self.postRenderProcedure()
        
    def isRunningIprProcedure(self):        
        log.debug("isRunningIprProcedure")
        return self.ipr_isrunning

        renderState = pm.mayatoCorona(s=True)
        if  renderState not in ["rstatenone"]:
            log.debug("rstate is NOT none - ipr is running.")
            return True
        log.debug("rstate IS  none - ipr is NOT running.")
        return False
    
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        canDoIPR = pm.mayatoCorona(canDoIPR=True)
        if canDoIPR == "no":
            log.warning("IPR is not yet supported, sorry.")
            return
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        log.debug("startIprRenderProcedure {0} {1} {2} {3}".format( editor, resolutionX, resolutionY, camera))
        self.createGlobalsNode()    
        self.preRenderProcedure()
        self.setImageName()
        pm.mayatoCorona(width=resolutionX, height=resolutionY, camera=camera, startIpr=True)
        self.postRenderProcedure()
        
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pm.mayatoCorona(stopIpr=True)
        self.postRenderProcedure()
            
    def preRenderProcedure(self):
        self.createGlobalsNode()
        if self.renderGlobalsNode.threads.get() == 0:
            # TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        
        if self.renderGlobalsNode.useOptimizedTextures.get():
            if not self.renderGlobalsNode.optimizedTexturePath.get() or len(self.renderGlobalsNode.optimizedTexturePath.get()) == 0:
                optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
                if not os.path.exists(optimizedPath):
                    optimizedPath.makedirs()
                self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
    
            # craete optimized exr textures
            optimizeTextures.preRenderOptimizeTextures(optimizedFilePath=self.renderGlobalsNode.optimizedTexturePath.get())

    def postRenderProcedure(self):
        optimizeTextures.postRenderOptimizeTextures()
        
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        aet.AECoronaNodeTemplate(nodeName)
        
    def afterGlobalsNodeReplacement(self):
        log.debug("afterGlobalsNodeReplacement")        
        self.rendererTabUiDict = {}        

"""
This procedure loads all AETemplates that are loaceted in the AETemplates module. 
Normally if you load pymel, it automatically loads the templates but only the ones it finds in the
very first AETemplates directory. If you have several OpenMaya renderers loaded or if you have your own
AETemplates directory, the automatic loading will not work. So I replace it with this procedure.
"""

def loadAETemplates():    
    rendererName = "Corona"
    aeDir = path.path(__file__).dirname() + "/" + rendererName + "/AETemplate/"
    log.debug("searching aeTemplate dir {0} for templates.".format(aeDir))
    log.debug("found {0} templates.".format(len(aeDir.listdir("*.py"))))
    for d in aeDir.listdir("*.py"):
        if d.endswith("Template.py") and d.basename().startswith("AE"):
            templateName = d.basename().replace(".py", "")
            pythonCommand = "import {1}.AETemplate.{0}".format(templateName, rendererName)
            melCommand = 'python("{0}");'.format(pythonCommand)
            log.debug("trying to load aeTemplate {0} with command {1}.".format(templateName, melCommand))
            pm.mel.eval(melCommand)

def theRenderer():
    return CoronaRenderer.theRenderer()
        
def initRenderer():
    try:
        log.debug("Init renderer Corona")
        theRenderer().registerRenderer()
        loadAETemplates()
        theRenderer().createRendererMenu()
    except:
        traceback.print_exc(file=sys.__stderr__)
        log.error("Init renderer Corona FAILED")
        
def unregister():
    theRenderer().unRegisterRenderer()
    log.debug("Unregister done")

