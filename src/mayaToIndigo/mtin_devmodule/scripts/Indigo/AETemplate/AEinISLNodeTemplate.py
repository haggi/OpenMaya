import os
import thread
import subprocess

import logging
import pymel.core as pm

log = logging.getLogger("ui")

def editTextThread(text, node, scrollField):
    if text is None:
        text = ""
    tmpDir = os.environ['TMP']
    tmpFileName = tmpDir + "\\islEdit.txt"
    tmpFileName = tmpFileName.replace("\\", "/")
    
    f = open(tmpFileName, "w")
    f.write(text)
    f.close()
    textEditProgram = r"C:/Program Files (x86)/Notepad++/notepad++.exe -nosession"
    cmd = '{0} {1}'.format(textEditProgram, tmpFileName)
    subprocess.call(cmd)
    
    f = open(tmpFileName, "r")
    content = f.readlines()
    f.close()
    node.islData.set("".join(content))
    scrollField.setText("".join(content))
    
class BaseTemplate(pm.ui.AETemplate):
    
    def addControl(self, control, label=None, **kwargs):
        pm.ui.AETemplate.addControl(self, control, label=label, **kwargs)
        
    def beginLayout(self, name, collapse=True):
        pm.ui.AETemplate.beginLayout(self, name, collapse=collapse)
        

class AEinISLNodeTemplate(BaseTemplate):
    def __init__(self, nodeName):
        BaseTemplate.__init__(self, nodeName)
        self.thisNode = None
        self.node = pm.PyNode(self.nodeName)
        self.textField = None
        # pm.mel.AEswatchDisplay(nodeName)
        self.beginScrollLayout()
        self.buildBody(nodeName)
        self.addExtraControls("ExtraControls")
        self.endScrollLayout()
        
    def textChangeCommand(self, args):
        text = self.textField.getText()
        self.thisNode.islData.set(text)
        
    def editTextButtonCommand(self, arg):
        text = self.textField.getText()
        editTextThread(text, self.thisNode, self.textField)
        #thread.start_new_thread( editTextThread, (text, 0) )
        
    def saveShaderCommand(self, arg):
        filename = pm.fileDialog2(fileMode=0, caption="Export Shader File")
        if filename is not None:
            if len(filename) > 0:
                filename = filename[0].replace(".*", "")
                if not filename.endswith(".isl"):
                    filename += ".isl"
                text = self.textField.getText()
                try:
                    islFile = open(filename, "w")
                    islFile.write(text)
                    islFile.close()
                except:
                    log.error("Unable to save shader to file {0}".format(filename))
                
    def loadShaderCommand(self, arg):
        filename = pm.fileDialog2(fileMode=1, caption="Load Shader File")
        if filename is not None:
            if len(filename) > 0:
                filename = filename[0]
                try:
                    islFile = open(filename, "r")
                    content = islFile.readlines()
                    islFile.close()
                    content = "".join(content)
                    self.textField.setText(content)
                except:
                    log.error("Unable to load shader from file {0}".format(filename))
    
    def editButtonNew(self, attr):
        parent = pm.setParent(query=True)
        text = self.thisNode.islData.get()
        self.textField = pm.scrollField(wordWrap=True, text=text, editable=True, changeCommand=self.textChangeCommand, keyPressCommand=self.textChangeCommand)
        #self.addControl("islData", label="Data")
        pm.button(label="Edit Shader", c=self.editTextButtonCommand)
        pm.button(label="Load Shader", c=self.loadShaderCommand)
        pm.button(label="Save Shader", c=self.saveShaderCommand)
                
    def editButtonReplace(self, attr):
        text = self.thisNode.islData.get()
        if self.textField is not None:
            self.textField.setText(text)
        
    def buildBody(self, nodeName):
        self.thisNode = pm.PyNode(nodeName)
        self.beginLayout("ISLNodeSettings" , collapse=0)
        self.beginNoOptimize()
        # autoAddBegin
        self.callCustom(self.editButtonNew, self.editButtonReplace, 'islData')
        #self.addSeparator()
        # autoAddEnd
        self.endNoOptimize()
        self.endLayout()
        
