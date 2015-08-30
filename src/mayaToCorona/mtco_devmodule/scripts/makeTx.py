
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
        cmd = "{converterCmd} -v -oiio -o \"{destFile}\" \"{origFile}\"".format(converterCmd=converterCmd, destFile=destFileTmp, origFile=self.path)        
        print cmd
        try:
            subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True, universal_newlines=True)
            path.path(destFileTmp).rename(self.destFile)
        except subprocess.CalledProcessError as error:
            log.error("Conversion failed.\n\t{0}".format(error.output))
#         process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
#         while 1:
#             line = process.stdout.readline()
#             if not line: break
#             log.debug(line)
#             pm.mel.trace(line.strip())
            
def optimizeFileTextureNodes():
    for f in pm.ls(type="file"):
        FileTexture(f.fileTextureName.get()).convert()
        
        