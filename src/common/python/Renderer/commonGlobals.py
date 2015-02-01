import pymel.core as pm
import logging
import uiUtils

log = logging.getLogger("renderLogger")

class OpenMayaCommonGlobals(object):
    def __init__(self, renderNode=None):
        print "CREATE OpenMayaCommonGlobals"
        self.renderGlobalsNodeName = renderNode.name()
        self.renderNode = renderNode
        self.defaultGlobals = pm.SCENE.defaultRenderGlobals
        self.defaultResolution = pm.SCENE.defaultResolution
        self.rendererTabUiDict = {}
        self.imageFormatData = []
        self.cameras = []
        imageFormats = pm.getMelGlobal("string[]", "$gImageFormatData")
        if len(imageFormats) == 0:
            pm.mel.eval("source imageFormats")  # make sure we have the $gImageFormatData global variable
            imageFormats = pm.getMelGlobal("string[]", "$gImageFormatData")
        self.imageFormatData.append(("Custom", 320, 240))
        for line in imageFormats:
            elements = line.split()
            self.imageFormatData.append((elements[0], int(elements[1]), int(elements[2])))
            
    def addRenderDefaultGlobalsUIElement(self, attName=None, uiType=None, displayName=None, default=None, data=None, uiDict=None, callback=None):
        uiUtils.addRenderGlobalsUIElement(self.defaultGlobals.name(), attName, uiType, displayName, default, data, uiDict, callback)

    def addRenderDefaultResGlobalsUIElement(self, attName=None, uiType=None, displayName=None, default=None, data=None, uiDict=None, callback=None):
        uiUtils.addRenderGlobalsUIElement(self.defaultResolution.name(), attName, uiType, displayName, default, data, uiDict, callback)

    def addRenderGlobalsUIElement(self, attName=None, uiType=None, displayName=None, default=None, data=None, uiDict=None, callback=None):
        uiUtils.addRenderGlobalsUIElement(self.renderGlobalsNodeName, attName, uiType, displayName, default, data, uiDict, callback)

    def getFileNameExtensions(self):
        return uiUtils.getEnumList(self.renderNode.imageFormat)

    def switchCamRenderable(self, cam):
        cam.renderable.set(not cam.renderable.get())

    def createImageSize(self, parent):
        pass
    
    def setImageSize(self, preset):
        for imgFormat in self.imageFormatData:
            if imgFormat[0] == preset:
                self.defaultResolution.aspectLock.set(0)
                self.defaultResolution.width.set(imgFormat[1])
                self.defaultResolution.height.set(imgFormat[2])
                self.defaultResolution.deviceAspectRatio.set(float(imgFormat[1]) / float(imgFormat[2]))
    
    def updateImageSize(self, *args):
                        
        w = float(self.defaultResolution.width.get())
        h = float(self.defaultResolution.height.get())
        dar = float(self.defaultResolution.deviceAspectRatio.get())
        aspectLock = self.defaultResolution.aspectLock.get()
        
        
        if args is None or len(args) == 0:
            return
                        
        if args[0] == "width":
            if aspectLock:
                h = w / dar
        if args[0] == "height":
            if aspectLock:
                w = h * dar
        if args[0] == "devAsp":
            h = w / dar
        darNew = w / h
        
        self.defaultResolution.deviceAspectRatio.set(darNew)
        self.defaultResolution.width.set(w)
        self.defaultResolution.height.set(h)

        defrw = pm.SCENE.defaultResolution.width
        defrh = pm.SCENE.defaultResolution.height
        defrd = pm.SCENE.defaultResolution.deviceAspectRatio
        if args[0] == "width":
            pm.scriptJob(attributeChange=(defrw, pm.Callback(self.updateImageSize, ["width"])), runOnce=True)
        if args[0] == "height":
            pm.scriptJob(attributeChange=(defrh, pm.Callback(self.updateImageSize, ["height"])), runOnce=True)
        if args[0] == "devAsp":
            pm.scriptJob(attributeChange=(defrd, pm.Callback(self.updateImageSize, ["devAsp"])), runOnce=True)

        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        
        if not pm.optionMenuGrp(uiDict['imageSizePresets'], exists=True):
            log.debug("imageSizePresets Option menu does not exist.")
            return
        
        for imgFormat in self.imageFormatData:            
            if w == imgFormat[1] and h == imgFormat[2]:
                uiDict['imageSizePresets'].setSelect(self.imageFormatData.index(imgFormat) + 1) 
                return
        uiDict['imageSizePresets'].setSelect(1)
            
    def setFrameNumbering(self, args):
        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        if uiDict['imageNumbering'].getValue() == "name.ext":
            self.defaultGlobals.animation.set(0)
            self.defaultGlobals.periodInExt.set(1)
        else:
            self.defaultGlobals.animation.set(1)
            self.defaultGlobals.putFrameBeforeExt.set(1)
            self.defaultGlobals.periodInExt.set(1)
        self.updateFrameSettings()

    def createExrUI(self):
        pass
    
    def updateExrUI(self):
        ext = self.renderNode.imageFormat.getEnums().keys()[self.renderNode.imageFormat.get()]
        print "updateExrUI imageFormat extension", ext
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']
            if ext.lower() == "exr":
                print "Exr, activating contrls"
                uiDict['exrOptionsLayout'].setEnable(True)
            else:
                uiDict['exrOptionsLayout'].setEnable(False)
    
    def createCamerasUI(self, uiDict):
        with pm.columnLayout(adjustableColumn=True, width=400) as uiDict['camerasCL']:
            for cam in pm.ls(type="camera"):
                pm.checkBoxGrp(label=cam.name(), value1=cam.renderable.get(), cc=pm.Callback(self.switchCamRenderable, cam))
                self.cameras.append(cam.name())
                    
    def updateCamerasUI(self):
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']
            pm.deleteUI(uiDict['camerasCL'])
            with pm.columnLayout(adjustableColumn=True, width=400, parent=uiDict['camerasFrame']) as uiDict['camerasCL']:
                for cam in pm.ls(type="camera"):
                    pm.checkBoxGrp(label=cam.name(), value1=cam.renderable.get(), cc=pm.Callback(self.switchCamRenderable, cam))

    def selectionChangedEvent(self):
        try:
            sel = pm.ls(sl=True, type="camera")
            if len(sel) == 0:
                for t in pm.ls(sl=True, type="transform"):
                    s = t.getShape()
                    if s.type() == "camera":     
                        sel.append(s)
            for c in sel:
                if c.name() not in self.cameras or c.renderable.get() != pm.PyNode(self.cameras[self.cameras.index[c.name()]].renderable.get()):
                    self.updateCamerasUI()
        except:
            pass

    def updateFilePath(self, *args):   
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']

    def updateFilePathName(self, *args):   
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']
    
    def updateFileImageSize(self, *args):   
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']

    def updateFileNamePrefix(self, *args):   
        if self.rendererTabUiDict.has_key('common'):            
            uiDict = self.rendererTabUiDict['common']
        prefix = uiDict['fileNamePrefixField'].getText()

        sceneName = ".".join(pm.sceneName().basename().split(".")[:-1])
        if len(prefix) == 0:
            prefix = sceneName
            uiDict['imgname'].setLabel("File Name: " + sceneName)   
        
        settings = pm.api.MCommonRenderSettingsData()
        pm.api.MRenderUtil.getCommonRenderSettings(settings)

        cams = [cam for cam in pm.ls(type='camera') if cam.renderable.get()]
        if len(cams) < 1:
            log.error("No renderable camera. please make at least one camera renderable.")
            prefix = ""
            for c in pm.ls(type='camera'):
                if "perspShape" in  c:
                    cams.append(c)
                    break
        prefix = prefix.replace("<Camera>", cams[0].name())
        
        prefix = prefix.replace("<Scene>", sceneName)
                
        ext = self.renderNode.imageFormat.getEnums().keys()[self.renderNode.imageFormat.get()].lower()
        numberFormat = ""
        paddedFrameString = "{0:0" + str(settings.framePadding) + "d}."
        if settings.isAnimated():
            numberFormat = paddedFrameString.format(int(pm.SCENE.defaultRenderGlobals.startFrame.get()))
            
        completeFileName = "{prefix}.{numbering}{ext}".format(prefix=prefix, numbering=numberFormat, ext=ext)
        
        uiDict['imgname'].setLabel("File Name: " + completeFileName)   
        """
        <Scene>
        <RenderLayer>
        <Camera>
        <RenderPass>
        <RenderPassType>
        <Extension>
        <Version>
        """
        
    def OpenMayaCommonGlobalsCreateTab(self):        
        log.debug("OpenMayaCommonGlobalsCreateTab()")
        scLo = "scrollLayout"
        parentForm = pm.setParent(query=True)        
        pm.setUITemplate("attributeEditorTemplate", pushTemplate=True)
        if self.rendererTabUiDict.has_key('common'):
            self.rendererTabUiDict.pop('common')        
        uiDict = {}
        self.rendererTabUiDict['common'] = uiDict
        clo = "clo"
        
        with pm.frameLayout(clo, labelVisible=False, collapsable=False, mw=8, mh=5, borderVisible=False):
            with pm.columnLayout(adj=True):                        
                uiDict['imgpath'] = pm.text(label="Path:", align="left", font="smallBoldLabelFont")
                uiDict['imgname'] = pm.text(label="File Name:", align="left", font="smallBoldLabelFont")
                uiDict['imgsize'] = pm.text(label="Image Size:", align="left", font="smallBoldLabelFont")
        
        with pm.scrollLayout(scLo, horizontalScrollBarThickness=0) as sl:
            with pm.columnLayout("commonTabColumn", adjustableColumn=True, width=400) as ctc:
                with pm.frameLayout(label="File Output", collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        uiDict['fileNamePrefixField'] = pm.textFieldGrp(label="File Name Prefix:", cc=self.updateFileNamePrefix)
                        pm.connectControl(uiDict['fileNamePrefixField'], pm.SCENE.defaultRenderGlobals.imageFilePrefix, index = 2)
                        pm.attrEnumOptionMenuGrp("imageMenuMayaSW", label="Image Format:", attribute=self.renderNode.imageFormat)
                        formats = self.renderNode.imageFormat.getEnums().keys()
                        if "exr" in formats or "Exr"  in formats:
                            with pm.columnLayout("exrOptionsLayout", adjustableColumn=True, width=400) as uiDict['exrOptionsLayout']:
                                self.addRenderGlobalsUIElement(attName='exrDataTypeHalf', uiType='bool', displayName='Half Float', default='false', uiDict={})
                                self.addRenderGlobalsUIElement(attName='exrMergeChannels', uiType='bool', displayName='Merge passes', default='true', uiDict={})
                                
                        uiDict['imageNumbering'] = pm.optionMenuGrp(label="Frame/Animation ext:", changeCommand=self.setFrameNumbering)
                        for value in ["name.ext", "name.#.ext"]:
                            pm.menuItem(value)
                        if self.defaultGlobals.animation.get():
                            uiDict['imageNumbering'].setSelect(2)
                        
                        self.addRenderDefaultGlobalsUIElement(attName='extensionPadding', uiType='int', displayName='Frame Padding:', uiDict=uiDict)
                        
                with pm.frameLayout(label="Frame Range", collapsable=True, collapse=False) as frameRangeLayout:
                    uiDict['frameRangeLayout'] = frameRangeLayout
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        self.addRenderDefaultGlobalsUIElement(attName='startFrame', uiType='float', displayName='Start Frame:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='endFrame', uiType='float', displayName='End Frame:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='byFrame', uiType='float', displayName='By Frame:', uiDict=uiDict)

                with pm.frameLayout(label="Renderable Cameras", collapsable=True, collapse=False) as uiDict['camerasFrame']:
                    self.createCamerasUI(uiDict)

                with pm.frameLayout(label="Image Size", collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        uiDict['imageSizePresets'] = pm.optionMenuGrp(label="Presets:", changeCommand=self.setImageSize)
                        for imgFormat in self.imageFormatData:
                            pm.menuItem(imgFormat[0])
                        # uiDict['imageSizePresets'].setSelect(12) 

                        self.addRenderDefaultResGlobalsUIElement(attName='aspectLock', uiType='bool', displayName='Maintain aspect ratio', uiDict=uiDict)
                        self.addRenderDefaultResGlobalsUIElement(attName='width', uiType='int', displayName='Width:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "width"))
                        self.addRenderDefaultResGlobalsUIElement(attName='height', uiType='int', displayName='Height:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "height"))
                        self.addRenderDefaultResGlobalsUIElement(attName='deviceAspectRatio', uiType='float', displayName='Device Aspect:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "devAsp"))
                        # self.addRenderDefaultResGlobalsUIElement(attName='pixelAspect', uiType='float', displayName='Pixel Aspect:', uiDict=uiDict, callback=self.updateImageSize)
                        
                with pm.frameLayout(label="Render Scripts", collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        self.addRenderDefaultGlobalsUIElement(attName='preMel', uiType='string', displayName='Pre Render Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postMel', uiType='string', displayName='Post Render Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='preRenderLayerMel', uiType='string', displayName='Pre Layer Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postRenderLayerMel', uiType='string', displayName='Post Layer Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='preRenderMel', uiType='string', displayName='Pre Frame Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postRenderMel', uiType='string', displayName='Post Frame Mel:', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[  (clo, "right", 0), (clo, "left", 0), (clo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ], attachControl=[(scLo, "top", 0, clo)])
        self.OpenMayaCommonGlobalsUpdateTab()
        self.updateExrUI()
        self.updateImageSize(["width"])
        self.updateFileNamePrefix()
        pm.scriptJob(attributeChange=(self.renderNode.imageFormat, self.updateExrUI), parent=parentForm)
        defrw = pm.SCENE.defaultResolution.width
        defrh = pm.SCENE.defaultResolution.height
        defrd = pm.SCENE.defaultResolution.deviceAspectRatio
        pm.scriptJob(attributeChange=(defrw, pm.Callback(self.updateImageSize, ["width"])), runOnce=True)
        pm.scriptJob(attributeChange=(defrh, pm.Callback(self.updateImageSize, ["height"])), runOnce=True)
        pm.scriptJob(attributeChange=(defrd, pm.Callback(self.updateImageSize, ["devAsp"])), runOnce=True)
        pm.scriptJob(event=("SelectionChanged", self.selectionChangedEvent))

    def updateFrameSettings(self):
        log.debug("OpenMayaCommonGlobalsUpdateTab()")
        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        
        if self.defaultGlobals.animation.get():
            uiDict['extensionPadding'].setEnable(True)
            uiDict['frameRangeLayout'].setEnable(True)
        else:
            uiDict['extensionPadding'].setEnable(False)
            uiDict['frameRangeLayout'].setEnable(False)
    
    def OpenMayaCommonGlobalsUpdateTab(self):
        log.debug("OpenMayaCommonGlobalsUpdateTab()")
        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        
        self.updateFrameSettings()
        self.updateCamerasUI()
