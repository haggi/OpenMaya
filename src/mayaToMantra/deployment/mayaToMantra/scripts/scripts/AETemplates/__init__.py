import pymel.core as pm
import mtm_mantraAttributes as mantraAttributes

print "mayaToMantra AETemplates are loading..."

class AEmtm_envlightTemplate(pm.ui.AETemplate):
    def __init__(self, nodeName):
        self.nodeNameAttr = nodeName
        self.attList = mantraAttributes.mantraGlobalsATList
                
        self.beginScrollLayout()
        #self.beginLayout("Mantra EnvionmentLight", collapse=False)
        
        
        self.buildBody()
        pm.mel.AEmentalrayBaseTemplate(nodeName)
        #self.endLayout()
        self.endScrollLayout()

        
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
    
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

    def buildBody(self):
        
        self.beginLayout("Common Settings", collapse=False)

        self.addControl("lightColor", "Light Color")        
        self.addControl("intensity", "Intensity")        
        #self.addControl("shadowColor", "Shadow Color")        
        self.addControl("envtype", "Environment Type")        
        #self.addControl("areafullsphere", "Full Sphere")
        self.addControl("envintensity", "Env Intensity")
        self.addControl("envtint", "Env Tint")
        #self.addControl("raybackground", "Raytrace background")
        self.addControl("samplingquality", "Sampling Quality")
        self.addControl("castshadow", "Cast Shadows")
        self.addControl("shadowI", "Shadow Intensity")
        
        self.endLayout()
        
        self.beginLayout("Ambient Occlusion", collapse=False)
                
        #self.addControl("doraysamples", "Do Raysamples")
        self.addControl("cone_angle", "Cone Angle")
        self.addControl("doadaptive", "Use Adaptive")
        self.addControl("domaxdist", "Use Max Distance")
        self.addControl("maxdist", "Max Distance")
        
        self.endLayout()
        self.beginLayout("Maps", collapse=False)
        self.addControl("envmap", "Environment Map")
        #self.addControl("areamap", "Area Map")
        
        self.endLayout()
        
  
class AEmayaToMantraGlobalsTemplate(pm.ui.AETemplate):
    def __init__(self, nodeName):
        self.nodeNameAttr = nodeName
        self.attList = mantraAttributes.mantraGlobalsATList
                
        self.beginScrollLayout()
        self.beginLayout("MantraGlobals", collapse=False)
        
        self.buildBody()
        pm.mel.AEmentalrayBaseTemplate(nodeName)
        self.endScrollLayout()
#    static    MObject samples;
#    static    MObject samplelock;

        self.endLayout()
        
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
    
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)

    def buildBody(self):
        self.attList.createUiTemplates()
    
#class AEmeshTemplate(pm.ui.AETemplate):
#    def __init__(self, nodeName):
#        self.nodeNameAttr = nodeName
#        self.currentAtt = None
#        pm.editorTemplate(supress="renderInfo")
#        pm.setUITemplate("attributeEditorTemplate", pst = True)
#        node = pm.PyNode(nodeName)
#        self.beginLayout("TestMeshLayout", collapse=False)
#        self.addControl("mtm_renderAsSubdiv", "Render as Subdiv")    
#        self.endLayout()
#        pm.setUITemplate("attributeEditorTemplate", ppt = True)
#        
#        pm.mel.AEdependNodeTemplate(nodeName)
#  
#        
#    def addControl(self, control, label=None, **kwargs):
#        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
#    
#    def beginLayout(self, name, collapse=True):
#        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
#
##class AEtransfromTemplate(pm.ui.AETemplate):
##    def __init__(self, nodeName):
##        self.nodeNameAttr = nodeName
##        self.currentAtt = None
##        pm.setUITemplate("attributeEditorTemplate", pst = True)
##        #self.beginScrollLayout()
##        
##        #self.buildBody(nodeName)
##        #self.endScrollLayout()
##        node = pm.PyNode(nodeName)
##        if node.type() == "transform":
##            self.beginLayout("TransformLayout", collapse=False)
##            self.endLayout()
##        if node.type() == "mesh":
##            self.beginLayout("TestMeshLayout", collapse=False)
##            self.endLayout()
##        pm.setUITemplate("attributeEditorTemplate", ppt = True)
##        
##        
##    def addControl(self, control, label=None, **kwargs):
##        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
##    
##    def beginLayout(self, name, collapse=True):
##        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
##
##class AEdagNodeTemplate(pm.ui.AETemplate):
##    def __init__(self, nodeName):
##        self.nodeNameAttr = nodeName
##        self.currentAtt = None
##        pm.setUITemplate("attributeEditorTemplate", pst = True)
##        #self.beginScrollLayout()
##        
##        #self.buildBody(nodeName)
##        #self.endScrollLayout()
##        node = pm.PyNode(nodeName)
##        if node.type() == "transform":
##            self.beginLayout("TransformLayout", collapse=False)
##            self.endLayout()
##        if node.type() == "mesh":
##            self.beginLayout("TestMeshLayout", collapse=False)
##            self.endLayout()
##        pm.setUITemplate("attributeEditorTemplate", ppt = True)
##        
##        
##    def addControl(self, control, label=None, **kwargs):
##        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
##    
##    def beginLayout(self, name, collapse=True):
##        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
##        
#class AEmeshTemplate(pm.ui.AETemplate):
#    def __init__(self, nodeName):
#        pm.mel.AEmeshTemplate(nodeName)
#        pm.mel.AEdependNodeTemplate(nodeName)
#        pm.setUITemplate("attributeEditorTemplate", pst = True)
#        #self._nodeType = "mesh"
#        #self.beginScrollLayout()
#        
#        #self.buildBody(nodeName)
#        #self.endScrollLayout()
#        node = pm.PyNode(nodeName)
##        if node.type() == "transform":
##            self.beginLayout("TransformLayout", collapse=False)
##            self.endLayout()
#        self.beginLayout("TestMeshLayout", collapse=False)
#        self.endLayout()
#        pm.setUITemplate("attributeEditorTemplate", ppt = True)
#        
#    def addControl(self, control, label=None, **kwargs):
#        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
#    
#    def beginLayout(self, name, collapse=True):
#        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


class AEdependNodeTemplate(pm.ui.AETemplate):
    def __init__(self, nodeName):
        node = pm.PyNode(nodeName)
        if node.type() == 'lambert':
            pm.editorTemplate(beginLayout="Mantra Raytrace Options")
            self.addControl("mtm_refrBlur", "Refraction Blur")
            self.addControl("mtm_refrSamples", "Refraction Samples")
            pm.editorTemplate(endLayout=True)
        if node.type() in ['phong', 'phongE', 'blinn']:
            pm.editorTemplate(beginLayout="Mantra Raytrace Options")
            self.addControl("mtm_refrBlur", "Refraction Blur")
            self.addControl("mtm_refrSamples", "Refraction Samples")
            self.addControl("mtm_reflBlur", "Reflection Blur")
            self.addControl("mtm_reflSamples", "Reflection Samples")
            pm.editorTemplate(endLayout=True)
            
        if node.type() == 'mesh':
            pm.editorTemplate(beginLayout="Mantra Mesh Options")
            #self.addControl("mtm_renderAsSubdiv", "Render As Subdiv")
            pm.editorTemplate("mtm_renderAsSubdiv", ac=True, label="Render As Subdiv")
            pm.editorTemplate(endLayout=True)
            
        pm.editorTemplate(beginLayout=pm.mel.uiRes("m_AEdependNodeTemplate.kNodeBehavior"))
        pm.editorTemplate("caching", ac=True)
        pm.editorTemplate("nodeState", ac=True)
        pm.editorTemplate(endLayout=True)
        
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
    
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)


#class AElambertTemplate(pm.ui.AETemplate):
#    def __init__(self, nodeName):
#        pm.mel.AEswatchDisplay(nodeName)
#        self.beginScrollLayout()
#        
#        pm.mel.AElambertCommon(nodeName)
#        pm.mel.AElambertCommon2(nodeName)
#
#        self.beginLayout(pm.mel.uiRes("m_AElambertTemplate.kRaytraceOptions"), collapse=False)
#
#        pm.mel.AElambertCommonRaytraceOptions(nodeName)
#
#        self.endLayout()
#
#        self.beginLayout("Mantra Refraction Options", collapse=False)
#        self.endLayout()
#
#        # include/call base class/node attributes
#        #AElambertCommonInclude(self, nodeName)
#        pm.mel.AElambertCommonInclude(nodeName)
#        
#        self.beginLayout(pm.mel.uiRes("m_AElambertTemplate.kHardwareTexturing"), collapse=True)
#        pm.mel.AEhardwareTextureTemplate(nodeName, ("color transparency ambientColor incandescence normalCamera diffuse translucence"))
#        self.endLayout()
#
#        self.addExtraControls()
#        self.endScrollLayout()


#def enhanceShader(node):
#    pm.editorTemplate("mtm_refrBlur", ac=True, label="RefractionBlur")
#    
#def enhanceMesh(node):
#    pm.editorTemplate("mtm_renderAsSubdiv", ac=True)
#    
#def enhanceTemplate(nodeName):
#    print "Enhance", nodeName
#    node = pm.PyNode(nodeName)
#    #pa = pm.setParent()
#    #print "pa", pa
#    pm.editorTemplate(beginLayout="Mantra")
#    if node.type() == 'mesh':
#        enhanceMesh(node)
#       
#    pm.editorTemplate(endLayout=True)
#
#cmd = "global proc AEdagNodeInclude ( string $nodeName )\
#{\
#  editorTemplate -suppress \"renderInfo\";\
#  if (`objectType $nodeName` == \"transform\" ) {\
#     AEmentalrayInstanceFlags $nodeName;\
#  }\
#  string $cmd = \"import AETemplates as t;t.enhanceTemplate('\"+$nodeName+\"');\";\
#  print($cmd);\
#  python($cmd);\
#  AEdependNodeTemplate $nodeName;\
#}"
#
#pm.mel.eval(cmd)
