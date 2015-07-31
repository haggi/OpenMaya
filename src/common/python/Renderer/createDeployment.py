import shutil
import os
import path
import zipfile
import sys

def zip_folder(folder_path, output_path):
    """Zip the contents of an entire folder (with that folder included
    in the archive). Empty subfolders will be included in the archive
    as well.
    """
    parent_folder = os.path.dirname(folder_path)
    os.chdir(folder_path)
    # Retrieve the paths of the folder contents.
    contents = os.walk(folder_path)
    try:
        zip_file = zipfile.ZipFile(output_path, 'w', zipfile.ZIP_DEFLATED)
        for root, folders, files in contents:
            # Include all subfolders, including empty ones.
            for folder_name in folders:
                print "folder name", folder_name, "parent folder", parent_folder
                absolute_path = os.path.join(root, folder_name)
                relative_path = absolute_path.replace(parent_folder + '\\','')
                print "Adding folder '%s' to archive." % relative_path
                #zip_file.write(absolute_path, relative_path)
                zip_file.write(relative_path, folder_name)
            for file_name in files:
                absolute_path = os.path.join(root, file_name)
                relative_path = absolute_path.replace(parent_folder + '\\', '')
                print "Adding '%s' to archive." % absolute_path
                zip_file.write(absolute_path, relative_path)
        print "'%s' created successfully." % output_path
    except IOError, message:
        print message
        sys.exit(1)
    except OSError, message:
        print message
        sys.exit(1)
    except zipfile.BadZipfile, message:
        print message
        sys.exit(1)
    finally:
        zip_file.close()


def zipdir(p, zipf):
#    ed = []
#    for root, dirs, files in os.walk(p):
#        for file in files:
#            zipf.write(os.path.join(root, file))
#        for d in dirs:
#            dd = path.path(os.path.join(root, d))
#            if len(dd.listdir()) > 0:
#                continue
#            else:
#                zipf.write(os.path.join(root, dd))
#                ed.append(dd)
#    print "empty dirs", ed
    pass

def createDeployment(renderer, shortCut, mayaRelease):
    
    projectName = "mayaTo{0}".format(renderer.capitalize())
    if os.name == 'nt':
        basePath = path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/{0}".format(projectName))
    else:
        print "Os not supported"
        return
    
    sourceDir = path.path(basePath + "/" +  shortCut + "_devmodule")
    devDestDir = path.path("{0}/deployment/{1}_{2}".format(basePath, projectName, mayaRelease))
    if devDestDir.exists():
        print "removing old {0} dir".format(devDestDir)
        shutil.rmtree(devDestDir)

    deploymentDir =  path.path("{0}/deployment".format(basePath))
    # dev destination directories                
    try:
        print "Creating destination directory", devDestDir
        os.makedirs(devDestDir)
    except WindowsError:
        print "\nERROR: Failure creating {0}\n".format(devDestDir)
        return

    # get folders from dev module
    devFolders = ['plug-ins', 'shaders']
    
    for folder in devFolders:
        print "Creating destination directory", devDestDir + "/" + folder
        os.makedirs(devDestDir + "/" + folder)
    
    #module file
    shutil.copy(sourceDir+"/mayaTo" + renderer.capitalize() + ".mod", devDestDir)
    mfh = open(devDestDir+"/mayaTo" + renderer.capitalize() + ".mod", "r")
    content = mfh.readlines()
    mfh.close()
    mfh = open(devDestDir+"/mayaTo" + renderer.capitalize() + ".mod", "w")
    for c in content:
        mfh.write(c.replace(shortCut +"_devmodule", "mayaTo" + renderer.capitalize() + "_{0}".format(mayaRelease)))
    mfh.close()

    
    #mll
    print "copy ", sourceDir + "/plug-ins/mayato" + renderer + "_maya"+ mayaRelease + ".mll"
    shutil.copy(sourceDir + "/plug-ins/mayato" + renderer + "_maya"+ mayaRelease + ".mll", devDestDir + "/plug-ins/mayato" + renderer + ".mll")
    plugInDir = sourceDir + "/plug-ins"
    print "-------------- plugInDir ------------------", plugInDir
    files = plugInDir.listdir("*.xml")
    for xmlFile in files:
        print "Copy xml file", xmlFile
        shutil.copy(xmlFile, devDestDir + "/plug-ins/")
    
    #ressources
    scDir = path.path(sourceDir + "/ressources")
    shutil.copytree(scDir, devDestDir + "/ressources")
    
    manuapPdf = None
    #bin
    if renderer == "appleseed":
        dkDir = path.path(sourceDir.parent + "/devkit/appleseed_devkit_win64/bin/Release")
        for f in dkDir.listdir():
            shutil.copy(f, devDestDir + "/bin")

    
    manualPdf = path.path("C:/daten/web/openmaya/manuals/{0}/{0}.pdf".format(projectName))
    if renderer == "corona":
        binDir = sourceDir + "/bin"
        destBinDir = devDestDir + "/bin/"
        if not destBinDir.exists():
            destBinDir.makedirs()
        for f in ["Corona_Release.dll", "CoronaOpenExr2.dll", "wxmsw30u_gl_vc_corona.dll", "wxmsw30u_vc_corona.dll", "Config-6773585.conf", "oslc.exe", "oslinfo.exe"]:
            sourceFile = binDir + "/" + f
            destFile =   destBinDir + f
            print "Copy ", sourceFile, "to", destFile
            shutil.copy(sourceFile, devDestDir + "/bin")
            
        shaderDir = sourceDir + "/shaders"
        shaderDestDir = devDestDir + "/shaders"
        shutil.rmtree(shaderDestDir)
        shutil.copytree(shaderDir, shaderDestDir)
                
    if renderer == "indigo":
        binDir = sourceDir + "/bin"
        destDir = devDestDir + "/bin"
        shutil.copytree(binDir, destDir)
        try:
            os.remove(destDir + "/licence.sig")
        except:
            pass
        
    if manualPdf is not None:
        docsDir = deploymentDir + "/docs"
        print "Check", docsDir
        if not docsDir.exists():
            print "Create docs dir"
            docsDir.makedirs()
        manualPdf.copy2(docsDir)
        
    iconsSourceDir = "{0}/icons/".format(sourceDir)
    iconsDestDir = "{0}/icons/".format(devDestDir)
    shutil.copytree(iconsSourceDir, iconsDestDir)

    #common python scripts
    scDir = path.path(sourceDir.parent.parent + "/common/python/")
    shutil.copytree(scDir, devDestDir + "/scripts/")
    scDir = devDestDir + "/scripts/"
    files = scDir.listdir(".*")
    for f in files:
        if f.isdir():
            f.rmtree()
        else:
            f.remove()
    scDir = path.path(scDir + "Renderer")
    files = scDir.listdir("*.pyc")
    for f in files:
        f.remove()

    #common mel replacement scripts
    #scDir = path.path(sourceDir.parent.parent + "/common/mel/maya{0}".format(mayaRelease))
    #for f in scDir.listdir("*.mel"):
    #    f.copy("{destDir}/scripts/{melName}".format(destDir=devDestDir, melName=f.basename()))

    #renderer python scripts
    for root, dirs, files in os.walk(sourceDir + "/scripts/"):
            for file in files:
                ff = path.path(os.path.join(root, file))
                if ff.endswith(".pyc"):
                    continue
                if file.startswith("."):
                    continue
                dst = ff.replace(sourceDir, devDestDir)
                print "copy", ff, "to", dst
                shutil.copy(ff, dst)
            for d in dirs:
                dd = path.path(os.path.join(root, d))
                dst = dd.replace(sourceDir, devDestDir)
                print "mkdir", dst
                os.makedirs(dst)
                
    # get external folders 
    extFolders = ['{0}Examples'.format(projectName)]
        
    for folder in extFolders:
        deployDir = devDestDir.parent
        try:
            shutil.rmtree(deployDir + "/" + folder)
        except:
            pass
        shutil.copytree(basePath + "/" + folder, deployDir + "/" + folder)

def createDeploymentCombined(renderer, shortCut):
    
    projectName = "mayaTo{0}".format(renderer.capitalize())
    if os.name == 'nt':
        basePath = path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/{0}".format(projectName))
    else:
        print "Os not supported"
        return
    
    sourceDir = path.path(basePath + "/" +  shortCut + "_devmodule")
    devDestDir = path.path("{0}/deployment/{1}".format(basePath, projectName))
    if devDestDir.exists():
        print "removing old {0} dir".format(devDestDir)
        shutil.rmtree(devDestDir)

    deploymentDir =  path.path("{0}/deployment".format(basePath))
    # dev destination directories                
    try:
        print "Creating destination directory", devDestDir
        os.makedirs(devDestDir)
    except WindowsError:
        print "\nERROR: Failure creating {0}\n".format(devDestDir)
        return

    for mayaVersionName in ["2014", "2015", "2016"]:
        mayaVersionSubPlugInDir = "{deploymentDir}/{mayaVersion}/plug-ins".format(deploymentDir=devDestDir, mayaVersion=mayaVersionName)
        print "Creating version depenedent directory", mayaVersionSubPlugInDir
        os.makedirs(mayaVersionSubPlugInDir)
        pluginFile = path.path(sourceDir + "/" +  mayaVersionName + "/plug-ins/mayaTo" + renderer.capitalize() + ".mll")
        pluginDestFile = mayaVersionSubPlugInDir + "/mayaTo" + renderer.capitalize() + ".mll"
        shutil.copy2(pluginFile, pluginDestFile)
    # get folders from dev module
    devFolders = ['shaders']
    
    for folder in devFolders:
        print "Creating destination directory", devDestDir + "/" + folder
        os.makedirs(devDestDir + "/" + folder)
    
    #module file
    shutil.copy(sourceDir+"/mayaTo" + renderer.capitalize() + ".mod", devDestDir)
    mfh = open(devDestDir+"/mayaTo" + renderer.capitalize() + ".mod", "r")
    content = mfh.readlines()
    mfh.close()
    mfh = open(devDestDir+"/mayaTo" + renderer.capitalize() + ".mod", "w")
    for c in content:
        mfh.write(c.replace(shortCut +"_devmodule", "mayaTo" + renderer.capitalize()))
    mfh.close()

    
    #mll
    pluginsDir = path.path(sourceDir + "/plug-ins/")
    xmlFiles = pluginsDir.listdir("*.xml")    
    for destMayaVersion in ["2014", "2015", "2016"]:
        plugInDestDir = devDestDir + "/" + destMayaVersion + "/plug-ins"
        for xmlFile in xmlFiles:
            print "Copy xml file", xmlFile
            shutil.copy(xmlFile, plugInDestDir)
            
    #ressources
    scDir = path.path(sourceDir + "/resources")
    shutil.copytree(scDir, devDestDir + "/resources")
    
    manuapPdf = None
    #bin
    if renderer == "appleseed":
        dkDir = path.path(sourceDir.parent + "/devkit/appleseed_devkit_win64/bin/Release")
        for f in dkDir.listdir():
            shutil.copy(f, devDestDir + "/bin")

    
    manualPdf = path.path("C:/daten/web/openmaya/manuals/{0}/{0}.pdf".format(projectName))
    if renderer == "corona":
        binDir = sourceDir + "/bin"
        destBinDir = devDestDir + "/bin/"
        if not destBinDir.exists():
            destBinDir.makedirs()
        for f in ["Corona_Release.dll", "CoronaOpenExr2.dll", "wxmsw30u_gl_vc_corona.dll", "wxmsw30u_vc_corona.dll", "Config-6773585.conf", "oslc.exe", "maketx.exe", "oslinfo.exe"]:
            sourceFile = binDir + "/" + f
            destFile =   destBinDir + f
            print "Copy ", sourceFile, "to", destFile
            shutil.copy(sourceFile, devDestDir + "/bin")
            
        shaderDir = sourceDir + "/shaders"
        shaderDestDir = devDestDir + "/shaders"
        shutil.rmtree(shaderDestDir)
        shutil.copytree(shaderDir, shaderDestDir)
                
    if renderer == "indigo":
        binDir = sourceDir + "/bin"
        destDir = devDestDir + "/bin"
        shutil.copytree(binDir, destDir)
        try:
            os.remove(destDir + "/licence.sig")
        except:
            pass
        
    if manualPdf is not None:
        docsDir = deploymentDir + "/docs"
        print "Check", docsDir
        if not docsDir.exists():
            print "Create docs dir"
            docsDir.makedirs()
        manualPdf.copy2(docsDir)
        
    iconsSourceDir = "{0}/icons/".format(sourceDir)
    iconsDestDir = "{0}/icons/".format(devDestDir)
    shutil.copytree(iconsSourceDir, iconsDestDir)

    #common python scripts
    scDir = path.path(sourceDir.parent.parent + "/common/python/")
    shutil.copytree(scDir, devDestDir + "/scripts/")
    scDir = devDestDir + "/scripts/"
    files = scDir.listdir(".*")
    for f in files:
        if f.isdir():
            f.rmtree()
        else:
            f.remove()
    scDir = path.path(scDir + "Renderer")
    files = scDir.listdir("*.pyc")
    for f in files:
        f.remove()

    #renderer python scripts
    for root, dirs, files in os.walk(sourceDir + "/scripts/"):
            for file in files:
                ff = path.path(os.path.join(root, file))
                if ff.endswith(".pyc"):
                    continue
                if file.startswith("."):
                    continue
                dst = ff.replace(sourceDir, devDestDir)
                print "copy", ff, "to", dst
                shutil.copy(ff, dst)
            for d in dirs:
                dd = path.path(os.path.join(root, d))
                dst = dd.replace(sourceDir, devDestDir)
                print "mkdir", dst
                os.makedirs(dst)
                
    # get external folders 
    extFolders = ['{0}Examples'.format(projectName)]
        
    for folder in extFolders:
        deployDir = devDestDir.parent
        try:
            shutil.rmtree(deployDir + "/" + folder)
        except:
            pass
        shutil.copytree(basePath + "/" + folder, deployDir + "/" + folder)
    
if __name__ == "__main__":
    #createDeployment("appleseed", "mtap")    
    createDeploymentCombined("corona", "mtco")
    #createDeploymentCombined("indigo", "mtin")
    print "Deployment creation done"
