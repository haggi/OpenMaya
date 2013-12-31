import pymel.core as pm
import path
import logging
import os
import subprocess
import shutil

log = logging.getLogger("mtfuLogger")

def isOlderThan(fileA, fileB):
    return path.path(fileA).mtime < path.path(fileB).mtime

def makeExr( filePath, exrFile ):
    log.debug("Convert %s to exr %s" % (filePath, exrFile))
    
    #TODO:currently use mentalrays imf_copy, maybe there is a better way...
    cmd = "\"" + os.environ['MENTALRAY_LOCATION'] + "bin/imf_copy\" "
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

def makeMip( originalFile, convertedFile ):
    log.debug("Convert %s to mip %s" % (originalFile, convertedFile))
    
    # hdr2mip.exe and jpg2mip.exe should be in the plugin dir bin directory
    cmd = ""
    if originalFile.endswith(".hdr"):
        cmd += "hdr2mip "
    if originalFile.endswith(".jpg"):
        cmd += "jpg2mip "
    cmd += originalFile + " " + convertedFile   
    log.debug(cmd)
    try:
        if subprocess.call(cmd, shell = True) is not 0:
            log.error("Conversion failed")
            return False            
    except:
        log.warning("Conversion to mip failed " +  str(sys.exc_info()[0]) + " - skipping")
        return False
    return True

def preRenderOptimizeTextures(destFormat = "exr", optimizedFilePath = ""):
    log.debug("local " + __file__)
    log.debug("preRenderOptimizeTextures dest format: {0}".format(destFormat))
    for fileTexture in pm.ls(type="file"):
        fileNamePath = path.path(fileTexture.fileTextureName.get())
        log.debug("Check file texture:" + fileNamePath)
        
        if not fileNamePath.exists():
            log.debug("file texture could not be found, skipping.")
            continue
            
        if fileNamePath.ext[1:] == destFormat:
            log.debug("file texture is already an " + destFormat + " skipping.")
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
        localPath += "." + destFormat
        localPath = path.path(localPath)
        log.debug("local path " + localPath.realpath())
        localPath = localPath.realpath()
        
        doConvert = True
        if localPath.exists():
            if isOlderThan(fileNamePath, localPath):
                log.debug(localPath + " already exists and is older then original file. No conversion needed.")
                doConvert = False
        
        if doConvert:
            # make it a optimized
            if not makeMip(fileNamePath.realpath(), localPath.realpath()):
                log.debug("Problem converting " + fileNamePath)
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
        