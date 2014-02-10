import pymel.core as pm
import path
import logging
import os
import subprocess
import shutil

log = logging.getLogger("mtapLogger")

def isOlderThan(fileA, fileB):
    return path.path(fileA).mtime < path.path(fileB).mtime

def makeExrTiled(sourceFile):
    tiledPath = path.path(sourceFile.replace(".exr", ".t_exr"))
    cmd = "maketiledexr {0} {1}".format(sourceFile, tiledPath)    
    try:
        log.debug(cmd)
        subprocess.call(cmd, shell = True)
    except:
        log.warning("Conversion to tiled exr failed: {0}\nskipping tiled exr for this file.".format(sys.exc_info()[0]))
        return True
    
    try:
        tiledPath.move(sourceFile)
    except:
        log.warning("Renaming tiled exr failed: {0}\nskipping file.".format(sys.exc_info()[0]))
        return False
    
def makeExr(sourceFile, destFile):
    
    #TODO:currently use mentalrays imf_copy, maybe there is a better way...
    #cmd = "\"" + os.environ['MENTALRAY_LOCATION'] + "bin/imf_copy\" "
    cmd = "imf_copy {0} {1}".format(sourceFile, destFile)
    log.debug(cmd)
    try:
        if subprocess.call(cmd, shell = True) is not 0:
            log.error("Conversion of {0} to {1} failed".format(sourceFile, destFile))
            return False            
    except:
        log.warning("Conversion to exr failed: {0}\nskipping file.".format(sys.exc_info()[0]))
        return False
    
    makeExrTiled(destFile)
    
def makeDestFile( sourceFile, destFile ):
    log.debug("Convert file {0} to {1}".format(sourceFile, destFile))
    
    if not destFile.dirname().exists():
        destFile.makedirs()
    
    if destFile.endswith(".exr"):
        return makeExr(sourceFile, destFile)    
    
    return True

def preRenderOptimizeTextures(destFormat = "exr", optimizedFilePath = ""):
    
    for fileTexture in pm.ls(type="file"):
        fileNamePath = path.path(fileTexture.fileTextureName.get())
        log.debug("Check file texture:" + fileNamePath)
    
        if not fileNamePath.exists():
            log.debug("Original file texture {0} could not be found, skipping.".format(fileNamePath))
            continue
                    
        if fileTexture.useFrameExtension.get():
            log.debug("File texture is animated skipping.")
            continue

        if fileNamePath.lower().startswith(optimizedFilePath.lower()):
            log.debug("Orig file is already local - skipping conversion.")
            continue
        
        if fileNamePath.endswith(destFormat):
            log.debug("Orig file {0} already has destFormat {1}, skipping conversion.".format(fileNamePath, destFormat))
            continue
        
        # path will contain either contain a leading slash for 
        # unix /server/textures/file.exr
        # or a drive in windows like c:
        # or a dfs name like //server/textures.... 
        optimizedFilePath = path.path(optimizedFilePath)
        localPath = optimizedFilePath
        if fileNamePath[1] == ":":
            localPath = optimizedFilePath / fileNamePath[3:]
        if fileNamePath.startswith("//"):
            localPath = optimizedFilePath / fileNamePath[2:]
        elif fileNamePath.startswith("/"):
            localPath = optimizedFilePath / fileNamePath[1:]
        localPath = path.path("{0}.{1}".format(localPath, destFormat))
        log.debug("Local destination path {0}".format(localPath.realpath()))
        localPath = localPath.realpath()
        
        doConvert = True
        if localPath.exists():
            if isOlderThan(fileNamePath, localPath):
                log.debug("Local file {0} already exists and is younger than the original file, skipping conversion.".format(localPath.basename()))
                doConvert = False
        
        if doConvert:
            if not makeDestFile(fileNamePath.realpath(), path.path(localPath.replace(".exr", "_t.exr")).realpath()):
                log.debug("Problem converting {0}".format(fileNamePath))
                continue
                    
        if not fileTexture.hasAttr("origFilePath"):
            fileTexture.addAttr("origFilePath", dt="string")
        fileTexture.origFilePath.set(fileNamePath)
        fileTexture.fileTextureName.set(localPath)
        
def postRenderOptimizeTextures():
    for fileTexture in pm.ls(type="file"):
        if fileTexture.hasAttr("origFilePath"):
            fileTexture.fileTextureName.set(fileTexture.origFilePath.get())
            fileTexture.origFilePath.delete()
        