'''
First implementation of an attribute window.
You simply select an object or hierarchy, call the interface and selct the attributes you need.
Then you can add or remove attributes.
Maybe this will change in future versions in a way to get more control.  
'''
import pymel.core as pm
import pymel.core.uitypes as pui

mantra_node_attributes = {'mesh': ['Render As Subdiv'],
                          'shader': ['Reflection Options','Refraction Options'],
                          'light':['Sampling Quality'],
                          'geo' : ['Raytrace Options']
                          }

class GUI(pm.ui.Window):
    _singleton = True
    def __new__(cls, *args):
        return super(GUI, cls).__new__(cls)

    def __init__(self):
        #pm.scriptJob(event = ["SelectionChanged", pm.Callback(self.updateObjectSelection)], parent=self)
        self.title = "MantraAttributeManager"
        pm.setUITemplate("DefaultTemplate", pushTemplate=True)
        self.height = 500
        #with pm.paneLayout( configuration='vertical2') as pane:
        with pm.frameLayout(collapsable = False, labelVisible = False) as frame:
            self.objectTree = pm.treeView(abr=False, height = 150)
            self.fillObjectTree()
            pm.button(label = "Assign Selected Attributes", c = pm.Callback(self.setAttributes, "assignSelected"))            
            pm.button(label = "Remove Selected Attributes", c = pm.Callback(self.setAttributes, "removeSelected"))        
            pm.button(label = "Remove All Attributes from Scene", c = pm.Callback(self.setAttributes, "removeAllAttr"))           
        self.show()

    def fillObjectTree(self):
        pm.treeView(self.objectTree, edit=True, removeAll = True)
        selection = pm.ls(sl=True)

        for key in mantra_node_attributes.keys():
            pm.treeView(self.objectTree, edit=True,  addItem = (key, None))
            pm.treeView(self.objectTree, edit=True,  expandItem = (key, False))
            for entry in mantra_node_attributes[key]:
                pm.treeView(self.objectTree, edit=True,  addItem = (entry, key))
                
    def setAttributes(self, cmd):
        print "AttibuteManager:setAttributes command", cmd
        
        if cmd == "assignSelected":
            attributeSelected = pm.treeView(self.objectTree, query=True, si = True)
            if not attributeSelected or len(attributeSelected) == 0:
                print "No attr selected"
                return
            
            # It is possible to select the parent Menu entry or the child. 
            # I need the parent entry to decide if is a mesh, light or other type
            # and the attribute itself
            print "Selected attributes:", attributeSelected
            leafAttributes = []
            typeList = []
            for att in attributeSelected:
                if not pm.treeView(self.objectTree, query=True, il = att):
                    children =  pm.treeView(self.objectTree, query=True, ch = att)
                    if children and len(children) > 1:
                        #print "Children of ", att, children
                        leafAttributes.extend(children[1:])
                else:
                    #print "Att", att, "is leaf"
                    leafAttributes.append(att)
            leafAttributes = list(set(leafAttributes))
            print "LeafAttributes", leafAttributes
            for leaf in leafAttributes:
                parent =  pm.treeView(self.objectTree, query=True, ip = leaf)
                typeList.append(parent)
            
            selection = pm.ls(sl=True)
            relatives = pm.listRelatives(ad=True) # give me all children
            print "selection", relatives
            for element in relatives:
                print "Check", element, element.type()
                eType = element.type()
                if eType == 'mesh':
                    for att in leafAttributes:
                        if att in mantra_node_attributes['mesh']:
                            print "Adding attribute to mesh", element
                            if att == 'Render As Subdiv':
                                if not element.hasAttr('mtm_renderAsSubdiv'):
                                    element.addAttr('mtm_renderAsSubdiv', at='bool')
                if 'Light' in eType:
                    for att in leafAttributes:
                        if att in mantra_node_attributes['light']:
                            print "Adding attribute to light", element
                            if att == 'Sampling Quality':
                                if not element.hasAttr('mtm_samplingQuality'):
                                    element.addAttr('mtm_samplingQuality', at='float')
            for element in selection:
                if element.type() in ['lambert', 'phong', 'phongE', 'blinn']:
                    for att in leafAttributes:
                        if att in mantra_node_attributes['shader']:
                            print "Adding attribute to shader", element
                            if att == 'Reflection Options':
                                if not element.hasAttr('mtm_reflBlur'):
                                    element.addAttr('mtm_reflBlur', at='float')
                                if not element.hasAttr('mtm_reflSamples'):
                                    element.addAttr('mtm_reflSamples', at='long')
                            if att == 'Refraction Options':
                                if not element.hasAttr('mtm_refrBlur'):
                                    element.addAttr('mtm_refrBlur', at='float')
                                if not element.hasAttr('mtm_refrSamples'):
                                    element.addAttr('mtm_refrSamples', at='long')
                    
                
            
            pm.select(selection)
            
        if cmd == "removeSelected":
            selection = pm.ls(sl=True)
            relatives = pm.listRelatives(ad=True) # give me all children
            if relatives:
                for a in relatives:
                    attrs = pm.listAttr(a, ud=True, leaf=False, sn=False, string="mtm_*")
                    for att in attrs:
                        pm.deleteAttr(a, at=att)
            for a in selection:
                attrs = pm.listAttr(a, ud=True, leaf=False, sn=False, string="mtm_*")
                for att in attrs:
                    pm.deleteAttr(a, at=att)
                    
            
        if cmd == "removeAllAttr":
            all = pm.ls()
            attrList = []
            for a in all:
                attrs = pm.listAttr(a, ud=True, leaf=False, sn=False, string="mtm_*")
                for att in attrs:
                    pm.deleteAttr(a, at=att)
                            
            
        
#    def updateObjectSelection(self):
#        print "GUI: updateObjectSelection"
#        self.fillObjectTree() 
        
        
#class AMTextScrollList(pui.TextScrollList):
#    def __init__(self, **args):
#        super(AMTextScrollList, self).__init__()
#        self.selectCommand(self.itemSelected)
#        
#    def itemSelected(self):
#        print "Item selected"
#
#class AMObjScrollList(pui.TextScrollList):
#    def __init__(self, **args):
#        super(AMObjScrollList, self).__init__()
#        self.selectCommand(self.itemSelected)
#        self.supportedItems = ['mesh', 'nurbsSurface', 'nurbsCurve', 'pointLight', 'areaLight', 'directionalLight', 'ambientLight']
#        self.setAllowMultiSelection(True)
#        
#        self.objDict = {}
#        for obj in pm.ls(type = self.supportedItems):
#            if obj.intermediateObject.get():
#                continue
#            self.objDict[str(obj)] = obj
#            self.append(obj)
#        self.callback = None
#        
#    def itemSelected(self):
#        print "Item selected"
#        print self.getSelectItem()
#        print self.objDict[self.getSelectItem()[0]]
#        selectList = []
#        for obj in self.getSelectItem():
#            selectList.append(self.objDict[str(obj)])
#        if self.callback:
#            self.callback(selectList)
#            
#class AMAttrScrollList(pui.TextScrollList):
#    def __init__(self, **args):
#        super(AMAttrScrollList, self).__init__()
#        self.selectCommand(self.itemSelected)
#        self.attrList = {}
#        self.setAllowMultiSelection(True)
#        self.objList = []
#        self.currentAttrList = []
#        self.selectList = []
#        self.sortedList = None
#        
#    def updateList(self, list):
#        print "attribute list got", list
#        if not self.sortedList:
#            self.sortedList = {}
#            for key in self.attrList.keys():
#                for element in self.attrList[key]:
#                    self.sortedList[element] = key
#        self.objList = list
#        self.currentAttrList = []
#        self.selectList = []
#        self.removeAll()
#        
#        for obj in self.objList:
#            attrName = "mantra_" + pm.nodeType(obj)
#            if "Light" in pm.nodeType(obj):
#                attrName = "mantra_light"
#                for item in self.attrList['light']:
#                    if item not in self.currentAttrList:
#                        self.currentAttrList.append(item)
#                        self.append(item)        
#                        if obj.hasAttr(attrName):
#                            self.selectList.append(item)
#            else:
#                for item in self.attrList[pm.nodeType(obj)]:
#                    if item not in self.currentAttrList:
#                        self.currentAttrList.append(item)
#                        self.append(item)        
#                        if obj.hasAttr(attrName):
#                            self.selectList.append(item)
#        print "Selectlist", self.selectList
#        self.setSelectItem(self.selectList)
#
#    def addMantraAttr(self, obj, type, subKategory):
#        print "Adding mantra attributes for", obj, "type", type, "subkat", subKategory
#        #mantra_renderAsSubdiv
#        if type == "mantra_mesh":
#            if not obj.hasAttr("mantra_renderAsSubdiv"):
#                obj.addAttr("mantra_renderAsSubdiv", at = "bool")
#            
#        if type == "mantra_light":
#            subDict = mantra_light_attributes[subKategory]
#            print "SubDict", subDict, "from subkat", subKategory, mantra_light_attributes[subKategory]
#            for key in subDict.keys():
#                #solo=1:triplet=3:quintet=5
#                print "adding", key, "type", subDict[key]
#                type = subDict[key]
#                if type == 'bool':
#                    obj.addAttr(key, at = 'bool')
#                if type == 'float':
#                    obj.addAttr(key, at = 'float')
#                if type == 'string':
#                    obj.addAttr(key, dt = 'string')
#                if type == 'color':
#                    obj.addAttr(key, usedAsColor=True, attributeType='float3')
#                    obj.addAttr( key + "R", at='float', parent=key )
#                    obj.addAttr( key + "G", at='float', parent=key )
#                    obj.addAttr( key + "B", at='float', parent=key )
#                    
#                if type == 'enum':
#                    if subKategory == "GiLight":
#                        enumString = []
#                        for id, name in enumerate(mantra_gilight_type):
#                            enumString.append("%s:%d" % (name, id))
#                        enumString = ":".join(enumString)
#                        obj.addAttr(key, at = 'enum', enumName = enumString )
#                        
#    def removeMantraAttr(self, obj, type, subKategory):
#        print "Removing mantra attributes for", obj, "type", type
#        #mantra_renderAsSubdiv
#        if type == "mantra_mesh":
#            pm.deleteAttr(obj + ".mantra_renderAsSubdiv")
#    
#    def itemSelected(self):
#        print "Item selected"
#        selectedAttr = self.getSelectItem()
#        for attr in selectedAttr:
#            #print "Selected Attribute", selectedAttr, "gehoert zu", self.sortedList[attr]
#            kategory = self.sortedList[attr]
#            for obj in self.objList:
#                attrName = "mantra_" + kategory
#                subKategory = attr
#                if obj.hasAttr(attrName):
#                    print "Object hat attribute, deleting"
#                    pm.deleteAttr(obj + "." + attrName)
#                    self.deselectItem(selectedAttr)
#                    self.removeMantraAttr(obj, attrName, subKategory)
#                else:
#                    print "Object hat kein attribute", attrName, "creating"
#                    obj.addAttr(attrName, at = "bool")
#                    self.addMantraAttr(obj, attrName, subKategory)


                    
          
#def attributeManager():
#    print "Building mantra attribute manager"
#
#    attrList = {'mesh' : ['Subdiv'],
#                'light': ['SurfaceShaders', 'GiLight'],
#                'nurbsSurface': []}
#                
#    windowTitle = "Mantra Attribute Editor"
#    if pm.window(windowTitle, query=True, exists=True):
#        pm.window.deleteUI(windowTitle)
#         
#    amWindow = pm.window(title=windowTitle)
#    amLayout = pm.paneLayout( configuration='vertical2', parent = amWindow )
#    objScrollList = AMObjScrollList(parent = amLayout)
#    attrScrollList = AMAttrScrollList(parent = amLayout)
#    attrScrollList.attrList = attrList
#    objScrollList.callback = attrScrollList.updateList
#    
#    #btn.setCommand(buttonPressed)
#    amWindow.show()
