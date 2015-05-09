import pymel.core as pm
import logging
import os

log = logging.getLogger("mtcoLogger")

class AboutWindow(pm.ui.Window):
    
    def __init__(self):
        self.title = "About"
        self.rtf = True        
        self.initUI()
    
    def initUI(self):
        pm.setUITemplate("DefaultTemplate", pushTemplate=True)
        form = pm.formLayout()
        versionData = pm.mayatoCorona(version=True)
        license = versionData[7].split("\r\n")
        with pm.columnLayout('AboutLayout') as StandinLayout:
            with pm.rowColumnLayout('AboutColumnLayout', nc=2, columnWidth=((1, 160), (2, 220)), co=(2, "left", 10)):
                pm.text(label="mayaToCorona:", align="right")
                pm.text(label=versionData[0], align="left")
                pm.text(label="Corna version:", align="right")
                pm.text(label=versionData[1], align="left")
                pm.text(label="OIIO:", align="right")
                pm.text(label=versionData[2], align="left")
                pm.text(label="OSL:", align="right")
                pm.text(label=versionData[3], align="left")
                pm.text(label="BOOST:", align="right")
                pm.text(label=versionData[4], align="left")
                pm.text(label="OpenEXR:", align="right")
                pm.text(label=versionData[5], align="left")
                pm.text(label="License type:", align="right")
                pm.text(label=license[0], align="left")
                pm.text(label="Valid until:", align="right")
                pm.text(label=license[1].replace("Activated until: ", ""), align="left")
            pm.separator()
            with pm.columnLayout():
                pm.text(label="Corona Renderer\n(c) Render Legion s.r.o., 2014-2015", align="center")
                #pm.textField(text="Corona Renderer (c) Render Legion s.r.o., 2014-2015") 
        pm.formLayout(form, edit=True, attachForm=[(StandinLayout, 'top', 5), (StandinLayout, 'bottom', 5), (StandinLayout, 'right', 5), (StandinLayout, 'left', 5)])
        pm.setUITemplate("DefaultTemplate", popTemplate=True)
    
class CoronaMenu(pm.ui.Menu):
    """Custom Corona menu"""
    def __new__(cls, *args, **kwargs):
        name = "CoronaMenu"
        parent = kwargs.pop("parent", "MayaWindow")
        if pm.menu(name, query=True, exists=True):
            pm.deleteUI(name)
        return super(CoronaMenu, cls).__new__(cls, name, label="Corona", parent=parent, familyImage="corona", tearOff=True, allowOptionBoxes=True)

    def aboutCorona(self, *args):
        AboutWindow().show()
        
    def __init__(self):
        pm.setParent(self, menu=True)
        pm.menuItem(label="About Corona", annotation="about", command=self.aboutCorona, stp="python")

        