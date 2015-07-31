import path
import logging
import subprocess
import pymel.core as pm

log = logging.getLogger("mtTheaLogger")

def compileOSLShaders(renderer="Corona"):
    oslShaderPath = path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaTo{0}/mt{1}_devmodule/shaders".format(renderer, renderer[:2].lower()))
    oslShadersToCompile = []
    for p in oslShaderPath.listdir("*.osl"):
        oslFile = path.path(p)
        osoFile = path.path(p[:-3] + "oso")
        if osoFile.exists():
            if osoFile.mtime < oslFile.mtime:
                osoFile.remove()
        if osoFile.exists():
            continue
        oslShadersToCompile.append(oslFile)
        
    for oslShader in oslShadersToCompile:
        cmd = "{compiler} -v -o {output} {oslFile}".format(compiler="oslc.exe", output=oslShader.replace(".osl", ".oso"), oslFile=oslShader)
        log.debug("Compiling osl shader: {0}".format(oslShader))
        log.debug("Command: {0}".format(cmd))
        
        IDLE_PRIORITY_CLASS = 64
        process = subprocess.Popen(cmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)

        while 1:
            line = process.stdout.readline()
            if not line: break
            log.debug(line)
            pm.mel.trace(line.strip())



def getOSOFiles(shaderPath, files = None):
    print "Check path", shaderPath
    if files == None:
        files = []
    if len(files) > 10:
        print "too much"
        return []
    for f in path.path(shaderPath).listdir():        
        if f.isdir():
            print f, "istDir"
            newFiles = getOSOFiles(f, files)
            print "FoundFiles:"
            for ff in newFiles:
                print ff
        else:
            if f.endswith(".oso"):
                files.append(f)
    return files

def getShaderInfo(shaderPath):
    print "Getting shader info for path", shaderPath
    
    osoFiles = getOSOFiles(shaderPath)
    
    return osoFiles
