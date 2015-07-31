import pymel.core as pm

def getEnumList(attr):
    return [(i, v) for i,v in enumerate(attr.getEnums().keys())]

def addUIElement(uiType, attribute, uiLabel, callback, renderGlobalsNodeName):
    ui = None
    if uiType == 'bool':
        ui = pm.checkBoxGrp(label=uiLabel)
        if callback is not None:
            pm.checkBoxGrp(ui, edit=True, cc=callback)
    if uiType == 'int':
        ui = pm.intFieldGrp(label=uiLabel, numberOfFields = 1)
        if callback is not None:
            pm.intFieldGrp(ui, edit=True, cc = callback)
    if uiType == 'float':
        ui = pm.floatFieldGrp(label=uiLabel, numberOfFields = 1)
        if callback is not None:
            pm.floatFieldGrp(ui, edit=True, cc= callback)
    if uiType == 'enum':
        ui = pm.attrEnumOptionMenuGrp(label = uiLabel, at=attribute, ei = getEnumList(attribute)) 
        # attrEnumOptionGrp has no cc callback, so I create a script job
        if callback is not None:
            attribute = pm.Attribute(renderGlobalsNodeName + "." + attribute)
            pm.scriptJob(attributeChange=[attribute, callback], parent=ui)           
    if uiType == 'color':
        ui = pm.attrColorSliderGrp(label=uiLabel, at=attribute)
    if uiType == 'string':
        ui = pm.textFieldGrp(label=uiLabel)
        if callback is not None:
            pm.textFieldGrp(ui, edit=True, cc=callback)
    if uiType == 'vector':
        ui = pm.floatFieldGrp(label=uiLabel, nf=3)
        if callback is not None:
            pm.floatFieldGrp(ui, edit=True, cc=callback)
    return ui

def connectUIElement(uiElement, attribute):
    
    if attribute.type() == 'color':
        #color is automatically connnected via attrEnumOptionMenu
        return        
    if attribute.type() == 'enum':
        #enum is automatically connnected via attrEnumOptionMenu
        return        
    if attribute.type() == 'float3':
        #float3 == color is automatically connnected via attrColorSliderGrp
        return
    if attribute.type() == 'message':
        #no automatic connection necessary, will be controlled by other scritps
        return
    if attribute.type() == 'vector':
        #no automatic connection necessary, will be controlled by other scritps
        return
    pm.connectControl(uiElement, attribute, index = 2)

def addRenderGlobalsUIElement(renderGlobalsNodeName = None, attName = None, uiType = None, displayName = None, default=None, data=None, uiDict=None, callback=None, anno=None):
    
    attribute = pm.Attribute(renderGlobalsNodeName + "." + attName)
    uiElement = addUIElement(uiType, attribute, displayName, callback, renderGlobalsNodeName)
    if anno:
        uiElement.setAnnotation(anno)
    connectUIElement(uiElement, attribute)
    uiDict[attName] = uiElement
