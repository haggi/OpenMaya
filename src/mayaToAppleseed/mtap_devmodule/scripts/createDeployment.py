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
    
    if os.name == 'nt':
        basePath = path.path("C:/users/haggi/coding/OpenMaya/src/mayaTo" + renderer.capitalize())
    else:
        pass
    if not basePath:
        print "No base path"
        return
    
    sourceDir = path.path(basePath + "/" +  shortCut + "_devmodule")
    destDir = path.path(basePath + "/deployment/mayaTo" + renderer.capitalize())
    if destDir.exists():
        print "rm old dir"
        shutil.rmtree(destDir)
        print "rm old dir done"
        
    os.makedirs(destDir)
    
    # get folders from dev module
    devFolders = ['bin', 'icons', 'plug-ins', 'ressources', 'shaders']
    
    for folder in devFolders:
        os.makedirs(destDir + "/" + folder)

    #module file
    shutil.copy(sourceDir+"/mayaTo" + renderer.capitalize() + ".mod", destDir)
    mfh = open(destDir+"/mayaTo" + renderer.capitalize() + ".mod", "r")
    content = mfh.readlines()
    mfh.close()
    mfh = open(destDir+"/mayaTo" + renderer.capitalize() + ".mod", "w")
    for c in content:
        mfh.write(c.replace(shortCut +"_devmodule", "mayaTo" + renderer.capitalize()))
    mfh.close()
    
    #mll
    print "copy ", sourceDir + "/plug-ins/mayato" + renderer + "_maya"+ mayaRelease + "release.mll"
    shutil.copy(sourceDir + "/plug-ins/mayato" + renderer + "_maya"+ mayaRelease + "release.mll", destDir + "/plug-ins/mayato" + renderer + ".mll")

    #bin
    if renderer == "appleseed":
        dkDir = path.path(sourceDir.parent + "/devkit/appleseed_devkit_win64/bin/Release")
        for f in dkDir.listdir():
            shutil.copy(f, destDir + "/bin")

    #scripts
    scDir = path.path(sourceDir.parent.parent + "/common/python/")
    shutil.copytree(scDir, destDir + "/scripts/")
    scDir = destDir + "/scripts/"
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

    for root, dirs, files in os.walk(sourceDir + "/scripts/"):
            for file in files:
                ff = path.path(os.path.join(root, file))
                if ff.endswith(".pyc"):
                    continue
                if file.startswith("."):
                    continue
                dst = ff.replace(sourceDir, destDir)
                print "copy", ff, "to", dst
                shutil.copy(ff, dst)
            for d in dirs:
                dd = path.path(os.path.join(root, d))
                dst = dd.replace(sourceDir, destDir)
                print "mkdir", dst
                os.makedirs(dst)
                
#    scDir = path.path(sourceDir + "/scripts/")
#    for f in scDir.listdir("*.py"):
#        try:
#            shutil.copytree(f, destDir + "/scripts/")
#        except:
#            pass
#    scDir = path.path(sourceDir + "/scripts/AETemplates")
#    for f in scDir.listdir("*.py"):
#        try:
#            shutil.copytree(f, destDir + "/scripts/AETemplates")
#        except:
#            pass
#    print "chdir", destDir.parent    
#    os.chdir(destDir.parent)        
#    zippedFileName = "mayato" + renderer.lower() + ".zip"
#    print "Creating zip file:", zippedFileName
#    zip = zipfile.ZipFile(zippedFileName, 'w')
#    zipdir("mayato" + renderer.lower(), zip)
#    zip.close() 
#    zip_folder(destDir, zippedFileName)

#    for f in files:
#        print "remove", f
        #shutil.rmtree(f)
    
if __name__ == "__main__":
    createDeployment("appleseed", "mtap", "2013")