import shutil
import os
import path
import zipfile
import sys

class Deployment:
    rendererDict = {'appleseed': {'shortcut':'mtap', 'hasTools' : True},
                    'lux' : {'shortcut':'mtlu', 'hasTools' : False},
                    'fuji' : {'shortcut':'mtfu', 'hasTools' : False},
                    'kray' : {'shortcut':'mtkr', 'hasTools' : False},
                    'centi' : {'shortcut':'mtce', 'hasTools' : False},
                    'corona' : {'shortcut':'mtco', 'hasTools' : False},
                    'fluid' : {'shortcut':'mtfl', 'hasTools' : False},
                    'mitsuba' : {'shortcut':'mtmi', 'hasTools' : False}}
    
    mayaToBaseDir = "H:/UserDatenHaggi/Documents/coding/OpenMaya/src"
    mayaToPythonBaseDir = mayaToBaseDir + "/common/python"
    
    ignoreFileTypes = ['pyc', 'gitignore', 'project', 'pydevproject']
    
    def __init__(self):
        self.renderer = None
        self.shortCut = None
        self.mayaReleases = None
        
    def setRenderer(self, renderer):
        self.renderer = renderer
        self.shortCut = Deployment.rendererDict[renderer]['shortcut']
        self.sourceBaseDir = path.path("{0}/mayaTo{1}/{2}_devmodule".format(Deployment.mayaToBaseDir, self.renderer, self.shortCut)) 
        self.destBaseDir = path.path("{0}/mayaTo{1}/deployment/mayaTo{1}".format(Deployment.mayaToBaseDir, self.renderer.capitalize())) 
        print self.sourceBaseDir, self.destBaseDir
        
        if self.destBaseDir.exists():
            shutil.rmtree(self.destBaseDir, ignore_errors=True)
            
        if not self.destBaseDir.exists():
            self.destBaseDir.makedirs()
                
    def setVersions(self, versions):
        self.mayaReleases = versions

    def fileOkay(self, fileName):
        if path.path(fileName).basename().startswith("."):
            return False
        if fileName.startswith("."):
            return False
        if fileName.split(".")[-1] in Deployment.ignoreFileTypes:
            return False
        if fileName.endswith(".mll"):
            if not "Release" in fileName:
                return False
        return True

    def filterFileNames(self, fileName):
        newName = fileName
        if "Release.mll" in newName:
            newName = newName.replace("Release.mll", ".mll") 
        return newName

    def filterFileContents(self, fileToCheck):
        if fileToCheck.endswith(".mod"):
            fh = open(fileToCheck, "r")
            content = fh.readlines()
            fh.close()
            fh = open(fileToCheck, "w")
            for line in content:
                newLine = line
                devmod = "../{0}_devmodule".format(self.shortCut)
                if devmod in newLine:
                    newLine = "+ LOCALE:en_US mayaTo{0} 0.1.0 ../mayaTo{0}\n".format(self.renderer.capitalize())
                fh.write(newLine)
            fh.close()

    def copyDir(self, directory, destDirExtension = None):
        for element in path.path(directory).listdir():
            if not self.fileOkay(element):
                continue            
            if element.isdir():
                self.copyDir(element, destDirExtension)
            else:
                sourceFile = element.realpath().replace("\\", "/")
                destDir = self.destBaseDir
                if destDirExtension is not None:
                    destDir += "/" + destDirExtension
                destFile = sourceFile.replace(self.sourceBaseDir, destDir)
                destFile = destFile.replace(Deployment.mayaToPythonBaseDir, destDir)
                destDir = path.path(destFile).dirname()
                if not destDir.exists():
                    destDir.makedirs()
                destFile = self.filterFileNames(destFile)
                shutil.copy2(sourceFile, destFile)
                self.filterFileContents(destFile)
    
    def createTools(self):
        toolsDir = self.destBaseDir.parent + "/" + self.renderer + "Tools"
        shutil.copytree(self.destBaseDir, toolsDir)

    def createDeployment(self):
        self.copyDir(self.sourceBaseDir)
        self.copyDir(Deployment.mayaToPythonBaseDir, destDirExtension = "scripts" )
        if Deployment.rendererDict[self.renderer]['hasTools']:
            print "create tools"
            self.createTools()
            
    
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
    sourceLib = "{0}/plug-ins/mayato{1}_maya{2}release.mll".format(sourceDir, renderer, mayaRelease)
    destLib = "{0}/plug-ins/mayato{1}_maya{2}.mll".format(destDir, renderer, mayaRelease)
    print "copy ", sourceLib, "to", destLib
    shutil.copy(sourceLib, destLib)

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
    
    
if __name__ == "__main__":
    #createDeployment("appleseed", "mtap", "2013")
    #d = Deployment()
    #d.setRenderer('appleseed')
    #d.setVersions(['2013', '2013.5', '2014'])
    #d.createDeployment()
    
    d = Deployment()
    d.setRenderer('corona')
    d.setVersions(['2015', '2016'])
    d.createDeployment()    