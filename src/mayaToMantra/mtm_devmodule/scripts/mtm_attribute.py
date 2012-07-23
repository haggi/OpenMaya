import maya.OpenMaya as om
import pymel.core as pm
from pymel.core import Callback
import logging

MAPTO_IGNORE = 'ignore'
MAPTO_NONE = 'none'
kColor = 999
kEnum = 1000
kMessage=10000

MAP_API_ATTRIDS = {
                kEnum : "kEnum",
                kColor : "MFnNumericData::k3Float",
                kMessage: "kMessage",
                om.MFnNumericData.k2Int:"MFnNumericData::k2Int",
                om.MFnNumericData.k2Float:"MFnNumericData::k2Float",
                om.MFnNumericData.kFloat:"MFnNumericData::kFloat",
                om.MFnNumericData.kBoolean:"MFnNumericData::kBoolean",
                om.MFnNumericData.kInt:"MFnNumericData::kInt",
                om.MFnData.kString:"MFnNumericData::kString",                
                }

MAP_API_ATTR = {
                kEnum : ['at','enum'],
                kColor: ['at','spectrum'],
                kMessage: ['at','message'],
                om.MFnNumericData.k2Int: ['dt','long2'],
                om.MFnNumericData.k2Float: ['dt','float2'],
                om.MFnNumericData.kFloat:['at','float'],
                om.MFnNumericData.kBoolean:['at', 'bool'],
                om.MFnNumericData.kInt: ['at', 'int'],
                om.MFnData.kString:['dt', 'string']                
                }


log = logging.getLogger("mantraLogger")

class Attribute(object):
    def __init__(self, an="", dn="", tp=None, mapto=MAPTO_NONE, values=None, default=None, affectAtt = None, kat=None):
        self.attname = an
        self.shortname = self.attname
        self.displayname = dn
        self.type = tp
        self.mapto = mapto
        self.values = values
        self.default = default
        self.affectAtt = affectAtt
        self.child = None
        self.next = None
        self.kategory = kat
        self.uiElement = None
        self.uiDimFunction = None

def dimConnections(att):
    log.debug("dimConnections for att %s" % att.attname)
    
            
class AttributeCollection(object):
    """Creates a list of attributes.
    The class can return a flattened list of string/attribute assignments, 
    as well as create ui templates with dim ctrls.
    """    
    def __init__(self):
        self.attList = []
        
    def addAttr(self, an="", dn="", tp=None, mapto=MAPTO_NONE, values=None, default=None, affectAtt = None, kat=None):
        """kategory should be one or more strings separated with a | sign"""
        self.attList.append(Attribute(an=an, dn=dn, tp=tp, mapto=mapto, values=values, default=default, affectAtt = affectAtt, kat=kat))
        
    def listAttr(self):
        pass

    def endLayout(self):
        pm.editorTemplate(endLayout=True)         

    def beginLayout(self, label):
        pm.editorTemplate(collapse=False, beginLayout=label)          
    
    def addControl(self, attname, displayname):
        pm.editorTemplate(attname, addDynamicControl=True, label=displayname)   

    def beginUISegment(self, segmentName):
        log.debug("beginUISegment: %s" % segmentName)
        frameName = segmentName + "Frame"
        columnName = segmentName + "Column"
        pm.frameLayout(frameName, label = segmentName, collapsable = True, collapse = False, width = 414)
        pm.columnLayout(columnName, adj = True, columnAttach=('both', 5), columnWidth=250)
    
    def dimConnections(self, attrib):
        log.debug("dimConnections self for att %s" % attrib.attname)
        return
        value = attrib.uiDimFunction(attrib.uiElement, query = True, value1 = True)
        for att in self.attList:
            if att == attrib:
                dimConnections = att.affectAtt
                if dimConnections:
                    log.debug("Dimming elements %s" % str(dimConnections))
                    dimList = []
                    for dimatt in self.attList:
                        if dimatt.attname in dimConnections:
                            dimList.append(dimatt)
                    for di in dimList:
                        di.uiDimFunction(di.uiElement, edit=True, enable=value)
                            
    def createUi(self, node = None):
        log.debug("createUi for node %s" % str(node))
        currentSegment = ""
        layoutList = []        
        for att in self.attList:            
            if att.kategory!=None and currentSegment != att.kategory:
                log.debug("kategory %s != %s" % (currentSegment, att.kategory ))
                katlist = att.kategory.split("|")
                diff = len(layoutList) - len(katlist)
                # neue liste ist kuerzer als alte, also solange dicht machen bis die laenge gleich ist
                log.debug("check diff %d" % diff)                
                while diff > 0:
                    layoutList.pop()
                    pm.setParent("..")
                    pm.setParent("..")
                    diff = len(layoutList) - len(katlist)
                    log.debug("new diff %d" % diff)

                # alte liste ist nun nicht laenger als neue liste
                # durchgehen und alles dichtmachen was nicht gleich ist
                for i in range(len(layoutList)):
                    kat = katlist[i]
                    ckat = layoutList[i]
                    # wenn werte ungelich dann alles was noch kommt zumachen
                    if kat != ckat:
                        laylen = len(layoutList)
                        for n in range(i, laylen):
                            pm.setParent("..")
                            pm.setParent("..")
                            layoutList.pop(n)
                
                # nun sollte sichergestellt sein, dass layoutList nur noch elemente
                # enthaelt, die gleich sind, also kann man daran anknuepfen
                for i in range(len(layoutList), len(katlist)):
                    log.debug("opening layout for katlist %s %d %s" % (str(katlist), i, katlist[i]))
                    self.beginUISegment(katlist[i])          
                    layoutList.append(katlist[i])
                
                currentSegment = att.kategory                  
                                
            if MAP_API_ATTR.has_key(att.type):
                log.debug("Adding element %s with displayName %s" % (att.attname,att.displayname))
                attype, attypeval = MAP_API_ATTR[att.type]
                log.debug("Adding attribute named %s type %s val %s default %s" % (att.attname, attype, attypeval, att.default))
                if attypeval == 'bool':
                    att.uiElement = pm.checkBoxGrp( att.attname,  numberOfCheckBoxes = 1, label1 = att.displayname, cc = Callback(self.dimConnections,att))
                    att.uiDimFunction = pm.checkBoxGrp
                    if node:
                        pm.connectControl(att.uiElement, node + "." + att.attname, index = 2 )
                if attypeval == 'int':
                    att.uiElement = pm.intFieldGrp( att.attname, numberOfFields=1, label=att.displayname, value1=att.default, cc = Callback(self.dimConnections,att))
                    att.uiDimFunction = pm.intFieldGrp
                    if node:
                        pm.connectControl(att.uiElement, node + "." + att.attname, index = 2 )
                if attypeval == 'long2':
                    if node:
                        att.uiDimFunction = pm.attrFieldGrp( label = att.displayname, attribute='%s' % (node + "." + att.attname), cc = Callback(self.dimConnections,att))
                if attypeval == 'float':
                    att.uiElement = pm.floatFieldGrp( att.attname, numberOfFields=1, label=att.displayname, value1=att.default, cc = Callback(self.dimConnections,att))
                    att.uiDimFunction = pm.floatFieldGrp
                    if node:
                        pm.connectControl(att.uiElement, node + "." + att.attname, index = 2 )
                if attypeval == 'float2':
                    if node:
                        att.uiDimFunction = pm.attrFieldGrp( attribute='%s' % (node + "." + att.attname), cc = Callback(self.dimConnections,att))
                if attypeval == 'string':
                    att.uiElement = pm.textFieldGrp( att.attname, label=att.displayname, text=att.default, cc = Callback(self.dimConnections,att))
                    pm.connectControl(att.uiElement, node + "." + att.attname, index = 2 )
                    pm.textFieldGrp(att.uiElement, edit=True, text = att.default)

                if attypeval == 'enum':
                    poplist = map(list, enumerate(att.values))
                    
                    if node:
                        nodeatt = node + "." + att.attname
                        att.uiElement = pm.attrEnumOptionMenuGrp(att.attname, label = att.displayname, at=nodeatt, ei = poplist, vcc = Callback(self.dimConnections,att))            
                        att.uiDimFunction = pm.attrEnumOptionMenuGrp
                    else:
                        att.uiElement = pm.attrEnumOptionMenuGrp(att.attname, label = att.displayname, ei = poplist, vcc = Callback(self.dimConnections,att))            
                        att.uiDimFunction = pm.attrEnumOptionMenuGrp
                    
                if attypeval == 'message':
                    pass
                
        if len(layoutList) > 0:
            for i in range(len(layoutList)):
                pm.setParent("..")
                pm.setParent("..")
    
    def createUiTemplates(self):
        log.debug("crateUiTemplates")
        currentSegment = ""
        layoutList = []        
        for att in self.attList:            
            if att.kategory!=None and currentSegment != att.kategory:
                log.debug("kategory %s != %s" % (currentSegment, att.kategory ))
                katlist = att.kategory.split("|")
                diff = len(layoutList) - len(katlist)
                # neue liste ist kuerzer als alte, also solange dicht machen bis die laenge gleich ist
                log.debug("check diff %d" % diff)                
                while diff > 0:
                    layoutList.pop()
                    self.endLayout()
                    diff = len(layoutList) - len(katlist)
                    log.debug("new diff %d" % diff)

                # alte liste ist nun nicht laenger als neue liste
                # durchgehen und alles dichtmachen was nicht gleich ist
                for i in range(len(layoutList)):
                    kat = katlist[i]
                    ckat = layoutList[i]
                    # wenn werte ungelich dann alles was noch kommt zumachen
                    if kat != ckat:
                        laylen = len(layoutList)
                        for n in range(i, laylen):
                            self.endLayout()
                            layoutList.pop(n)
                
                # nun sollte sichergestellt sein, dass layoutList nur noch elemente
                # enthaelt, die gleich sind, also kann man daran anknuepfen
                for i in range(len(layoutList), len(katlist)):
                    log.debug("opening layout for katlist %s %d %s" % (str(katlist), i, katlist[i]))
                    self.beginLayout(katlist[i])          
                    layoutList.append(katlist[i])
                
                currentSegment = att.kategory                  
                
            log.debug("Adding element %s with displayName %s" % (att.attname,att.displayname))                
            
            self.addControl(att.attname, att.displayname)
                
        if len(layoutList) > 0:
            for i in range(len(layoutList)):
                self.endLayout()
