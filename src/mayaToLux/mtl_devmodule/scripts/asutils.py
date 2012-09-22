import pymel.core as pm

def unload_plugin():
    pm.newFile(force=True)
    if pm.pluginInfo("mayatolux.mll", query=True, loaded=True):
        pm.unloadPlugin("mayatolux.mll")    
    pm.newFile(force=True)
    
def load_plugin():
    if not pm.pluginInfo("mayatolux.mll", query=True, loaded=True):
        pm.loadPlugin("mayatolux.mll")
    
def reloadPlugin():
    unload_plugin()
    load_plugin()
    
