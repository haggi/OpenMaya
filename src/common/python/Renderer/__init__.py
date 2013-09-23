import pymel.core as pm
import logging
import sys
import os

log = logging.getLogger("renderLogger")

class MayaToRenderer(object):
    
    def __init__(self, rendererName, moduleName):
        self.rendererName = rendererName
        self.moduleName = moduleName
        self.pluginName = "mayato" + self.rendererName.lower()
        self.renderGlobalsNodeName = self.rendererName.lower() + "Globals"
        self.renderGlobalsNode = None
        #self.baseRenderMelCommand = "import {0} as rcall; reload(rcall); rcall.theRenderer().".format(self.moduleName)
        self.baseRenderMelCommand = "import {0} as rcall; rcall.theRenderer().".format(self.moduleName)
        self.imageFormats = []
        self.ipr_isrunning = False
        self.imageFormatCtrl = None
    
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
        pCmd  = "\"{0}{1}(".format(self.baseRenderMelCommand, pythonCmdObj.__name__)
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
        #print "MelCmd:", melCmd
        pm.mel.eval(melCmd)
        return melProcName

    def getEnumList(self, attr):
        return [(i, v) for i,v in enumerate(attr.getEnums().keys())]
    
    def addUIElement(self, uiType, attribute, uiLabel, callback):
        ui = None
        if uiType == 'bool':
            ui = pm.checkBoxGrp(label=uiLabel)
            if callback is not None:
                pm.checkBoxGrp(ui, cc=callback)
        if uiType == 'int':
            ui = pm.intFieldGrp(label=uiLabel, numberOfFields = 1)
        if uiType == 'float':
            ui = pm.floatFieldGrp(label=uiLabel, numberOfFields = 1)
        if uiType == 'enum':
            ui = pm.attrEnumOptionMenuGrp(label = uiLabel, at=attribute, ei = self.getEnumList(attribute)) 
            # attrEnumOptionGrp has no cc callback, so I create a script job
            if callback is not None:
                attribute = pm.Attribute(self.renderGlobalsNodeName + "." + attribute)
                pm.scriptJob(attributeChange=[attribute, callback], parent=ui)           
        if uiType == 'color':
            ui = pm.attrColorSliderGrp(label=uiLabel, at=attribute)
        if uiType == 'string':
            ui = pm.textFieldGrp(label=uiLabel)
            if callback is not None:
                pm.textFieldGrp(ui, cc=callback)
        return ui
    
    def connectUIElement(self, uiElement, attribute):
        
        if attribute.type() == 'enum':
            #enum is automatically connnected via attrEnumOptionMenu
            return        
        if attribute.type() == 'float3':
            #float3 == color is automatically connnected via attrColorSliderGrp
            return
        if attribute.type() == 'message':
            #no automatic connection necessary, will be controlled by other scritps
            return
        log.debug("Adding connection for {0}".format(attribute))
        pm.connectControl(uiElement, attribute, index = 2)
    
    def addRenderGlobalsUIElement(self, attName = None, uiType = None, displayName = None, default=None, data=None, uiDict=None, callback=None):
        
        attribute = pm.Attribute(self.renderGlobalsNodeName + "." + attName)
        uiElement = self.addUIElement(uiType, attribute, displayName, callback)
        self.connectUIElement(uiElement, attribute)
        uiDict[attName] = uiElement
                
    
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
    
    def renderingEditorsSubMenuProcedure(self):
        self.preRenderProcedure()
        log.debug("renderingEditorsSubMenuProcedure")
    
    def renderMenuProcedure(self):
        self.preRenderProcedure()
        log.debug("renderMenuProcedure")
            
    # render tab creation. Renderer tabs need a mel globals procedure.
    # a mel procedure is called for create and update tab
    # here I create a fake mel procedure to call my own python command
    
    # per default, the inheriting class will implement the procedures:
    # {Name}RendererCreateTab() and the {Name}RendererUpdateTab()
    # {Name}TranslatorCreateTab() and the {Name}TranslatorUpdateTab()
    def renderTabMelProcedure(self, tabName):
        
        createTabCmd = "{0}{1}CreateTab".format(self.rendererName, tabName) 
        updateTabCmd = "{0}{1}UpdateTab".format(self.rendererName, tabName) 
        tabLabel = "{0}{1}".format(self.rendererName, tabName)
        createPyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, createTabCmd)
        updatePyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, updateTabCmd)
        melCreateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(createTabCmd, createPyCmd)
        melUpdateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(updateTabCmd, updatePyCmd)
        
        if tabName == "OpenMayaCommonGlobals":
            createTabCmd = "{0}CreateTab".format(tabName) 
            updateTabCmd = "{0}UpdateTab".format(tabName) 
            tabLabel = "Common"
            createPyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, createTabCmd)
            updatePyCmd = "python(\"{0}{1}()\");".format(self.baseRenderMelCommand, updateTabCmd)
            melCreateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(createTabCmd, createPyCmd)
            melUpdateCmd = "global proc {0}()\n{{\t{1}\n}}\n".format(updateTabCmd, updatePyCmd)
        
        log.debug("mel create tab: \n" + melCreateCmd)
        log.debug("mel update tab: \n" + melUpdateCmd)
        log.debug("python create tab: \n" + createPyCmd)
        log.debug("python update tab: \n" + updatePyCmd)
        pm.mel.eval(melCreateCmd)
        pm.mel.eval(melUpdateCmd)
        return (tabLabel, createTabCmd, updateTabCmd)
    
    def createImageFormats(self):
        pass
        
    def preRenderProcedure(self):
        self.createGlobalsNode()
        drg = pm.PyNode("defaultRenderGlobals")
        if self.renderGlobalsNode.threads.get() == 0:
            #TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        #craete optimized exr textures
        
        
    def postRenderProcedure(self):
        drg = pm.PyNode("defaultRenderGlobals")

    def preFrameProcedure(self):
        pass
    
    def postFrameProcedure(self):
        pass

    def overwriteUpdateMayaImageFormatControl(self):
        melCmdCreateA = """
global proc string createMayaImageFormatControl()
{
    string $currentRenderer = currentRenderer();
    if( $currentRenderer == "mentalRay" )
        return createMRImageFormatControl();
        """
        melCmdCreateB = """
    string $parent = `setParent -query`;

    // Delete the control if it already exists
    //
    string $fullPath = $parent + "|imageMenuMayaSW";
    if (`layout -exists $fullPath`) {
        deleteUI $fullPath;
    }

    optionMenuGrp
        -label (uiRes("m_createMayaSoftwareCommonGlobalsTab.kImageFormatMenu"))
        -changeCommand "changeMayaSoftwareImageFormat"
        imageMenuMayaSW;

    int $isVector = 0;
    if( $currentRenderer == "mayaVector" )
        $isVector = 1;

    buildImageFormatsMenu($isVector, 1, 1, 1, 1);

    // connect the label, so we can change its color
    connectControl -index 1 imageMenuMayaSW defaultRenderGlobals.imageFormat;
    // connect the menu, so it will always match the attribute
    connectControl -index 2 imageMenuMayaSW defaultRenderGlobals.imageFormat;

    scriptJob
        -parent $parent
        -attributeChange
            "defaultRenderGlobals.imageFormat"
            "updateMayaSoftwareImageFormatControl";

    return "imageMenuMayaSW";
}

global proc updateMayaImageFormatControl()
{
    string $currentRenderer = currentRenderer();
        """
        melCmdCreateC = """
    else if( $currentRenderer == "mentalRay" )
        updateMentalRayImageFormatControl();
    else
        updateMayaSoftwareImageFormatControl();

    //update the Mulitple camera frame buffer naming control
        updateMultiCameraBufferNamingMenu();
}
        """
        
        rendererCmdA = "\n\tif( $currentRenderer == \"{0}\" )\n\
        return {1};".format(self.rendererName, "python(\"minit.theRenderer().createImageFormatControls()\")")
        rendererCmdB = "\n\tif( $currentRenderer == \"{0}\" )\n\
        python(\"minit.theRenderer().updateImageFormatControls()\");".format(self.rendererName)

        melCmd = melCmdCreateA + rendererCmdA +  melCmdCreateB + rendererCmdB + melCmdCreateC 
        print "Complete mel command", melCmd   
        pm.mel.eval(melCmd)
    
    def unDoOverwriteUpdateMayaImageFormatControl(self):
        pm.mel.eval("source createMayaSoftwareCommonGlobalsTab")
        pass
    
    def createGlobalsNode(self):      
        selection = pm.ls(sl=True)
        if not self.renderGlobalsNode: 
            # maybe the node has been replaced by replaced by a new loaded node, check this
            if len(pm.ls(self.renderGlobalsNodeName)) > 0:
                self.renderGlobalsNode = pm.ls(self.renderGlobalsNodeName)[0]
                log.debug("Globals node replaced.")
                self.afterGlobalsNodeReplacement()
            else:
                self.renderGlobalsNode = pm.createNode(self.renderGlobalsNodeName)
                self.renderGlobalsNode.rename(self.renderGlobalsNodeName)
                log.debug("Created node " + str(self.renderGlobalsNode))
                optimizedPath = pm.workspace.path / pm.workspace.fileRules['renderData'] / "optimizedTextures"
                if not os.path.exists(optimizedPath):
                    optimizedPath.makedirs()
                self.renderGlobalsNode.optimizedTexturePath.set(str(optimizedPath))
                self.afterGlobalsNodeReplacement()
        else:
            log.debug("renderlgobalsnode already defined: " + self.renderGlobalsNode)
        pm.select(selection)
        
    def registerNodeExtensions(self):
        pass

    def hyperShadePanelBuildCreateMenuCallback(self):
        #log.debug("hyperShadePanelBuildCreateMenuCallback")
        pm.menuItem(label=self.rendererName)
        pm.menuItem(divider=True)
    
    def hyperShadePanelBuildCreateSubMenuCallback(self):
        #log.debug("hyperShadePanelBuildCreateSubMenuCallback")
        return "shader/surface"
    
    def buildRenderNodeTreeListerContentCallback(self, tl, postCommand, filterString):
        #log.debug("buildRenderNodeTreeListerContentCallback")        

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

        # this is necessary for < maya2014 because in these versions the mentalray plugin somehow destroys the callback call
        if len(tl) > 0:
            melCmd = 'addToRenderNodeTreeLister( "{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(tl, postCommand, self.rendererName + "/Materials", self.rendererName.lower() + "/material", "-asShader", "")
            #melCmd = 'addToRenderNodeTreeLister( "{0}", "{1}", "{2}", "{3}", "{4}", "{5}");'.format(tl, postCommand, self.rendererName + "/Textures", "lux/shader/texture", "-asUtility", "")
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
        #aeCallbackName = "AE{0}NodeCallback".format(self.rendererName)
        #aeCallbackString = "callbacks -addCallback {0} -hook AETemplateCustomContent -owner {1};".format(aeCallbackName, self.pluginName)
        #pm.mel.eval(aeCallbackString)

        pm.callbacks(addCallback=self.aeTemplateCallback, hook='AETemplateCustomContent', owner=self.pluginName)
        

#        aeCallbackName = "{0}hyperShadePanelBuildCreateMenuCallback"
#        aeCallbackString = "callbacks -addCallback {0} -hook hyperShadePanelBuildCreateMenu -owner {1};".format(aeCallbackName, self.pluginName)
#
#        aeCallbackName = "{0}hyperShadePanelBuildCreateSubMenuCallback".format(self.rendererName)
#        aeCallbackString = "callbacks -addCallback {0} -hook hyperShadePanelBuildCreateSubMenu -owner {1};".format(aeCallbackName, self.pluginName)

        pm.callbacks(addCallback=self.hyperShadePanelBuildCreateMenuCallback, hook='hyperShadePanelBuildCreateMenu', owner=self.pluginName)
        pm.callbacks(addCallback=self.hyperShadePanelBuildCreateSubMenuCallback, hook='hyperShadePanelBuildCreateSubMenu', owner=self.pluginName)
        pm.callbacks(addCallback=self.buildRenderNodeTreeListerContentCallback, hook='buildRenderNodeTreeListerContent', owner=self.pluginName)
        
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
        #pm.renderer(self.rendererName, edit=True, batchRenderProcedure=self.renderCallback("batchRenderProcedure"))
        #pm.renderer(self.rendererName, edit=True, commandRenderProcedure=self.renderCallback("commandRenderProcedure"))
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
        pm.renderer(self.rendererName, edit=True, logoCallbackProcedure=self.renderCallback("logoCallbackProcedure"))
        pm.renderer(self.rendererName, edit=True, logoImageName=self.rendererName + ".png")
        pm.renderer(self.rendererName, edit=True, renderDiagnosticsProcedure=self.renderCallback("renderDiagnosticsProcedure"))
        
        pm.renderer(self.rendererName, edit=True, renderOptionsProcedure=self.renderCallback("renderOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, cancelBatchRenderProcedure=self.renderCallback("cancelBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderProcedure=self.renderCallback("showBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showRenderLogProcedure=self.renderCallback("showRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderLogProcedure=self.renderCallback("showBatchRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, textureBakingProcedure=self.renderCallback("textureBakingProcedure"))
        pm.renderer(self.rendererName, edit=True, renderingEditorsSubMenuProcedure=self.renderCallback("renderingEditorsSubMenuProcedure"))
        pm.renderer(self.rendererName, edit=True, renderMenuProcedure=self.renderCallback("renderMenuProcedure"))
            
        pm.renderer(self.rendererName, edit=True, renderRegionProcedure="mayaRenderRegion")
                
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"))
        #self.overwriteUpdateMayaImageFormatControl()
        # because mentalray is still hardcoded in the maya scripts, I cannot simply use my own commons without replacing some original scripts
        # so I use the defaults
        #pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("OpenMayaCommonGlobals"))    
        
        # my own tabs
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Renderer"))    
        self.addUserTabs()
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Translator"))    
        
        pm.mel.source('createMayaSoftwareCommonGlobalsTab.mel')
        #pm.evalDeferred(self.overwriteUpdateMayaImageFormatControl)
        self.overwriteUpdateMayaImageFormatControl()
        
        log.debug("RegisterRenderer done")

    def createImageFormatControls(self):
        log.debug("createImageFormatControls()")
        self.createGlobalsNode()        
        self.createImageFormats()
        #self.imageFormatCtrl = pm.optionMenuGrp(label="Image Formats")
        
        if pm.optionMenuGrp("openMayaImageFormats", q=True, exists=True):
            pm.deleteUI("openMayaImageFormats")
            
        self.imageFormatCtrl = pm.optionMenuGrp("openMayaImageFormats",label="Image Formats", cc=pm.Callback(self.imageFormatCallback))                    
        
        for pr in self.imageFormats:
            log.debug("adding image format: " + pr)
            pm.menuItem(pr)
        #pm.scriptJob(attributeChange=[self.renderGlobalsNode.imageFormat, pm.Callback(self.imageFormatCallback)], parent = self.imageFormatCtrl)
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
        if pm.renderer(self.rendererName, q=True, exists=True):
            pm.renderer(self.rendererName, unregisterRenderer=True)
            #self.unDoOverwriteUpdateMayaImageFormatControl()
    
    def globalsTabCreateProcNames(self):
        pass
    
    def globalsTabLabels(self):
        pass

    def globalsTabUpdateProcNames(self):
        pass
        
    def changeIprRegionProcedure(self):
        log.debug("changeIprRegionProcedure")
        pass

    def iprOptionsProcedure(self):
        log.debug("iprOptionsProcedure")
        pass
            
    def iprOptionsMenuLabel(self):
        log.debug("iprOptionsMenuLabel")
        pass

    def iprRenderProcedure(self):
        log.debug("iprRenderProcedure")
        pass

    def iprRenderSubMenuProcedure(self):
        log.debug("iprRenderSubMenuProcedure")
        pass
            
    def isRunningIprProcedure(self):
        
        return self.ipr_isrunning
        log.debug("isRunningIprProcedure")
        pass
            
    def pauseIprRenderProcedure(self, editor, pause):
        log.debug("pauseIprRenderProcedure")
            
    def refreshIprRenderProcedure(self):
        log.debug("refreshIprRenderProcedure")
        pass
            
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
            
    def startIprRenderProcedure(self, editor, resolutionX, resolutionY, camera):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        pass
            
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
    
    
