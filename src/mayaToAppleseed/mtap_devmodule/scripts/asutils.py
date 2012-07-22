import pymel.core as pm

def unload_plugin():
    pm.newFile(force=True)
    if pm.pluginInfo("mayatoappleseed.mll", query=True, loaded=True):
        pm.unloadPlugin("mayatoappleseed.mll")    
    pm.newFile(force=True)
    
def load_plugin():
    if not pm.pluginInfo("mayatoappleseed.mll", query=True, loaded=True):
        pm.loadPlugin("mayatoappleseed.mll")
    
def reloadPlugin():
    unload_plugin()
    load_plugin()
    
