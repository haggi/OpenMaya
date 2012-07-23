#import pymel.core as pm
##import maya.OpenMaya as om
##import mantraRenderer.mantraAttributes as attributes

#import logging
#import mtm_mantraattributes

#log =logging.getLogger("render")

#
##def AEbuildBody(nodeName):    
##    returnStringArray = []
##    for segment in shaderParams.mantraSurfaceShaderDict:
##        returnStringArray.append("editorTemplate -beginLayout %s -collapse 0;" % (segment['segment']))
##        for element in segment['elements']:
##            log.debug("Adding element %s with displayName %s" % (element['attname'],element['displayname']))                
##            returnStringArray.append("editorTemplate -l %s -addControl %s;" % (element['displayname'], element['attname']))
##        returnStringArray.append("editorTemplate -endLayout;")        
##    return "\n".join(returnStringArray)
#
#
##class AEmantraShaderTemplate(pm.ui.AETemplate):
##    def __init__(self, nodeName):
##        self.nodeNameAttr = nodeName
##        self.currentAtt = None
##        pm.mel.AEswatchDisplay(nodeName)
##        pm.setUITemplate("attributeEditorTemplate", pst = True)
##        self.beginScrollLayout()
##        self.buildBody(nodeName)
##        self.endScrollLayout()
##        pm.setUITemplate("attributeEditorTemplate", ppt = True)
##        
##    def addControl(self, control, label=None, **kwargs):
##        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
##    
##    def beginLayout(self, name, collapse=True):
##        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
#
##class AEmantraSurfaceTemplate(AEmantraShaderTemplate):
#
#    
##global proc AEmantraSurface_fileTextureBrowser( string $cmd )
##{
##      string  $workspace = `workspace -q -fn`;
##    setWorkingDirectory $workspace "image" "sourceImages";
##    fileBrowser ($cmd, "Open", "image", 0);
##}
##
##global proc AEmantraSurface_imageFileBrowserReplace( string $fileAttribute)
##{    
##    connectControl -fileName textureNameField $fileAttribute;
##      string $command = "AEmantraSurface_assignTextureFilename "+" "+$fileAttribute;
##      button -e -c ("AEmantraSurface_fileTextureBrowser \"" + $command + "\"" ) browser;
##
##}
##
##global proc AEmantraSurface_assignTextureFilename( string $fileAttribute, string $file, string $type )
##{
##     setAttr -type "string" $fileAttribute $file;
##}
#
##    def fileBrowserCallback(self, attr, fileName):
##        print "FileName", fileName, "attr", attr
##        att = pm.Attribute(attr)
##        att.set(fileName)
##        
##    def fileBrowser(self, arg):
##        print "fileBrowser", arg
##        singleFilter = "All Files (*.*)"
##        result = pm.fileDialog2(fm=1, fileFilter=singleFilter, dialogStyle=2)
##        print "open file", result
##        self.fileBrowserCallback(arg, result[0])
##        
##    def new(self, node):
##        print "new", node
##        attr = node.split(".")[-1]
##        for att in attributes.mantraMantraSurfaceShaderATList.attList:
##            if att.attname == attr:
##                attr = att.displayname
##        self.createFileBrowser(node, attr)
##        
##    def replace(self, attr):
##        print "replace", attr
##        self.replaceFileBrowser(attr)
##        
##    def createFileBrowser(self, node, attr):
##        print "createFileBrowser", node, attr
##        pm.setUITemplate("attributeEditorTemplate", pst = True)
##        with pm.rowLayout("imageFileLayout", nc = 3) as imageFileLayout:
##            label = attr
##            pm.text(label=label)
##            pm.textField("textureFileField")
##            pm.symbolButton("browser", image = "navButtonBrowse.xpm")
##        pm.setUITemplate("attributeEditorTemplate", ppt = True)
##        self.replaceFileBrowser(node)
##        
##    def replaceFileBrowser(self, node):
##        print "replaceFileBrowser", node
##        pm.setUITemplate("attributeEditorTemplate", pst = True)
##        print "connect", "textureFileField", "with", node 
##        pm.connectControl("textureFileField", node, fileName=True)
##        pm.button("browser", edit=True, command = pm.Callback(self.fileBrowser, node))
##        
##    def buildBody(self, nodeName):
##        log.debug("AEmantraSurfaceTemplate: nodeName %s" % nodeName)
##
##        currentSegment = ""
##        layoutList = []
##        for att in attributes.mantraMantraSurfaceShaderATList.attList:
##            
##            if currentSegment != att.kategory:
##                log.debug("kategory %s != %s" % (currentSegment, att.kategory ))
##                katlist = att.kategory.split("|")
##                diff = len(layoutList) - len(katlist)
##                # neue liste ist kuerzer als alte, also solange dicht machen bis die laenge gleich ist
##                log.debug("check diff %d" % diff)                
##                while diff > 0:
##                    layoutList.pop()
##                    self.endLayout()
##                    diff = len(layoutList) - len(katlist)
##                    log.debug("new diff %d" % diff)
##
##                # alte liste ist nun nicht laenger als neue liste
##                # durchgehen und alles dichtmachen was nicht gleich ist
##                for i in range(len(layoutList)):
##                    kat = katlist[i]
##                    ckat = layoutList[i]
##                    # wenn werte ungelich dann alles was noch kommt zumachen
##                    if kat != ckat:
##                        laylen = len(layoutList)
##                        for n in range(i, laylen):
##                            self.endLayout()
##                            layoutList.pop(n)
##                
##                # nun sollte sichergestellt sein, dass layoutList nur noch elemente
##                # enthaelt, die gleich sind, also kann man daran anknuepfen
##                for i in range(len(layoutList), len(katlist)):
##                    self.beginLayout(katlist[i],collapse=0)  
##                    layoutList.append(katlist[i])
##                
##                currentSegment = att.kategory                  
##                
##            log.debug("Adding element %s with displayName %s" % (att.attname,att.displayname))                
##            if att.type == om.MFnData.kString:
##                self.callCustom(self.new, self.replace, att.attname)
##                self.currentAtt = None
##            else:
##                self.addControl(att.attname, label=att.displayname)
##                
##        if len(layoutList) > 0:
##            for i in range(len(layoutList)):
##                self.endLayout()
##                
###        self.addControl("bright", label="kBright", changeCommand=self.colorChanged)
###        self.addControl("dark", label="kDark", changeCommand=self.colorChanged)
##        self.addExtraControls()
#
#class AEdagNodeTemplate(pm.ui.AETemplate):
#    def __init__(self, nodeName):
#        self.nodeNameAttr = nodeName
#        self.currentAtt = None
#        pm.setUITemplate("attributeEditorTemplate", pst = True)
#        #self.beginScrollLayout()
#        
#        #self.buildBody(nodeName)
#        #self.endScrollLayout()
#        node = pm.PyNode(nodeName)
#        if node.type() == "transform":
#            self.beginLayout("TransformLayout", collapse=False)
#            self.endLayout()
#        if node.type() == "mesh":
#            self.beginLayout("TestMeshLayout", collapse=False)
#            self.endLayout()
#        pm.setUITemplate("attributeEditorTemplate", ppt = True)
#        
#        
#    def addControl(self, control, label=None, **kwargs):
#        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
#    
#    def beginLayout(self, name, collapse=True):
#        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
#        
#class AEdagNodeInclude(pm.ui.AETemplate):
#    def __init__(self, nodeName):
#        self.nodeNameAttr = nodeName
#        self.currentAtt = None
#        pm.setUITemplate("attributeEditorTemplate", pst = True)
#        #self.beginScrollLayout()
#        
#        #self.buildBody(nodeName)
#        #self.endScrollLayout()
#        node = pm.PyNode(nodeName)
#        if node.type() == "transform":
#            self.beginLayout("TransformLayout", collapse=False)
#            self.endLayout()
#        if node.type() == "mesh":
#            self.beginLayout("TestMeshLayout", collapse=False)
#            self.endLayout()
#        pm.setUITemplate("attributeEditorTemplate", ppt = True)
#        
#        
#    def addControl(self, control, label=None, **kwargs):
#        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
#    
#    def beginLayout(self, name, collapse=True):
#        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
#        
##global proc AEdagNodeInclude ( string $nodeName )
##{
##  // include/call base class/node attributes
##  editorTemplate -suppress "renderInfo";
##
##  // mantra instance flags
##  if (`objectType $nodeName` == "transform" ) {
##     AEmentalrayInstanceFlags $nodeName;
##     AEmantraTransform $nodeName;
##  }
##
##  if (`objectType $nodeName` == "camera" )    AEmantraCamera          $nodeName;
##  if (`objectType $nodeName` == "mesh" )    AEmantraMesh              $nodeName;
##  if (`objectType $nodeName` == "areaLight" ) AEmantraAreaLight       $nodeName;
##  if (`objectType $nodeName` == "spotLight" ||
##      `objectType $nodeName` == "directionalLight" ||
##      `objectType $nodeName` == "pointLight" ) AEmantraLightControls  $nodeName;
##
##  AEdependNodeTemplate $nodeName;
##}
