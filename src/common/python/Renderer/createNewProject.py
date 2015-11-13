import shutil
import path

import logging

log = logging

BASE_DIR=path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToBase")

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
        string = string.replace(r'"@Globals"', '"' + rendererName.lower() + 'Globals"')
        string = string.replace(r'mayaTo@.', "mayaTo" + rendererName.capitalize())
        if "MayaTo@GlobalsName" in string:
            string = string.replace(r'@', rendererName.lower())
        else:
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
    
    sourceDirBase = BASE_DIR
    
    destDirBase = path.path("{destdir}/mayaTo{renderer}".format(destdir=BASE_DIR.parent, renderer=rendererName.capitalize()))
    
    def recursiveFiles(directory, tab = ""):
        files = directory.listdir()
        for f in files:
            if f.endswith(".opensdf"):
                continue
            print tab + "Checking File:", f
            destElement = f.replace(sourceDirBase, destDirBase)
            destElement = replaceString(destElement, rendererName, shortCut)
            destElement = path.path(destElement)
            if f.isfile():
                if not destElement.exists():
                    print tab + "Copy file", f, "to", destElement
                    shutil.copy(f, destElement)
                    replaceStringInFile(destElement, rendererName, shortCut)
                else:
                    print tab + "File", destElement, "already exists, skipping."
            if f.isdir():
                #print "Is Dir"
                if not destElement.exists():
                    print tab + "Making dir", destElement
                    destElement.makedirs()
                    recursiveFiles(f, tab + "\t")
    
    recursiveFiles(sourceDirBase)
    
if __name__ == "__main__":
    createNewProject(rendererName="Cycles", shortCut="mtcy")
        
    
    