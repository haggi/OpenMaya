import shutil
import gzip
import os


def createDeployment(renderer):
    
    if os.name == 'nt':
        basePath = path.path("C:/users/haggi/coding/OpenMaya/src/mayaTo" + renderer.capitalize())
    else:
        pass
    if not basePath:
        print "No base path"
        return
    
    pass

if __name__ == "__main__":
    createDeployment("appleseed")