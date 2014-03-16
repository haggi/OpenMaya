import pymel.core as pm
import path

class MayaToOptionAttrMenu(pm.ui.OptionMenu):
    def __new__(cls, *args, **kwargs):
        return super(MayaToOptionAttrMenu, cls).__new__(cls, args, kwargs)
    
    def __init__(self, *args, **kwargs):
        self.callback = None
        if kwargs.has_key('callback'):
            self.callback = kwargs['callback']
        self.changeCommand(self.changeCB)
        self.elementList = []
        if kwargs.has_key('elementList'):
            self.elementList = kwargs['elementList']
        self.attribute = kwargs['attribute']
        self.update()

    def changeCB(self, item):
        print "CAllback"
        if self.callback is not None:
            self.callback(self.v)
        else:
            self.attribute.set(item)

    def update(self):
        self.clear()
        with self:
            for element in self.elementList:
                pm.menuItem(label=element)

class MayaToFileListPopUp(MayaToOptionAttrMenu):
    def __new__(cls, *args, **kwargs):
        return super(MayaToFileListPopUp, cls).__new__(cls, *args, **kwargs)
    
    def __init__(self, *args, **kwargs):
        self.directory = kwargs['directory']
        self.attribute = kwargs['attribute']
        #self.ext = kwargs['ext']
        
        d = path.path(self.directory)
        self.elementList = []
        self.changeCommand(self.combinePathCallback)
        for f in d.listdir():
            self.elementList.append(f.basename())
        self.update()
        self.combinePathCallback(self.elementList[0])
        
    def combinePathCallback(self, value):
        finalPath = "{0}/{1}".format(self.directory, value)
        self.attribute.set(finalPath)
    
    