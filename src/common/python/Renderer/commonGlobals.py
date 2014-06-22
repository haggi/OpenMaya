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
        imageFormats = pm.getMelGlobal("string[]", "$gImageFormatData")
        if len(imageFormats) == 0:
            pm.mel.eval("source imageFormats") #make sure we have the $gImageFormatData global variable
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

    def setImageSize(self, preset):
        for imgFormat in self.imageFormatData:
            if imgFormat[0] == preset:
                self.defaultResolution.aspectLock.set(0)
                self.defaultResolution.width.set(imgFormat[1])
                self.defaultResolution.height.set(imgFormat[2])
                self.defaultResolution.deviceAspectRatio.set(float(imgFormat[1])/float(imgFormat[2]))
    
    def updateImageSize(self, *args):
        if not self.rendererTabUiDict.has_key('common'):
            return
        uiDict = self.rendererTabUiDict['common']
        
        w = self.defaultResolution.width.get()
        h = self.defaultResolution.height.get()
        dar = self.defaultResolution.deviceAspectRatio.get()
        
        if args[0] == "width":
            if self.defaultResolution.aspectLock.get():
                h = w/dar
        if args[0] == "height":
            if self.defaultResolution.aspectLock.get():
                w = h * dar
        if args[0] == "devAsp":
            h = w/dar
        darNew = w/h
        self.defaultResolution.deviceAspectRatio.set(darNew)
        self.defaultResolution.width.set(w)
        self.defaultResolution.height.set(h)
        #uiDict['width'].setValue1(w) 
        #uiDict['height'].setValue1(h)

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
            print "Scroll layout", sl
            with pm.columnLayout("commonTabColumn", adjustableColumn=True, width=400) as ctc:
                print "common", ctc
                with pm.frameLayout(label="File Output", collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        pm.textFieldGrp(label="File Name Prefix:")
                        
                        pm.attrEnumOptionMenuGrp("imageMenuMayaSW", label="Image Format:", attribute=self.renderNode.imageFormat)                            
                        
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

                with pm.frameLayout(label="Renderable Cameras",collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        for cam in pm.ls(type="camera"):
                            pm.checkBoxGrp(label=cam.name(), value1=cam.renderable.get(), cc=pm.Callback(self.switchCamRenderable, cam))

                with pm.frameLayout(label="Image Size",collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        uiDict['imageSizePresets'] = pm.optionMenuGrp(label="Presets:", changeCommand=self.setImageSize)
                        for imgFormat in self.imageFormatData:
                            pm.menuItem(imgFormat[0])
                        uiDict['imageSizePresets'].setSelect(12) 

                        self.addRenderDefaultResGlobalsUIElement(attName='aspectLock', uiType='bool', displayName='Maintain aspect ratio', uiDict=uiDict)
                        self.addRenderDefaultResGlobalsUIElement(attName='width', uiType='int', displayName='Width:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "width"))
                        self.addRenderDefaultResGlobalsUIElement(attName='height', uiType='int', displayName='Height:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "height"))
                        self.addRenderDefaultResGlobalsUIElement(attName='deviceAspectRatio', uiType='float', displayName='Device Aspect:', uiDict=uiDict, callback=pm.Callback(self.updateImageSize, "devAsp"))
                        #self.addRenderDefaultResGlobalsUIElement(attName='pixelAspect', uiType='float', displayName='Pixel Aspect:', uiDict=uiDict, callback=self.updateImageSize)
                        
                with pm.frameLayout(label="Render Scripts",collapsable=True, collapse=False):
                    with pm.columnLayout(adjustableColumn=True, width=400):
                        self.addRenderDefaultGlobalsUIElement(attName='preMel', uiType='string', displayName='Pre Render Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postMel', uiType='string', displayName='Post Render Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='preRenderLayerMel', uiType='string', displayName='Pre Layer Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postRenderLayerMel', uiType='string', displayName='Post Layer Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='preRenderMel', uiType='string', displayName='Pre Frame Mel:', uiDict=uiDict)
                        self.addRenderDefaultGlobalsUIElement(attName='postRenderMel', uiType='string', displayName='Post Frame Mel:', uiDict=uiDict)

        pm.setUITemplate("attributeEditorTemplate", popTemplate=True)
        pm.formLayout(parentForm, edit=True, attachForm=[  (clo, "right", 0), (clo, "left", 0), (clo, "top", 0), (scLo, "bottom", 0), (scLo, "left", 0), (scLo, "right", 0) ], attachControl = [(scLo, "top", 0, clo)])
        self.setImageSize("HD_540") # set default
        self.OpenMayaCommonGlobalsUpdateTab()
        

    
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
