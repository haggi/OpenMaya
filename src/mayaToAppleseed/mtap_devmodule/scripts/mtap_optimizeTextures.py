import pymel.core as pm
import path
import logging
import os
import subprocess
import shutil

log = logging.getLogger("mtapLogger")

def isOlderThan(fileA, fileB):
    return path.path(fileA).mtime < path.path(fileB).mtime

def makeExr( filePath, exrFile ):
    log.debug("Convert %s to exr %s" % (filePath, exrFile))
    
    #TODO:currently use mentalrays imf_copy, maybe there is a better way...
    #cmd = "\"" + os.environ['MENTALRAY_LOCATION'] + "bin/imf_copy\" "
    cmd = "imf_copy "
    cmd += filePath + " " + exrFile   
    log.debug(cmd)
    try:
        if subprocess.call(cmd, shell = True) is not 0:
            log.error("Conversion failed")
            return False            
    except:
        log.warning("Conversion to exr failed " +  str(sys.exc_info()[0]) + " - skipping")
        return False
    return True

def preRenderOptimizeTextures(destFormat = "exr", optimizedFilePath = ""):
    for fileTexture in pm.ls(type="file"):
        fileNamePath = path.path(fileTexture.fileTextureName.get())
        log.debug("Check file texture:" + fileNamePath)
        
        if not fileNamePath.exists():
            log.debug("file texture could not be found, skipping.")
            continue
                    
        if fileTexture.useFrameExtension.get():
            log.debug("file texture is animated skipping.")
            continue

        # path will contain either contain a leading slash for 
        # unix /server/textures/file.exr
        # or a drive in windows like c:
        # or a dfs name like //server/textures.... 
        optimizedFilePath = path.path(optimizedFilePath)
        log.debug("optimizedFilePath " + optimizedFilePath)
        if fileNamePath[1] == ":":
            localPath = optimizedFilePath / fileNamePath[3:]
        if fileNamePath.startswith("//"):
            localPath = optimizedFilePath / fileNamePath[2:]
        if fileNamePath.startswith("/"):
            localPath = optimizedFilePath / fileNamePath[1:]
        localPath += ".exr"
        localPath = path.path(localPath)
        log.debug("local path " + localPath.realpath())
        localPath = localPath.realpath()
        
        doConvert = True
        if localPath.exists():
            if isOlderThan(fileNamePath, localPath):
                log.debug(localPath + " already exists and is older then original file. No conversion needed.")
                doConvert = False
        
        if doConvert:
            # make it a exr
            if not makeExr(fileNamePath.realpath(), path.path(localPath.replace(".exr", "_t.exr")).realpath()):
                log.debug("Problem converting " + fileNamePath)
                continue
            
            #shutil.copy(localPath.replace(".exr", "_t.exr"), localPath)
            # executable: maketiledexr(.exe)
            cmd = "maketiledexr"
            
            cmd += " " + localPath.replace(".exr", "_t.exr") + " " + localPath
            
            try:
                log.debug("make tiledExr: " + cmd)
                subprocess.call(cmd, shell = True)
            except:
                log.warning("Conversion to tiled exr failed " +  str(sys.exc_info()[0]) + " - skipping")
                continue 
            
            os.remove(localPath.replace(".exr", "_t.exr"))
        
        if not fileTexture.hasAttr("origFilePath"):
            fileTexture.addAttr("origFilePath", dt="string")
        fileTexture.origFilePath.set(fileNamePath)
        fileTexture.fileTextureName.set(localPath)
        
def postRenderOptimizeTextures():
    for fileTexture in pm.ls(type="file"):
        if fileTexture.hasAttr("origFilePath"):
            fileTexture.fileTextureName.set(fileTexture.origFilePath.get())
            fileTexture.origFilePath.delete()
        