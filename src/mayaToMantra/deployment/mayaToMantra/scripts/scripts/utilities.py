import pymel.core as pm
import AETemplates as aetml

def unload_mtm_plugin():
    pm.newFile(force=True)
    if pm.pluginInfo("mayatomantra.mll", query=True, loaded=True):
        pm.unloadPlugin("mayatomantra.mll")    
    pm.newFile(force=True)
    
def load_mtm_plugin():
    if not pm.pluginInfo("mayatomantra.mll", query=True, loaded=True):
        pm.loadPlugin("mayatomantra.mll")
    
def testPlugin():
    print aetml.AEdagNodeInclude.nodeType()
    print "Executing test"