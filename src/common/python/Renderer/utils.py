import path
import xml.etree.ElementTree as ET

def luxMatTextFile():
    file = open(r"C:\Users\haggi\coding\OpenMaya\src\mayaToLux\vs2010\sourceCodeDocs\Textures.txt", "r")
    content = file.readlines()
    file.close()
    
    for line in content:
        line = line.strip()
        erg = line.split("::")
        l = line
        if len(erg) > 1:
            l = erg[1]
        l = l.strip()
        erg = l.split()
        pname = ""
        ptype = ""
        
        if len(erg) > 1:
            pname = erg[0]
            ptype = erg[1]
        print line    
        print pname, ptype

def getElements(elementList, child):
    
    elementList[child.attrib['model']] = {}
    for parameter in child:
        #print "\t", parameter.tag, parameter.attrib['name']
        elementList[child.attrib['model']][parameter.attrib['name']] = {}
        for paramElements in parameter:
            if paramElements.attrib['name'] == 'label':
                elementList[child.attrib['model']][parameter.attrib['name']]['label'] =  paramElements.attrib['value']
            #print "\t\t->", paramElements.tag, paramElements.attrib
            if paramElements.attrib['name'] == 'default':
                #print "\t\t\t", paramElements.attrib['name'], paramElements.attrib['value']
                elementList[child.attrib['model']][parameter.attrib['name']]['default'] =  paramElements.attrib['value']
            if paramElements.attrib['name'] == 'type':
                if paramElements.attrib['value'] == 'text':
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'float'
                if paramElements.attrib['value'] == 'file':
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'string'
                if paramElements.attrib['value'] == 'entity':
                    #print "\t\t\t", "Uniform value"
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'message'
                    elementList[child.attrib['model']][parameter.attrib['name']]['default'] =  ""
                if paramElements.attrib['value'] == 'colormap':
                    #print "\t\t\t", "Varying value"                        
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'varying'
                if paramElements.attrib['value'] == 'enumeration':
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'enum'
                if paramElements.attrib['value'] == 'numeric':
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'uniform_scalar'
                if paramElements.attrib['value'] == 'boolean':
                    elementList[child.attrib['model']][parameter.attrib['name']]['type'] =  'boolean'
                    #print "\t\t\t", "Enum type"
                #bsdfs[child.attrib['model']][parameter.attrib['name']]['default'] =  paramElements.attrib['value']
                #print "\t\t\t--", paramElements.attrib['name'], paramElements.attrib['value']
                
            if paramElements.attrib['name'] == 'items':
                # print "Items found"
                items = []
                for et in paramElements:
                    #print "Enum", et.attrib['name'], et.attrib['value']
                    items.append(et.attrib['value'])
                elementList[child.attrib['model']][parameter.attrib['name']]['data'] = items 
    
            else:
                for et in paramElements:
                    if "color" in et.attrib['name']:
                        elementList[child.attrib['model']][parameter.attrib['name']]['type'] +=  '_color'
                        #print "\t\t\t", "Type color"
                        break
                    if "texture_instance" in et.attrib['name']:
                        #print "\t\t\t", "Type scalar"
                        elementList[child.attrib['model']][parameter.attrib['name']]['type'] +=  '_scalar'
            #bsdfs[child.attrib['model']][parameter.attrib['name']] 
    

def getShadersFromXML():
    p = "D:/install/3d/Appleseed/inputMeta.xml"
    #p = "D:/install/3d/Appleseed/inputMetaTst.xml"
    tree = ET.parse(p)
    root = tree.getroot()
    
    bsdfs = {}
    surfaceShaders = {}
    textures = {}
    lights = {}
    edfs = {}
    for child in root:
        if "bsdf" in child.attrib['type']:
            getElements(bsdfs, child)
        if "surface_shader" in child.attrib['type']:
            getElements(surfaceShaders, child)
        if "light" in child.attrib['type']:
            getElements(lights, child)
        if "edf" in child.attrib['type']:
            getElements(edfs, child)
        if "texture" in child.attrib['type']:
            getElements(textures, child)

    fh = open(r"C:\Users\haggi\coding\OpenMaya\src\mayaToAppleseed\vs2010\sourceCodeDocs\Surfaces.txt", "w")    
    for key, value in bsdfs.iteritems():
        fh.write("\n#{0}\n".format(key))
        for paramKey, paramValue in bsdfs[key].iteritems():
            #print paramValue        
            print ",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]),
            fh.write(",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]))
            if paramValue.has_key('data'):
                print "," + ":".join(paramValue['data'])
                fh.write("," + ":".join(paramValue['data']) + "\n")
            else:
                fh.write("\n")
                print
    for key, value in surfaceShaders.iteritems():
        fh.write("\n#{0}\n".format(key))
        for paramKey, paramValue in surfaceShaders[key].iteritems():
            #print paramValue        
            print ",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]),
            fh.write(",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]))
            if paramValue.has_key('data'):
                print "," + ":".join(paramValue['data'])
                fh.write("," + ":".join(paramValue['data']) + "\n")
            else:
                fh.write("\n")
                print
    for key, value in edfs.iteritems():
        fh.write("\n#{0}\n".format(key))
        for paramKey, paramValue in edfs[key].iteritems():
            print paramValue        
            print ",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]),
            fh.write(",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]))
            if paramValue.has_key('data'):
                print "," + ":".join(paramValue['data'])
                fh.write("," + ":".join(paramValue['data']) + "\n")
            else:
                fh.write("\n")
                print
    for key, value in textures.iteritems():
        fh.write("\n#{0}\n".format(key))
        for paramKey, paramValue in textures[key].iteritems():
            print paramValue        
            print ",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]),
            fh.write(",".join([paramKey, paramValue['type'], paramValue['label'], paramValue['default']]))
            if paramValue.has_key('data'):
                print "," + ":".join(paramValue['data'])
                fh.write("," + ":".join(paramValue['data']) + "\n")
            else:
                fh.write("\n")
                print
    fh.close()
    #print bsdfs
    
if __name__ == "__main__":
    getShadersFromXML()
    #luxMatTextFile()
    
    