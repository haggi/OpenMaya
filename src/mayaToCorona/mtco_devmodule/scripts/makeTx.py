
import path
import logging
import subprocess
import pymel.core as pm

log = logging.getLogger("mtCoronaLogger")

binDir = path.path(__file__).dirname().parent / "bin"
converterCmd = binDir/"makeTx.exe"

class FileTexture:
    def __init__(self, filePath):
        self.path = path.path(filePath)
        self.valid = True
        self.needsConversion = True
        if not self.path.exists():
            self.valid = False
            return None
        self.destFile = path.path(self.path + ".exr.tx")
        if self.destFile.exists():
            if self.path.mtime > self.destFile.mtime:
                log.warn("The existing tx file {0} is older than the original file - recreating.".format(self.destFile))
                self.destFile.remove()
            else:
                self.needsConversion = False
                
        print "Dest File", self.destFile
        
    def convert(self):
        if not self.needsConversion:
            return
        if not self.valid:
            return
        
        destFileTmp = ".".join(self.destFile.split(".")[:-1])
        cmd = "{converterCmd} -v -oiio -o {destFile} {origFile} ".format(converterCmd=converterCmd, destFile=destFileTmp, origFile=self.path)        
        print cmd
        if subprocess.call(cmd, shell = True) is not 0:
            log.error("Conversion failed")
        else:
           
            path.path(destFileTmp).rename(self.destFile)
            print "Done"               
    
def optimizeFileTextureNodes():
    for f in pm.ls(type="file"):
        FileTexture(f.fileTextureName.get()).convert()
        
        