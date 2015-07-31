import sys
import os

sys.path.append("C:/daten/install/3d/Appleseed/appleseed/bin")
sys.path.append("C:/daten/install/3d/Appleseed/appleseed/bin/appleseed")

os.environ["PYTHONPATH"] = os.environ["PYTHONPATH"] + ";C:/daten/install/3d/Appleseed/appleseed/bin/appleseed"
os.environ["PATH"] = os.environ["PYTHONPATH"] + ";C:/daten/install/3d/Appleseed/appleseed/bin"

import appleseed

md = appleseed.Configuration.get_metadata()
fh=open("c:/daten/params.txt", "w")

attributeList = []
def analyzeAttr(attr, indent="\t", name=""):
    attribute = {'name':"", 'type':"int", 'default': 0, 'min': 0, 'max':1, 'options':[], 'help':""}
    attribute['name'] = name
    if "type" in attr.keys():
        if attr.has_key("type"):
            attribute['type'] = attr['type']
            #print indent,"Type", attr['type']
            if attr['type'] == "enum":
                if attr.has_key("options"):
                    #print indent,
                    for option in attr['options'].keys():
                        #print option+",",
                        attribute['options'].append(option)
                    #print ""
                
        if attr.has_key("default"):
            #print indent,"Default", attr['default']
            attribute['default'] = attr['default']
        if attr.has_key("help"):
            attribute['help'] = attr['help']
            
        attributeList.append(attribute)
    else:
        for newKey, newAttr in attr.iteritems():
            #print indent,"Attr",newKey 
            analyzeAttr(newAttr, indent+"\t", newKey)

for k, m in md.iteritems():
    print "Segment: ", k, m
    #print m
    fh.write(k + " " + str(m) + "\n")
    
    analyzeAttr(m, "\t", k)
    
anames = []    
for attr in attributeList:
    print attr['name'], attr['default']
    anames.append(attr['name'])
    
anames.sort()
for attr in anames:
    print attr    

print "-----------------"    
fh = open(r"H:\UserDatenHaggi\Documents\coding\OpenMaya\src\mayaToAppleseed\src\mtap_common\mtap_renderGlobalsNode.cpp", "r")    
content = fh.readlines()
fh.close()

cppnames = []
for line in content:
    line = line.strip()
    if line.startswith("MObject MayaToAppleseedGlobals::"):
        cppnames.append(line.split("::")[-1][:-1])
cppnames.sort()
for attr in cppnames:
    print attr    

    #for pkey, param in m.iteritems():
    #    print "\t", pkey, param
        #if isinstance(param, type({})):
        #    if param.has_key("type"):
        #        print "ParamType", param['type']
        #else:
        #    print "Simple param", param
#         parameter = param
#         if not isinstance(param, type({})):
#             parameter = m
#         
#         try:
#             print "Type", parameter['type']
#         except:
#             print "Error", parameter

fh.close()