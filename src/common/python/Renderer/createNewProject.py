import shutil
import path

import logging
import logging.handlers


## Set up a specific logger with our desired output level
#log = logging.getLogger('MyLogger')
#log.setLevel(logging.DEBUG)
#
#class MyGlogging.Handler()
#log.addHandler(hdlr)

log = logging

def replaceStringInFile(fileName, rendererName, shortCut):    
    print "replaceStringInFile", fileName
    fh = open(fileName)
    content = fh.readlines()
    fh.close()
    
    for index, string in enumerate(content):
        if "@staticmethod" in string:
            continue
        string = string.replace(r'@Renderer', rendererName.capitalize()+"Renderer")
        string = string.replace(r'mt@_', shortCut + "_")    
        string = string.replace(r'@.', rendererName + ".")
        string = string.replace(r'mayaTo@.', "mayaTo" + rendererName.capitalize())
        string = string.replace(r'@', rendererName)
        content[index] = string   

    fh = open(fileName, "w")
    fh.writelines(content)
    fh.close()
        
def replaceString(string, rendererName, shortCut):    
    string = string.replace(r'mt@_', shortCut + "_")    
    string = string.replace(r'@.', rendererName + ".")
    string = string.replace(r'mayaTo@.', "mayaTo" + rendererName.capitalize())
    string = string.replace(r'@', rendererName)    
    return string
    
def createNewProject(rendererName=None, shortCut=None):
    if not rendererName:
        log.error("No renderer Name")
        return

    if not shortCut:
        log.error("No shortCut")
        return
    
    sourceDirBase = path.path("C:/users/haggi/coding/OpenMaya/src/mayaToBase")
    
    destDirBase = path.path("C:/users/haggi/coding/OpenMaya/src/mayaTo" + rendererName.capitalize())
    
    def recursiveFiles(directory):
        files = directory.listdir()
        for f in files:
            print f
            destElement = f.replace(sourceDirBase, destDirBase)
            destElement = replaceString(destElement, rendererName, shortCut)
            destElement = path.path(destElement)
            if f.isfile():
                if not destElement.exists():
                    print "Copy file", destElement
                    shutil.copy(f, destElement)
                    replaceStringInFile(destElement, rendererName, shortCut)
            if f.isdir():
                #print "Is Dir"
                if not destElement.exists():
                    print "Making dir", destElement
                    destElement.makedirs()
                    recursiveFiles(f)
    
    recursiveFiles(sourceDirBase)
    
if __name__ == "__main__":
    createNewProject(rendererName="Mitsuba", shortCut="mtmi")
        
    
    