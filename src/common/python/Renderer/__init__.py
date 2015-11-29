import sys
import os
import logging
import path
import pymel.core as pm
import uiUtils
import commonGlobals
import commonGlobalsMel

log = logging.getLogger("renderLogger")

class MayaToRenderer(object):
    
    def __init__(self, rendererName, moduleName):
        self.rendererName = rendererName
        self.moduleName = moduleName
        self.pluginName = "mayato" + self.rendererName.lower()
        self.renderGlobalsNodeName = self.rendererName.lower() + "Globals"
        self.renderGlobalsNode = None
        # self.baseRenderMelCommand = "import {0} as rcall; reload(rcall); rcall.theRenderer().".format(self.moduleName)
        self.baseRenderMelCommand = "import {0} as rcall; rcall.theRenderer().".format(self.moduleName)
        self.imageFormats = []
        self.ipr_isrunning = False
        self.imageFormatCtrl = None
        self.openMayaCommonGlobals = None
        self.useRendererTabPrefix = True
        
    def addRenderGlobalsUIElement(self, attName=None, uiType=None, displayName=None, default=None, data=None, uiDict=None, callback=None, anno=None):
        uiUtils.addRenderGlobalsUIElement(self.renderGlobalsNodeName, attName, uiType, displayName, default, data, uiDict, callback, anno)
    # the render callback is called with arguments like this
    # renderCmd 640 480 1 1 perspShape " -layer defaultRenderLayer"
    # if the renderCmd is replaced with a python call it may not get all informations so we 
    # can think about an alternative call like this
    # renderMelCmd pythonCmd 640 480 1 1 perspShape " -layer defaultRenderLayer"
    # this can result in a pythonCmd(640, 480, 1, 1, perspShape, " -layer defaultRenderLayer") call
    def renderCallback(self, procedureName):
        cmd = "python(\"{0}{1}()\");\n".format(self.baseRenderMelCommand, procedureName)
        return cmd

    def makeMelPythonCmdStringFromPythonCmd(self, pythonCmdObj, argSet):
        pCmd = "\"{0}{1}(".format(self.baseRenderMelCommand, pythonCmdObj.__name__)
        argList = []
        for dtype, value in argSet:
            if dtype == 'string':
                argList.append(value + "=" + r"""'"+$%s+"'""" % value)           
            else:
                argList.append(value + "=\" + $" + value + " + \"")           
        pCmd += ",".join(argList)
        pCmd += ")\""            
        return pCmd
    
    def makeMelProcFromPythonCmd(self, pythonCmdObj, argSet):    
        cmd = pythonCmdObj.__name__
        melProcName = "RenderProc_" + cmd
        melCmd = "global proc %s (" % melProcName
        argList = []
        for arg in argSet:
            argList.append(" $".join(arg))
        melCmd += ",".join(argList) + ")\n{\n"
        melCmd += "    string $cmdString = " + self.makeMelPythonCmdStringFromPythonCmd(pythonCmdObj, argSet) + ";\n"
        melCmd += "    python($cmdString);\n"
        melCmd += "};\n";
        # print "MelCmd:", melCmd
        pm.mel.eval(melCmd)
        return melProcName
    
    def batchRenderProcedure(self, options):
        self.preRenderProcedure()
        log.debug("batchRenderProcedure: options " + str(options))
        self.renderProcedure(-1, -1, True, True, None, options)    
    
    def renderOptionsProcedure(self):
        self.preRenderProcedure()
        log.debug("RenderOptionsProcedure")
    
    def commandRenderProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("commandRenderProcedure")
        self.renderProcedure()
        
    def renderProcedure(self, width, height, doShadows, doGlowPass, camera, options):
        pass
    
    def batchRenderOptionsProcedure(self):
        self.preRenderProcedure()
        log.debug("batchRenderOptionsProcedure")
    
    def batchRenderOptionsStringProcedure(self):
        self.preRenderProcedure()
        log.debug("batchRenderOptionsStringProcedure")
    
    def cancelBatchRenderProcedure(self):
        self.preRenderProcedure()
        log.debug("cancelBatchRenderProcedure")
    
    def showBatchRenderProcedure(self):
        self.preRenderProcedure()
        log.debug("showBatchRenderProcedure")
    
    def showRenderLogProcedure(self):
        self.preRenderProcedure()
        log.debug("showRenderLogProcedure")
    
    def showBatchRenderLogProcedure(self):
        self.preRenderProcedure()
        log.debug("showBatchRenderLogProcedure")
    
    def renderRegionProcedure(self):
        self.preRenderProcedure()
        log.debug("renderRegionProcedure")
    
    def textureBakingProcedure(self):
        self.preRenderProcedure()
        log.debug("textureBakingProcedure")
    
    def renderingEditorsSubMenuProcedure(self, *args):
        #pm.popupMenu(parentMenu, edit=True, deleteAllItems=True)
        #pm.menuItem(label="Dummy Menu Item", parent=parentMenu)
        #self.preRenderProcedure()
        log.debug("renderingEditorsSubMenuProcedure")
                  
    def OpenMayaCommonGlobalsCreateTab(self):
        self.createGlobalsNode()
        if self.openMayaCommonGlobals is None:
            self.openMayaCommonGlobals = commonGlobals.OpenMayaCommonGlobals(self.renderGlobalsNode)
        self.openMayaCommonGlobals.renderNode = self.renderGlobalsNode
        self.openMayaCommonGlobals.OpenMayaCommonGlobalsCreateTab()
                    
    def OpenMayaCommonGlobalsUpdateTab(self):
        self.createGlobalsNode()
        if self.openMayaCommonGlobals is None:
            self.openMayaCommonGlobals = commonGlobals.OpenMayaCommonGlobals(self.renderGlobalsNode)
        self.openMayaCommonGlobals.renderNode = self.renderGlobalsNode
        self.openMayaCommonGlobals.OpenMayaCommonGlobalsUpdateTab()
        
    # render tab creation. Renderer tabs need a mel globals procedure.
    # a mel procedure is called for create and update tab
    # here I create a fake mel procedure to call my own python command
    
    # per default, the inheriting class will implement the procedures:
    # {Name}RendererCreateTab() and the {Name}RendererUpdateTab()
    # {Name}TranslatorCreateTab() and the {Name}TranslatorUpdateTab()
    def renderTabMelProcedure(self, tabName):
        
        createTabCmd = "{0}{1}CreateTab".format(self.rendererName, tabName) 
        updateTabCmd = "{0}{1}UpdateTab".format(self.rendererName, tabName) 
        if self.useRendererTabPrefix:
            tabLabel = "{0}{1}".format(self.rendererName, tabName)
        else:
            tabLabel = "{0}".format(tabName)
        createPyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, createTabCmd)
        updatePyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, updateTabCmd)
        melCreateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(createTabCmd, createPyCmd)
        melUpdateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(updateTabCmd, updatePyCmd)
        
        #if tabName == "CommonGlobals":
        #   createTabCmd = "{0}{1}CreateTab".format(self.rendererName, tabName) 
        #log.debug("mel create tab: \n" + melCreateCmd)
        #log.debug("mel update tab: \n" + melUpdateCmd)
        #log.debug("python create tab: \n" + createPyCmd)
        #log.debug("python update tab: \n" + updatePyCmd)
        #log.debug("tab proc return {0} {1} {2}".format(tabLabel, createTabCmd, updateTabCmd))
        pm.mel.eval(melCreateCmd)
        pm.mel.eval(melUpdateCmd)
        return (tabLabel, createTabCmd, updateTabCmd)
    
    def createImageFormats(self):
        pass
    
    def checkNamingConvention(self):
        #check file naming convention. We use only name.ext and name.nr.ext
        defaultGlobals = pm.SCENE.defaultRenderGlobals
        defaultGlobals.periodInExt.set(1)
        defaultGlobals.putFrameBeforeExt.set(1)
#         if self.defaultGlobals.animation.get():
#         else:
#             self.defaultGlobals.putFrameBeforeExt.set(0)
            
        
        
    def preRenderProcedure(self):
        
        self.checkNamingConvention()
                
        self.createGlobalsNode()
        drg = pm.PyNode("defaultRenderGlobals")
        if self.renderGlobalsNode.threads.get() == 0:
            # TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        # craete optimized exr textures
        
        
    def postRenderProcedure(self):
        drg = pm.PyNode("defaultRenderGlobals")

    def preFrameProcedure(self):
        pass
    
    def postFrameProcedure(self):
        pass

    def overwriteUpdateMayaImageFormatControl(self):
        cmd = """
global proc string createMayaImageFormatControl()
{
    return "imageMenuMayaSW";
}
"""
        pm.mel.eval(cmd)        
        cmd = """
global proc updateMayaImageFormatControl()
{}
"""
        pm.mel.eval(cmd)        
        return
    
    def unDoOverwriteUpdateMayaImageFormatControl(self):
        log.debug("UndoOverwrite")
        pm.mel.eval("source createMayaSoftwareCommonGlobalsTab")
    
    def createGlobalsNode(self):      
        selection = pm.ls(sl=True)
        if not self.renderGlobalsNode: 
            # maybe the node has been replaced by replaced by a new loaded node, check this
            if len(pm.ls(self.renderGlobalsNodeName)) > 0:
                self.renderGlobalsNode = pm.ls(self.renderGlobalsNodeName)[0]
                log.debug("Globals node replaced.")
                self.afterGlobalsNodeReplacement()
                self.renderGlobalsNode.setLocked(True)
            else:
                self.renderGlobalsNode = pm.createNode(self.renderGlobalsNodeName)
                self.renderGlobalsNode.rename(self.renderGlobalsNodeName)
                log.debug("Created node " + str(self.renderGlobalsNode))
                optimizedPath = pm.workspace.path+"/optimizedTextures"
                if pm.workspace.fileRules.has_key('renderData'):
                    optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
                if not os.path.exists(optimizedPath):
                    optimizedPath.makedirs()
                self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
                self.afterGlobalsNodeReplacement()
                self.renderGlobalsNode.setLocked(True)
        #else:
        #    log.debug("renderlgobalsnode already defined: " + self.renderGlobalsNode)
        pm.select(selection)
        
    def registerNodeExtensions(self):
        pass

    def hyperShadePanelBuildCreateMenuCallback(self):
        # log.debug("hyperShadePanelBuildCreateMenuCallback")
        pm.menuItem(label=self.rendererName)
        pm.menuItem(divider=True)
    
    def hyperShadePanelBuildCreateSubMenuCallback(self):
        # log.debug("hyperShadePanelBuildCreateSubMenuCallback")
        return "shader/surface"
    
    def buildRenderNodeTreeListerContentCallback(self, tl, postCommand, filterString):
        # log.debug("buildRenderNodeTreeListerContentCallback")        

#        int $numCategories = mrNumNodeCategories();
#        int $i;
#        for( $i = 0; $i < $numCategories; $i++ )
#        {
#            string $categoryInfo[] = mrGetNodeCategory( $i );
#            string $title = $categoryInfo[0];
#            string $uiBaseName = $categoryInfo[1];
#            string $staticClassification = $categoryInfo[2];
#            string $runtimeClassification = $categoryInfo[3];
#
#            // add to the treeLister
#            //
#            addToRenderNodeTreeLister($renderNodeTreeLister, $postCommand, "mental ray/" + $title, $staticClassification, $runtimeClassification, "" );
#
#        }

#        $mrNodeTypeInfo[$index++] = getPluginResource("Mayatomr", "kmrCustomNodeUIMaterials");
#        $mrNodeTypeInfo[$index++] = "mrMaterials";
#        $mrNodeTypeInfo[$index++] = "rendernode/mentalray/material";
#        $mrNodeTypeInfo[$index++] = "-asShader";
        print "filterString", filterString, "postCommand", postCommand
        
        melCmd = 'addToRenderNodeTreeLister( "{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(tl, postCommand, self.rendererName + "/Materials", self.rendererName.lower() + "/material", "-asShader", "")
        # melCmd = 'addToRenderNodeTreeLister( "{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(tl, postCommand, self.rendererName + "/Textures", "lux/shader/texture", "-asUtility", "")
        log.debug("Treelister cmd " + melCmd)
        pm.mel.eval(melCmd)
        melCmd = 'addToRenderNodeTreeLister( "{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(tl, postCommand, self.rendererName + "/Textures", self.rendererName.lower() + "/texture", "-asUtility", "")
        pm.mel.eval(melCmd)
#        global proc addToRenderNodeTreeLister(
#            string $renderNodeTreeLister,
#            string $postCommand,
#            string $frame,
#            string $classification,
#            string $as,
#            string $flag)
    
    def aeTemplateCallback(self, nodeName):
        log.debug("aeTemplateCallback: " + nodeName)
        
        
    def registerAETemplateCallbacks(self):
        log.debug("registerAETemplateCallbacks")
        # callback is defined as mel script, didn't work as pymel command.. 
        # aeCallbackName = "AE{0}NodeCallback".format(self.rendererName)
        # aeCallbackString = "callbacks -addCallback {0} -hook AETemplateCustomContent -owner {1};".format(aeCallbackName, self.pluginName)
        # pm.mel.eval(aeCallbackString)

        pm.callbacks(addCallback=self.aeTemplateCallback, hook='AETemplateCustomContent', owner=self.pluginName)
        

#        aeCallbackName = "{0}hyperShadePanelBuildCreateMenuCallback"
#        aeCallbackString = "callbacks -addCallback {0} -hook hyperShadePanelBuildCreateMenu -owner {1};".format(aeCallbackName, self.pluginName)
#
#        aeCallbackName = "{0}hyperShadePanelBuildCreateSubMenuCallback".format(self.rendererName)
#        aeCallbackString = "callbacks -addCallback {0} -hook hyperShadePanelBuildCreateSubMenu -owner {1};".format(aeCallbackName, self.pluginName)

        pm.callbacks(addCallback=self.hyperShadePanelBuildCreateMenuCallback, hook='hyperShadePanelBuildCreateMenu', owner=self.pluginName)
        pm.callbacks(addCallback=self.hyperShadePanelBuildCreateSubMenuCallback, hook='hyperShadePanelBuildCreateSubMenu', owner=self.pluginName)
        pm.callbacks(addCallback=self.buildRenderNodeTreeListerContentCallback, hook='buildRenderNodeTreeListerContent', owner=self.pluginName)
        pm.callbacks(addCallback=self.createRenderNodeCallback, hook='createRenderNodeCommand', owner=self.pluginName)
        pm.callbacks(addCallback=self.connectNodeToNodeOverrideCallback, hook='connectNodeToNodeOverrideCallback', owner=self.pluginName)
        
        
        
        aeTemplateName = "AE{0}NodeTemplate".format(self.rendererName.lower())
        aeTemplateImportName = aeTemplateName

        
#        aeCallbackProc = "global proc " + aeCallbackName + "(string $nodeName)\n"
#        aeCallbackProc += "{\n"
#        aeCallbackProc += "\tprint(\"executing aenodecallbacktemplate.\");\n"
#        #aeCallbackProc += "string $cmd = \"import AETemplates." + aeTemplateImportName + " as aet; aet." + aeTemplateName + "(\\\"\" + $nodeName + \"\\\")\";\n"
#        aeCallbackProc += "string $cmd = \"import AETemplates as aet; aet." + aeTemplateImportName + "(\\\"\" + $nodeName + \"\\\")\";\n"
#        aeCallbackProc += "python($cmd);\n"
#        aeCallbackProc += "}\n"
#        log.debug("aeCallback: " + aeCallbackProc)
#        pm.mel.eval(aeCallbackProc)
    
    def createRenderNodeCallback(self, postCommand, nodeType):
        #log.debug("createRenderNodeCallback postCmd {0} nodeType {1}".format(postCommand, nodeType))
        for c in pm.getClassification(nodeType):
            if self.rendererName.lower() in c.lower():
                buildNodeCmd = "import {0} as rcall; rcall.theRenderer().createRenderNode(nodeType=\\\"{1}\\\", postCommand='{2}')".format(self.moduleName, nodeType, postCommand)
                buildNodeCmd = "string $cmd = \"{0}\"; python($cmd);".format(buildNodeCmd)
                #log.debug("buildNodeCmd {0}".format(buildNodeCmd))
                return buildNodeCmd

    def connectNodeToNodeOverrideCallback(self, srcNode, destNode):
        return 1    
    
    def registerRenderer(self):
        log.debug("registerRenderer")
        self.unRegisterRenderer()
        self.registerNodeExtensions()
        self.registerAETemplateCallbacks()
        pm.renderer(self.rendererName, rendererUIName=self.rendererName)
        pm.renderer(self.rendererName, edit=True, renderProcedure=self.makeMelProcFromPythonCmd(self.renderProcedure, [('int', 'width'),
                                                                                                                       ('int', 'height'),
                                                                                                                       ('int', 'doShadows'),
                                                                                                                       ('int', 'doGlow'),
                                                                                                                       ('string', 'camera'),
                                                                                                                       ('string', 'options')]))
        pm.renderer(self.rendererName, edit=True, batchRenderProcedure=self.makeMelProcFromPythonCmd(self.batchRenderProcedure, [('string', 'options')]))
        pm.renderer(self.rendererName, edit=True, commandRenderProcedure=self.makeMelProcFromPythonCmd(self.batchRenderProcedure, [('string', 'options')]))
        # pm.renderer(self.rendererName, edit=True, batchRenderProcedure=self.renderCallback("batchRenderProcedure"))
        # pm.renderer(self.rendererName, edit=True, commandRenderProcedure=self.renderCallback("commandRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, batchRenderOptionsProcedure=self.renderCallback("batchRenderOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, batchRenderOptionsStringProcedure=self.renderCallback("batchRenderOptionsStringProcedure"))
        pm.renderer(self.rendererName, edit=True, addGlobalsNode="defaultRenderGlobals")
        pm.renderer(self.rendererName, edit=True, addGlobalsNode="defaultResolution")
        pm.renderer(self.rendererName, edit=True, addGlobalsNode=self.renderGlobalsNodeName)
        
        pm.renderer(self.rendererName, edit=True, startIprRenderProcedure=self.makeMelProcFromPythonCmd(self.startIprRenderProcedure, [('string', 'editor'),
                                                                                                                                       ('int', 'resolutionX'),
                                                                                                                                       ('int', 'resolutionY'),
                                                                                                                                       ('string', 'camera')]))
        pm.renderer(self.rendererName, edit=True, stopIprRenderProcedure=self.makeMelProcFromPythonCmd(self.stopIprRenderProcedure, []))
        pm.renderer(self.rendererName, edit=True, pauseIprRenderProcedure=self.makeMelProcFromPythonCmd(self.pauseIprRenderProcedure, [('string', 'editor'), ('int', 'pause')]))
        
        pm.renderer(self.rendererName, edit=True, changeIprRegionProcedure=self.renderCallback("changeIprRegionProcedure"))
        pm.renderer(self.rendererName, edit=True, iprOptionsProcedure=self.renderCallback("iprOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, iprOptionsMenuLabel=self.renderCallback("iprOptionsMenuLabel"))
        pm.renderer(self.rendererName, edit=True, iprRenderProcedure=self.renderCallback("iprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, iprRenderSubMenuProcedure=self.renderCallback("iprRenderSubMenuProcedure"))
        pm.renderer(self.rendererName, edit=True, isRunningIprProcedure=self.renderCallback("isRunningIprProcedure"))
        pm.renderer(self.rendererName, edit=True, refreshIprRenderProcedure=self.renderCallback("refreshIprRenderProcedure"))
        #pm.renderer(self.rendererName, edit=True, logoCallbackProcedure=self.renderCallback("logoCallbackProcedure"))
        pm.renderer(self.rendererName, edit=True, logoImageName=self.rendererName + ".png")
        pm.renderer(self.rendererName, edit=True, renderDiagnosticsProcedure=self.renderCallback("renderDiagnosticsProcedure"))
        
        pm.renderer(self.rendererName, edit=True, renderOptionsProcedure=self.renderCallback("renderOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, cancelBatchRenderProcedure=self.renderCallback("cancelBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderProcedure=self.renderCallback("showBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showRenderLogProcedure=self.renderCallback("showRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderLogProcedure=self.renderCallback("showBatchRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, textureBakingProcedure=self.renderCallback("textureBakingProcedure"))        
        #pm.renderer(self.rendererName, edit=True, renderingEditorsSubMenuProcedure=self.renderCallback("renderingEditorsSubMenuProcedure"))
            
        pm.renderer(self.rendererName, edit=True, renderRegionProcedure="mayaRenderRegion")

        scriptDir = path.path(__file__).dirname().parent
        #os.environ['MAYA_SCRIPT_PATH'] = "{0};{1}".format(scriptDir, os.environ['MAYA_SCRIPT_PATH'])
        pm.mel.source('createMayaSoftwareCommonGlobalsTab')
        pm.mel.source("unifiedRenderGlobalsWindow")
        
        self.defineCommonMelProcedures()
        
        pm.evalDeferred(self.addTabs)                
        log.debug("RegisterRenderer done")
        
    def defineCommonMelProcedures(self):        
        melCmd = """global proc updateMayaSoftwareImageFormatControl(){
            python("{0}");
        }""".replace("{0}", self.baseRenderMelCommand + "updateMayaSoftwareImageFormatControl()")
        #print "commonMelCmd: ", melCmd
        pm.mel.eval(melCmd)

        melCmd = """global proc updateMultiCameraBufferNamingMenu(){
            python("{0}");
        }""".replace("{0}", self.baseRenderMelCommand + "updateMultiCameraBufferNamingMenu()")
        #print "commonMelCmd: ", melCmd
        pm.mel.eval(melCmd)
        
        melCmd = """global proc addOneTabToGlobalsWindow(string $renderer, string $tabLabel, string $createProc){
            python("{0}");
        }""".replace("{0}", self.baseRenderMelCommand + "addOneTabToGlobalsWindow(\\\"\" + $renderer + \"\\\", \\\"\" + $tabLabel + \"\\\", \\\"\" + $createProc + \"\\\")")
        #print "commonMelCmd: ", melCmd
        pm.mel.eval(melCmd)

    def addOneTabToGlobalsWindow(self, renderer, tabLabel, createProc):
        #print "python cmd: addOneTabToGlobalsWindow", renderer, tabLabel, createProc
        # no windows no need to call
        if not pm.window('unifiedRenderGlobalsWindow', exists=True):
            return
        
        displayAllTabs = pm.mel.isDisplayingAllRendererTabs()
        #print "displayAllTabs", displayAllTabs
        
        if not displayAllTabs and pm.SCENE.defaultRenderGlobals.currentRenderer.get() != renderer:
            #print "displayAllTabs false currentRenderer ('{0}') is not ('{0}')".format(pm.SCENE.defaultRenderGlobals.currentRenderer.get(),renderer)
            return
                
        # Set the correct tabLayout parent.
        if displayAllTabs:
            tabLayoutName = pm.mel.getRendererTabLayout(pm.melGlobals['gMasterLayerRendererName'])
        else:
            tabLayoutName = pm.mel.getRendererTabLayout(renderer)
        
        # Hide the tabForm while updating.
        tabFormManagedStatus = pm.formLayout('tabForm', q=True, manage=True)
        pm.formLayout('tabForm', edit=True, manage=False)
        pm.setParent('tabForm')
        
        # Set the correct tabLayout parent.
        if displayAllTabs:
            tabLayoutName = pm.mel.getRendererTabLayout(pm.melGlobals['gMasterLayerRendererName'])
        else:
            tabLayoutName = pm.mel.getRendererTabLayout(renderer)
        #print "TabLayoutName", tabLayoutName
        pm.setParent(tabLayoutName)        
        tabName = pm.mel.rendererTabName(renderer, tabLabel)
        #print "tabName", tabName
        
        commonTabNames = {
            'Common'             : "m_unifiedRenderGlobalsWindow.kCommon",
            'Passes'             : "m_unifiedRenderGlobalsWindow.kPassesTab",
            'Maya Software'      : "m_unifiedRenderGlobalsWindow.kMayaSoftware",
            'Maya Hardware'      : "m_unifiedRenderGlobalsWindow.kMayaHardware",
            'Maya Vector'        : "m_unifiedRenderGlobalsWindow.kMayaVector",
            'Features'           : "m_unifiedRenderGlobalsWindow.kFeatures",
            'Quality'            : "m_unifiedRenderGlobalsWindow.kQuality",
            'Indirect Lighting'  : "m_unifiedRenderGlobalsWindow.kIndirectLighting",
            'Options'            : "m_unifiedRenderGlobalsWindow.kOptions"
        }
        if commonTabNames.has_key(tabLabel):
            tabLabel = pm.mel.uiRes(commonTabNames[tabLabel])
                
        if not pm.layout(tabName, exists=True):
            pm.setUITemplate('renderGlobalsTemplate', pushTemplate=True)
            pm.setUITemplate('attributeEditorTemplate', pushTemplate=True)
            pm.formLayout(tabName)
            createProcs = ['createMayaSoftwareCommonGlobalsTab', 'createMayaSoftwareGlobalsTab']
            for renderer in ["Corona", "arnold", "Appleseed", "Thea", "Indigo", "Fuji", "Lux"]:
                try:
                    createProcs.extend(pm.renderer(renderer, q=True, globalsTabCreateProcNames=True))
                except:
                    pass
#             createProcs.extend(pm.renderer("Corona", q=True, globalsTabCreateProcNames=True))  
#             createProcs.extend(pm.renderer("Appleseed", q=True, globalsTabCreateProcNames=True))  
#             try:
#                 createProcs.extend(pm.renderer("arnold", q=True, globalsTabCreateProcNames=True))
#             except:
#                 pass
            if createProc in createProcs:
                pm.mel.eval(createProc)
                
            pm.setParent('..')    
            pm.setUITemplate(popTemplate=True)
            pm.setUITemplate(popTemplate=True)
            pm.tabLayout(tabLayoutName, edit=True, tabLabel=(tabName, tabLabel))
    
        pm.formLayout('tabForm', edit=True, manage=tabFormManagedStatus)
                    
    def updateMultiCameraBufferNamingMenu(self):
        #print "python cmd: updateMultiCameraBufferNamingMenu"
        pass
    
    def updateMayaSoftwareImageFormatControl(self):
        #print "python cmd: updateMayaSoftwareImageFormatControl"
        pass
    
    def addTabs(self):
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("CommonGlobals"))    
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Renderer"))
        self.addUserTabs()
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Translator"))    
        
    def createImageFormatControls(self):
        log.debug("createImageFormatControls()")
        self.createGlobalsNode()        
        self.createImageFormats()
        # self.imageFormatCtrl = pm.optionMenuGrp(label="Image Formats")
        
        if pm.optionMenuGrp("openMayaImageFormats", q=True, exists=True):
            pm.deleteUI("openMayaImageFormats")
            
        self.imageFormatCtrl = pm.optionMenuGrp("openMayaImageFormats", label="Image Formats", cc=pm.Callback(self.imageFormatCallback))                    
        
        for pr in self.imageFormats:
            log.debug("adding image format: " + pr)
            pm.menuItem(pr)
        # pm.scriptJob(attributeChange=[self.renderGlobalsNode.imageFormat, pm.Callback(self.imageFormatCallback)], parent = self.imageFormatCtrl)
        return self.imageFormatCtrl

    def updateImageFormatControls(self):
        log.debug("updateImageFormatControls()")
        self.createGlobalsNode()   
        selectedItem = self.renderGlobalsNode.imageFormat.get()
        enums = self.renderGlobalsNode.imageFormat.getEnums()
        selectedValue = "---"
        print enums, "Selected item", selectedItem
        for key in enums.keys():
            if enums[key] == selectedItem:
                selectedValue = key
        log.debug("updateImageFormatControls format: " + selectedValue)        
        self.imageFormatCtrl.setValue(selectedValue)

    def imageFormatCallback(self):
        log.debug("imageFormatCallback()")
        selectedItem = self.imageFormatCtrl.getSelect()
        selectedValue = self.imageFormatCtrl.getValue()
        
        id = self.renderGlobalsNode.imageFormat.getEnums()[selectedValue]
        selectedFormat = self.imageFormats[id]
            
        log.debug("Selected img format: {0}: {1}".format(selectedItem, selectedValue))
        self.renderGlobalsNode.imageFormat.set(id)
        
    def addUserTabs(self):
        pass
            
    def unRegisterRenderer(self):
        print "unregister"
        if pm.renderer(self.rendererName, q=True, exists=True):
            pm.renderer(self.rendererName, unregisterRenderer=True)
        pm.mel.source('createMayaSoftwareCommonGlobalsTab')
        pm.mel.source("unifiedRenderGlobalsWindow")
    
    def globalsTabCreateProcNames(self):
        pass
    
    def globalsTabLabels(self):
        pass

    def globalsTabUpdateProcNames(self):
        pass
        
    def changeIprRegionProcedure(self, *args, **kwargs):
        log.debug("changeIprRegionProcedure")
        print args
        print kwargs
        
    def iprOptionsProcedure(self):
        log.debug("iprOptionsProcedure")
            
    def iprOptionsMenuLabel(self):
        log.debug("iprOptionsMenuLabel")

    def iprRenderProcedure(self):
        log.debug("iprRenderProcedure")

    def iprRenderSubMenuProcedure(self):
        log.debug("iprRenderSubMenuProcedure")
            
    def isRunningIprProcedure(self):        
        log.debug("isRunningIprProcedure")
        return self.ipr_isrunning
            
    def pauseIprRenderProcedure(self, editor, pause):
        log.debug("pauseIprRenderProcedure")
            
    def refreshIprRenderProcedure(self):
        log.debug("refreshIprRenderProcedure")
            
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
            
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
            
    def logoCallbackProcedure(self):
        pass
    
    def logoImageName(self):
        return self.rendererName + ".png"
    
    def renderDiagnosticsProcedure(self):
        pass
    
    def renderGlobalsProcedure(self):
        pass

    def uiCallback(self, *args):
        pass
    
    def setImageName(self):
        self.renderGlobalsNode.basePath.set(pm.workspace.path)
        try:
            self.renderGlobalsNode.imagePath.set(pm.workspace.path + pm.workspace.fileRules['images'])
        except:
            self.renderGlobalsNode.imagePath.set(pm.workspace.path + 'images')
            
        sceneName = ".".join(pm.sceneName().basename().split(".")[:-1])
        
        # check for mayabatch name like sceneName_number 
        numberPart = sceneName.split("__")[-1]
        try:
            number = int(numberPart)
            if number > 999:
                sceneName = sceneName.replace("__" + numberPart, "")
        except:
            pass

        imageName = sceneName
        
        prefix = pm.SCENE.defaultRenderGlobals.imageFilePrefix.get()
        if prefix:
            if len(prefix) > 0:        
                settings = pm.api.MCommonRenderSettingsData()
                pm.api.MRenderUtil.getCommonRenderSettings(settings)
        
                cams = [cam for cam in pm.ls(type='camera') if cam.renderable.get()]
                if len(cams) < 1:
                    log.error("No renderable camera.")
                    prefix = ""
                prefix = prefix.replace("<Camera>", cams[0].name())
                prefix = prefix.replace("<Scene>", sceneName)
                imageName = prefix
            
        self.renderGlobalsNode.imageName.set(imageName)        
    
    
