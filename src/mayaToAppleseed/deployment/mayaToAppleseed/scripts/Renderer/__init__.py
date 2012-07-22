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
    
    # the render callback is called with arguments like this
    # renderCmd 640 480 1 1 perspShape " -layer defaultRenderLayer"
    # if the renderCmd is replaed with a python call it may not get all informations so we 
    # can thinkg about an alternative call like this
    # renderMelCmd pythonCmd 640 480 1 1 perspShape " -layer defaultRenderLayer"
    # this can result in a pythonCmd(640, 480, 1, 1, perspShape, " -layer defaultRenderLayer") call
    def renderCallback(self, procedureName):
        cmd = "python(\"{0}{1}()\")".format(self.baseRenderMelCommand, procedureName)
        print "callback: ", cmd
        return cmd
    
    def batchRenderProcedure(self):
        self.preRenderProcedure()
        log.debug("batchRenderProcedure")
    
    def renderOptionsProcedure(self):
        self.preRenderProcedure()
        log.debug("RenderOptionsProcedure")
    
    def commandRenderProcedure(self):
        self.preRenderProcedure()
        log.debug("commandRenderProcedure")
        self.renderProcedure()
        
    def renderProcedure(self):    
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
            #:TODO this is windows only, search for another solution...
            numThreads = int(os.environ['NUMBER_OF_PROCESSORS'])
            self.renderGlobalsNode.threads.set(numThreads)
        
    def postRenderProcedure(self):
        drg = pm.PyNode("defaultRenderGlobals")

    def preFrameProcedure(self):
        pass
    
    def postFrameProcedure(self):
        pass
                            
    def createGlobalsNode(self):      
        if not self.renderGlobalsNode: 
            # maybe the node has been replaced by replaced by a new loaded node, check this
            if len(pm.ls(self.renderGlobalsNodeName)) > 0:
                self.renderGlobalsNode = pm.ls(self.renderGlobalsNodeName)[0]
                log.debug("Globals node replaced.")
            else:
                self.renderGlobalsNode = pm.createNode(self.renderGlobalsNodeName)
                self.renderGlobalsNode.rename(self.renderGlobalsNodeName)
                log.debug("Created node "+ str(self.renderGlobalsNode))
        else:
            log.debug("renderlgobalsnode already defined: " + self.renderGlobalsNode)

    def registerNodeExtensions(self):
        pass
    
    def registerAETemplateCallbacks(self):
        log.debug("registerAETemplateCallbacks")
        # callback is defined as mel script, didn't work as pymel command.. 
        aeCallbackName = "AE{0}NodeCallback".format(self.rendererName)
        #aeCallbackName = "AEappleSeedNodeCallback"
        aeCallbackString="callbacks -addCallback %s -hook AETemplateCustomContent -owner %s;" % (aeCallbackName, self.pluginName)
        pm.mel.eval(aeCallbackString)
        
        aeTemplateName = "AE{0}NodeTemplate".format(self.rendererName.lower())
        #aeTemplate = "AEappleSeedNodeTemplate"
        aeCallbackProc = "global proc " + aeCallbackName + "(string $nodeName)\n"
        aeCallbackProc += "{\n"
        aeCallbackProc += "string $cmd = \"import " + aeTemplateName + " as aet; aet." + aeTemplateName + "(\\\"\" + $nodeName + \"\\\")\";\n"
        aeCallbackProc += "python($cmd);\n"
        aeCallbackProc += "}\n"
        log.debug("aeCallback: " + aeCallbackProc)
        pm.mel.eval(aeCallbackProc)
    
    def registerRenderer(self):
        log.debug("registerRenderer")
        self.unRegisterRenderer()
        self.registerNodeExtensions()
        self.registerAETemplateCallbacks()
        pm.renderer(self.rendererName, rendererUIName = self.rendererName)
        pm.renderer(self.rendererName, edit = True, renderProcedure = self.renderCallback("renderProcedure"))
        pm.renderer(self.rendererName, edit = True, batchRenderProcedure =  self.renderCallback("batchRenderProcedure"))
        pm.renderer(self.rendererName, edit = True, commandRenderProcedure =  self.renderCallback("commandRenderProcedure"))
        pm.renderer(self.rendererName, edit = True, batchRenderOptionsProcedure =  self.renderCallback("batchRenderOptionsProcedure"))
        pm.renderer(self.rendererName, edit = True, batchRenderOptionsStringProcedure =  self.renderCallback("batchRenderOptionsStringProcedure"))
        pm.renderer(self.rendererName, edit = True, addGlobalsNode = "defaultRenderGlobals")
        pm.renderer(self.rendererName, edit = True, addGlobalsNode = "defaultResolution")
        pm.renderer(self.rendererName, edit = True, addGlobalsNode = self.renderGlobalsNodeName)
        
        # because mentalray is still hardcoded in the maya scritps, I cannot simply use my own commons without replacing some original scripts
        # so I use the defaults
        pm.renderer(self.rendererName, edit = True, addGlobalsTab = ('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"))

        # my own tabs
        pm.renderer(self.rendererName, edit = True, addGlobalsTab = self.renderTabMelProcedure("Renderer"))    
        pm.renderer(self.rendererName, edit = True, addGlobalsTab = self.renderTabMelProcedure("Translator"))    
        
        log.debug("RegisterRenderer done")
        
    def unRegisterRenderer(self):
        if pm.renderer(self.rendererName, q = True, exists = True):
            pm.renderer(self.rendererName, unregisterRenderer = True)
