import path
import logging
import subprocess
import pymel.core as pm
import os
import shutil
import sys

log = logging.getLogger("renderLogger")

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

def getOSODirs(renderer = "appleseed"):
    
    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        shaderDir = path.path(__file__).parent() + "/shaders"
        print "Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir."

    osoDirs = set()
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".oso"):
                osoDirs.add(root.replace("\\", "/"))
    return list(osoDirs)

def compileAllShaders(renderer = "appleseed"):
    
    try:
        shaderDir = os.environ['{0}_OSL_SHADERS_LOCATION'.format(renderer.upper())]
    except KeyError:
        print "Error: there is no environmentvariable called OSL_SHADERS_LOCATION. Please create one and point it to the base shader dir."
        
    include_dir = os.path.join(shaderDir, "src/include")
    
    oslc_cmd = "oslc"
    for root, dirname, files in os.walk(shaderDir):
        for filename in files:
            if filename.endswith(".osl"):
                oslPath =  os.path.join(root, filename)
                dest_dir = root.replace("\\", "/").replace("shaders/src", "shaders") + "/"
                if not os.path.exists(dest_dir):
                    os.makedirs(dest_dir)
                osoOutputPath = dest_dir + filename.replace(".osl", ".oso")                
                osoOutputFile = path.path(osoOutputPath)
                oslInputFile = path.path(oslPath)
                
                if osoOutputFile.exists():
                    if osoOutputFile.mtime > oslInputFile.mtime:
                        continue
                    else:
                        osoOutputFile.remove()

                print "compiling shader: " + oslInputFile
    
                saved_wd = os.getcwd()
                os.chdir(root)
                compileCmd = oslc_cmd + " -v -I" + include_dir + ' -o '+ osoOutputPath + ' ' + oslInputFile
                print "Compile cmd", compileCmd
                #retcode = os.system(compileCmd)
                
                IDLE_PRIORITY_CLASS = 64
                process = subprocess.Popen(compileCmd, bufsize=1, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, creationflags=IDLE_PRIORITY_CLASS)
        
                while 1:
                    line = process.stdout.readline()
                    if not line: break
                    log.debug(line)
                    pm.mel.trace(line.strip())
                    
                os.chdir(saved_wd)
    
    print "All shaders compiled!"
