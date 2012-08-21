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
        print "MelCmd:", melCmd
        pm.mel.eval(melCmd)
        return melProcName
            
    
    def batchRenderProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("batchRenderProcedure")
    
    def renderOptionsProcedure(self):
        self.preRenderProcedure()
        log.debug("RenderOptionsProcedure")
    
    def commandRenderProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("commandRenderProcedure")
        self.renderProcedure()
        
    def renderProcedure(self, width, height, doShadows, doGlowPass, camera, options):
        pass
    
    def batchRenderOptionsProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("batchRenderOptionsProcedure")
    
    def batchRenderOptionsStringProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("batchRenderOptionsStringProcedure")
    
    def cancelBatchRenderProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("cancelBatchRenderProcedure")
    
    def showBatchRenderProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("showBatchRenderProcedure")
    
    def showRenderLogProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("showRenderLogProcedure")
    
    def showBatchRenderLogProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("showBatchRenderLogProcedure")
    
    def renderRegionProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("renderRegionProcedure")
    
    def textureBakingProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("textureBakingProcedure")
    
    def renderingEditorsSubMenuProcedure(self, width, height, doShadows, doGlow, camera, options):
        self.preRenderProcedure()
        log.debug("renderingEditorsSubMenuProcedure")
    
    def renderMenuProcedure(self, width, height, doShadows, doGlow, camera, options):
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
                log.debug("Created node " + str(self.renderGlobalsNode))
        else:
            log.debug("renderlgobalsnode already defined: " + self.renderGlobalsNode)

    def registerNodeExtensions(self):
        pass
    
    def registerAETemplateCallbacks(self):
        log.debug("registerAETemplateCallbacks")
        # callback is defined as mel script, didn't work as pymel command.. 
        aeCallbackName = "AE{0}NodeCallback".format(self.rendererName)
        #aeCallbackName = "AEappleSeedNodeCallback"
        aeCallbackString = "callbacks -addCallback %s -hook AETemplateCustomContent -owner %s;" % (aeCallbackName, self.pluginName)
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
    

#    args['renderProcedure'] = utils.pyToMelProc(arnoldRender.arnoldRender,
#                                          [('int', 'width'), ('int', 'height'),
#                                           ('int', 'doShadows'), ('int', 'doGlowPass'),
#                                           ('string', 'camera'), ('string', 'options')])
#    args['renderRegionProcedure'] = 'mayaRenderRegion'
#    args['commandRenderProcedure']    = utils.pyToMelProc(arnoldRender.arnoldBatchRender,
#                                                    [('string', 'option')])
#    args['batchRenderProcedure']      = utils.pyToMelProc(arnoldRender.arnoldBatchRender,
#                                                    [('string', 'option')])
#    args['cancelBatchRenderProcedure']= utils.pyToMelProc(arnoldRender.arnoldBatchStop)
#    args['iprRenderProcedure']        = utils.pyToMelProc(arnoldRender.arnoldIprRender,
#                                                    [('int', 'width'), ('int', 'height'),
#                                                     ('int', 'doShadows'), ('int', 'doGlowPass'),
#                                                     ('string', 'camera')])
#    args['isRunningIprProcedure']     = utils.pyToMelProc(arnoldRender.arnoldIprIsRunning, returnType='int')
#    args['startIprRenderProcedure']   = utils.pyToMelProc(arnoldRender.arnoldIprStart,
#                                                    [('string', 'editor'), ('int', 'resolutionX'),
#                                                     ('int', 'resolutionY'), ('string', 'camera')])
#    args['stopIprRenderProcedure']    = utils.pyToMelProc(arnoldRender.arnoldIprStop)
#    args['refreshIprRenderProcedure'] = utils.pyToMelProc(arnoldRender.arnoldIprRefresh)
#    args['pauseIprRenderProcedure']   = utils.pyToMelProc(arnoldRender.arnoldIprPause,
#                                                    [('string', 'editor'), ('int', 'pause')])
#    args['changeIprRegionProcedure']  = utils.pyToMelProc(arnoldRender.arnoldIprChangeRegion,
#                                                    [('string', 'renderPanel')])
#    pm.renderer('arnold', rendererUIName='Arnold Renderer', **args)
#        
#    pm.renderer('arnold', edit=True, addGlobalsTab=('Common',
#                                                      utils.pyToMelProc(createArnoldRendererCommonGlobalsTab, useName=True),
#                                                      utils.pyToMelProc(updateArnoldRendererCommonGlobalsTab, useName=True)))
#    pm.renderer('arnold', edit=True, addGlobalsTab=('Arnold Renderer',
#                                                      utils.pyToMelProc(createArnoldRendererGlobalsTab, useName=True),
#                                                      utils.pyToMelProc(updateArnoldRendererGlobalsTab, useName=True)))
#    pm.renderer('arnold', edit=True, addGlobalsTab=('AOVs', 
#                                                      utils.pyToMelProc(createArnoldAOVTab, useName=True), 
#                                                      utils.pyToMelProc(updateArnoldAOVTab, useName=True)))
#    pm.renderer('arnold', edit=True, addGlobalsNode='defaultArnoldRenderOptions')
#    utils.pyToMelProc(updateBackgroundSettings, useName=True)
#    #We have to source this file otherwise maya will override
#    #our mel proc overrides below.
#    #
#    pm.mel.source('createMayaSoftwareCommonGlobalsTab.mel')
#    
#    utils.pyToMelProc(addOneTabToGlobalsWindow,
#                      [('string', 'renderer'), ('string', 'tabLabel'), ('string', 'createProc')],
#                      useName=True)
#    utils.pyToMelProc(renderSettingsTabLabel_melToUI,
#                      [('string', 'mel')],
#                      useName=True)
#    utils.pyToMelProc(updateMayaImageFormatControl,
#                      useName=True)
    
    
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
        #pm.renderer(self.rendererName, edit=True, renderProcedure=self.renderCallback("renderProcedure"))
        pm.renderer(self.rendererName, edit=True, batchRenderProcedure=self.renderCallback("batchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, commandRenderProcedure=self.renderCallback("commandRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, batchRenderOptionsProcedure=self.renderCallback("batchRenderOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, batchRenderOptionsStringProcedure=self.renderCallback("batchRenderOptionsStringProcedure"))
        pm.renderer(self.rendererName, edit=True, addGlobalsNode="defaultRenderGlobals")
        pm.renderer(self.rendererName, edit=True, addGlobalsNode="defaultResolution")
        pm.renderer(self.rendererName, edit=True, addGlobalsNode=self.renderGlobalsNodeName)
        
        pm.renderer(self.rendererName, edit=True, changeIprRegionProcedure=self.renderCallback("changeIprRegionProcedure"))
        pm.renderer(self.rendererName, edit=True, iprOptionsProcedure=self.renderCallback("iprOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, iprOptionsMenuLabel=self.renderCallback("iprOptionsMenuLabel"))
        pm.renderer(self.rendererName, edit=True, iprRenderProcedure=self.renderCallback("iprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, iprRenderSubMenuProcedure=self.renderCallback("iprRenderSubMenuProcedure"))
        pm.renderer(self.rendererName, edit=True, isRunningIprProcedure=self.renderCallback("isRunningIprProcedure"))
        pm.renderer(self.rendererName, edit=True, pauseIprRenderProcedure=self.renderCallback("pauseIprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, refreshIprRenderProcedure=self.renderCallback("refreshIprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, stopIprRenderProcedure=self.renderCallback("stopIprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, startIprRenderProcedure=self.renderCallback("startIprRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, logoCallbackProcedure=self.renderCallback("logoCallbackProcedure"))
        pm.renderer(self.rendererName, edit=True, logoImageName=self.renderCallback("logoImageName"))
        pm.renderer(self.rendererName, edit=True, renderDiagnosticsProcedure=self.renderCallback("renderDiagnosticsProcedure"))
        
        pm.renderer(self.rendererName, edit=True, renderOptionsProcedure=self.renderCallback("renderOptionsProcedure"))
        pm.renderer(self.rendererName, edit=True, cancelBatchRenderProcedure=self.renderCallback("cancelBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderProcedure=self.renderCallback("showBatchRenderProcedure"))
        pm.renderer(self.rendererName, edit=True, showRenderLogProcedure=self.renderCallback("showRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, showBatchRenderLogProcedure=self.renderCallback("showBatchRenderLogProcedure"))
        pm.renderer(self.rendererName, edit=True, renderRegionProcedure=self.renderCallback("renderRegionProcedure"))
        pm.renderer(self.rendererName, edit=True, textureBakingProcedure=self.renderCallback("textureBakingProcedure"))
        pm.renderer(self.rendererName, edit=True, renderingEditorsSubMenuProcedure=self.renderCallback("renderingEditorsSubMenuProcedure"))
        pm.renderer(self.rendererName, edit=True, renderMenuProcedure=self.renderCallback("renderMenuProcedure"))
            
        
        # because mentalray is still hardcoded in the maya scritps, I cannot simply use my own commons without replacing some original scripts
        # so I use the defaults
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=('Common', "createMayaSoftwareCommonGlobalsTab", "updateMayaSoftwareCommonGlobalsTab"))

        # my own tabs
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Renderer"))    
        self.addUserTabs()
        pm.renderer(self.rendererName, edit=True, addGlobalsTab=self.renderTabMelProcedure("Translator"))    
        
        
        log.debug("RegisterRenderer done")

    def addUserTabs(self):
        pass
            
    def unRegisterRenderer(self):
        if pm.renderer(self.rendererName, q=True, exists=True):
            pm.renderer(self.rendererName, unregisterRenderer=True)
    
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
            
    def pauseIprRenderProcedure(self):
        log.debug("pauseIprRenderProcedure")
        pass
            
    def refreshIprRenderProcedure(self):
        log.debug("refreshIprRenderProcedure")
        pass
            
    def stopIprRenderProcedure(self):
        self.ipr_isrunning = False
        log.debug("stopIprRenderProcedure")
        pass
            
    def startIprRenderProcedure(self):
        self.ipr_isrunning = True
        log.debug("startIprRenderProcedure")
        pass
            
    def logoCallbackProcedure(self):
        pass
            
    def logoImageName(self):
        pass
    
    def renderDiagnosticsProcedure(self):
        pass
    
    def renderGlobalsProcedure(self):
        pass
    
