
import path
import logging
import shutil
import os

log = logging

START_ID = "automatically created attributes start"
END_ID = "automatically created attributes end"

START_NODE_ID = 0x0011EF57

COMPILER_VERSION = "vs2010"
baseDestPath = None
baseSourcePath = None

CODINGROOT = "H:/userDatenHaggi/documents/coding/"

# an automatic attibute is defined as follows:
# attributeName, type, displayName, defaultValue, options
# e.g.
# samples, int, Shading Samples, 2
# filters, enum, Pixel Filter, 0, Mitchell:Gauss:Triangle
# bgColor, color, Background Color, 0.4:0.5:0.7

def makeEnum(att):
    if not str(att[3]).isdigit():
        att[3] = att[4].split(":").index(att[3])
    string = "\t{0} = eAttr.create(\"{0}\", \"{0}\", {1}, &status);\n".format(att[0], att[3])
    for index, v in enumerate(att[4].split(":")):
        string += "\tstatus = eAttr.addField( \"{0}\", {1} );\n".format(v, index) 
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeInt(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(att[0], att[3])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string
    
def makeFloat(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(att[0], att[3])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeBool(att):
    ba = ["false", "true"]
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kBoolean, {1});\n".format(att[0], att[3])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeString(att):
#    exportXMLFileName = tAttr.create("exportXMLFileName", "exportXMLFileName",  MFnNumericData::kString);
#    tAttr.setUsedAsFilename(true);
#    CHECK_MSTATUS(addAttribute( exportXMLFileName ));
#        
    string = "\t{0} = tAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kString);\n".format(att[0])
    if len(att) > 4:
        if att[4] == "useAsFilename":
            string +="\ttAttr.setUsedAsFilename(true);\n"
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeColor(att):
#    environmentColor = nAttr.createColor("environmentColor", "environmentColor");
#    nAttr.setDefault(0.6f, 0.7f, 0.9f);
#    nAttr.setConnectable(false);
#    CHECK_MSTATUS(addAttribute( environmentColor ));
    string = "\t{0} = nAttr.createColor(\"{0}\", \"{0}\");\n".format(att[0])
    if len(att[3].split(":")) == 1:
        a = att[3].split(":")[0]
        att[3] = "{0}:{0}:{0}".format(a)
    string += "\tnAttr.setDefault({0});\n".format(",".join(att[3].split(":")))
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeMessage(att):
    string = "\t{0} = mAttr.create(\"{0}\", \"{0}\");\n".format(att[0])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string


def pluginLoaders(attDict, renderer):
    print "pluginLoaders"
    
    for key in attDict.keys():
        if key == "all":
            continue    
        print '#include "shaders/{0}Material.h"'.format(key)

    for key in attDict.keys():
        if key == "all":
            continue    
        print 'static const MString {0}sRegistrantId("{0}Plugin");'.format(key)
        print 'static const MString {0}sDrawDBClassification("drawdb/shader/surface/{0}");'.format(key)
        print 'static const MString {0}sFullClassification("{1}/material:shader/surface:" + {0}sDrawDBClassification);'.format(key, renderer.lower())
                
    registerOver = []        
    registerNormal = []        
    for key in attDict.keys():
        if key == "all":
            continue    
        registerNormal.append('CHECK_MSTATUS( plugin.registerNode( "{0}", {0}::id, {0}::creator, {0}::initialize, MPxNode::kDependNode, &{0}sFullClassification ));'.format(key, renderer.lower(), key.capitalize()))
        registerOver.append('CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( {0}sDrawDBClassification, {0}sRegistrantId,{0}Override::creator));'.format(key))

    deregisterOver = []        
    deregisterNormal = []        
    for key in attDict.keys():
        if key == "all":
            continue    
        deregisterNormal.append('CHECK_MSTATUS( plugin.deregisterNode( {0}::id ) );'.format(key))
        deregisterOver.append('CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator({0}sDrawDBClassification, {0}sRegistrantId));'.format(key))
    
    print "#ifdef HAS_OVERRIDE"
    for i in registerOver:
        print "\t"+i
    print "#endif\n"
    for i in registerNormal:
        print "\t"+i

    print "\n#ifdef HAS_OVERRIDE"
    for i in deregisterOver:
        print "\t"+i
    print "#endif\n"
    for i in deregisterNormal:
        print "\t"+i

def createNode(sourceFile, destFile, materialName):
    print "Creating shader file for shader", materialName
    
    global START_NODE_ID
    
    sourceH = open(sourceFile, "r")
    source = sourceH.readlines()
    sourceH.close()
    
    if destFile.endswith(".cpp") and not "materialBaseOverride" in sourceFile:
        START_NODE_ID += 1
    nodeId = "0x%08X" % START_NODE_ID
    
    destH = open(destFile, "w")
    for line in source:
        
        line = line.replace('#include "materialBase.h"', '#include "'+materialName + 'Material.h"')
        line = line.replace("MaterialBase", materialName)
        line = line.replace("materialBase", materialName)
        
        #0x0011CF4D
        
        if line.startswith("MTypeId"):
            line = line.replace("0x00000", nodeId)
        
        destH.write(line)
    destH.close()
    

def insertAttributes(destFileName, attDict, nodeName):    
    destH = open(destFileName, "r")
    content = destH.readlines()
    destH.close()
    
    newContent = []
    newContentI = []          
    isH = destFileName.endswith(".h")
    
    start_index = -1
    end_index = -1
    start_indexI = -1
    end_indexI = -1
    
    for index, value in enumerate(content):
        if START_ID in value:
            print "Start id found"
            start_index = index
            
        if END_ID in value:
            end_index = index
            print "End id found"
            break

    # for .h header files    
    if isH:
        for key, v in attDict.iteritems():
            attString = "\tstatic    MObject " + key + ";\n"
            newContent.append(attString)                    
            #print key, v
    # and for .cpp files        
    else:
        
        for index in range(end_index, len(content)):
            value = content[index]
            #print value
            if START_ID in value:
                print "Start id 2 found", index
                start_indexI = index
                
            if END_ID in value:
                end_indexI = index
                print "End id 2 found", index
                
          
        for key, v in attDict.iteritems():
            attString = "MObject " + nodeName + "::" + key + ";\n"
            newContent.append(attString)
            
        for key, v in attDict.iteritems():
            att = [key]
            att.extend(v)
            print att
            attString = ""
            if att[1] == "enum":
                attString = makeEnum(att)
            if att[1] == "int":
                attString = makeInt(att)
            if att[1] == "float":
                attString = makeFloat(att)
            if att[1] == "varying_float":
                attString = makeFloat(att)
            if att[1] == "uniform_float":
                attString = makeFloat(att)
            if att[1] == "varying_scalar":
                attString = makeFloat(att)
            if att[1] == "uniform_scalar":
                attString = makeFloat(att)
            if att[1] == "bool":
                attString = makeBool(att)
            if att[1] == "color":
                attString = makeColor(att)
            if att[1] == "varying_color":
                attString = makeColor(att)
            if att[1] == "uniform_color":
                attString = makeColor(att)
            if att[1] == "message":
                attString = makeMessage(att)
            if att[1] == "string":
                attString = makeString(att)
            newContentI.append(attString)                    
        
                            
    contentOut = content[:(start_index + 1)]
    contentOut.extend(newContent)

    if isH:
        contentOut.extend(content[end_index:])
    else:
        contentOut.extend(content[end_index:start_indexI+1])
        contentOut.extend(newContentI)
        contentOut.extend(content[end_indexI:])
    
    destH = open(destFileName, "w")
    destH.writelines(contentOut)
    destH.close()

def getPType(att):
    if att == "enum":
        return "ENUMSTRING"
    if att == "int":
        return "INT"
    if att == "float":
        return "FLOAT"
    if att == "bool":
        return "BOOL"
    if att == "color":
        return "COLOR"
    if att == "string":
        return "STRING"

def attrIncludeCreator(attDict, renderer, shortCut):
    global baseDestPath
    
    destAEPath = path.path(baseDestPath + "/src/{0}/{0}ShaderInclude.h".format(renderer.capitalize()))
    fh = open(destAEPath, "r")
    content = fh.readlines()
    fh.close()
    
    startIndex = 0
    endIndex = 0
    for index, line in enumerate(content):
        if "automatically created attributes start" in line:
            print "Start new content"
            startIndex = index
        if "automatically created attributes end" in line:
            print "End new content"
            endIndex = index
    
    allContent = []
    
#    for key in attDict.keys():        
#        if key.lower() == "all":
#            for attKey in attDict[key].keys():
#                attName = attKey
#                attDisplayName = attDict[key][attKey][1]
#                allContent.append('        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName))
    
    newContent = []
    for key in attDict.keys():
        
        if key.lower() == "all":
            continue
        
        attrs = attDict[key]

#class GlassMaterial : public LuxMaterial
#{
#    GlassMaterial(MObject mObject, Instance l): LuxMaterial(mObject, l)
#    {
#        MFnDependencyNode dn(mObject);
#        AttrParam p;
#        p.paramName = "Kd";
#        p.ptype = AttrParam::COLOR;
#        this->params.push_back(p);
#    }
#    virtual void defineParams()
#    {
#        createParamsFromMayaNode(this->shaderParams, this->nodeObject, this->params);
#    }
#
#    virtual void defineShader()
#    {
#        const char *type = "cloth";
#        this->shaderParams->AddString("type", &type);
#        this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));
#    }
#};        
        cls = "class\t{0}Material : public LuxMaterial\n".format(key.capitalize())
        cls += "{\npublic:\n"
        cls += "\t{0}Material(MObject mObject, Instance l) : LuxMaterial(mObject, l)\n".format(key.capitalize())
        cls += "\t" + "{\n"
        cls += "\t\t" + "MFnDependencyNode dn(mObject);\n"
        cls += "\t\t" + "AttrParam p;\n"
        for attKey in attrs.keys():
            cls += "\t\t" + 'p.paramName = "{0}";\n'.format(attKey)
            cls += "\t\t" + 'p.ptype = AttrParam::{0};\n'.format(getPType(attrs[attKey][0]))
            cls += "\t\t" + 'this->params.push_back(p);\n'
            
        cls += "\t" + "}\n"
        cls += "\t" + "virtual void defineParams()\n\t{\n\t\tcreateParamsFromMayaNode();\n\t}\n\n"
        cls += "\t" + "virtual void defineShader()\n\t{\n"
        cls += "\t\t" + 'const char *type = "{0}";\n'.format(key);
        cls += "\t\t" + 'this->shaderParams->AddString("type", &type);\n'
        cls += "\t\t" + 'this->lux->makeNamedMaterial(nodeName.asChar(),  boost::get_pointer(this->shaderParams));\n\t}\n'        
        cls += "\n};\n"
        
        newContent.append(cls)
        #for attKey in attDict[key].keys():
        #    attName = attKey
        #    attDisplayName = attDict[key][attKey][1]
        #    print '        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName)
        #    newContent.append('        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName))
        
    finalContent = []
    finalContent.extend(content[:startIndex+1])   
    finalContent.extend(allContent) 
    finalContent.extend(newContent)    
    finalContent.extend(content[endIndex:])    

    print "Writing data to file", destAEPath
    fh = open(destAEPath, "w")
    content = fh.writelines(finalContent)
    fh.close()
    
        #print finalContent

def aeTemplateCreator(attDict, renderer, shortCut):
    global baseDestPath
    
    sourceAEFile = baseSourcePath + "/mt@_devmodule/scripts/@/AETemplate/AE@shaderTemplate.py"
    destAEPath = path.path(baseDestPath + "/mt@_devmodule/scripts/@/AETemplate/".replace("mt@_", shortCut + "_").replace("@", renderer.capitalize()))
    
    print "Sourcefile", sourceAEFile
    print "Destpath", destAEPath

    allContent = []
    allContent.append('        self.addSeparator()\n')
    for key in attDict.keys():        
        if key.lower() == "all":
            for attKey in attDict[key].keys():
                attName = attKey
                attDisplayName = attDict[key][attKey][1]
                allContent.append('        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName))
    
    for key in attDict.keys():
        newContent = []
        
        aeFileName = "AE" + key + "Template.py"
        destAEFile = path.path(destAEPath + aeFileName)
        #print "create AE for", key, destAEFile
        if destAEFile.exists():
            continue
        if key.lower() == "all":
            continue
        print "Creating AE file", destAEFile

        sourceHandle = open(sourceAEFile, "r")
        content = sourceHandle.readlines()
        sourceHandle.close()
        
        startIndex = 0
        endIndex = 0
        for index, line in enumerate(content):
            if "AE@shaderTemplate" in line:
                content[index] = line.replace("AE@shaderTemplate", "AE" + key + "Template")                
            if "#autoAddBegin" in line:
                print "Start new content"
                startIndex = index
            if "#autoAddEnd" in line:
                print "End new content"
                endIndex = index

        #print "Creating data for", key
        #print attDict[key] 
        for attKey in attDict[key].keys():
            attName = attKey
            attDisplayName = attDict[key][attKey][1]
            #print '        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName)
            newContent.append('        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName))
        
        finalContent = []
        finalContent.extend(content[:startIndex+1])   
        finalContent.extend(newContent)    
        finalContent.extend(allContent) 
        finalContent.extend(content[endIndex:])    
        #print finalContent
        print "Write AETemplate file", destAEFile
        destHandle = open(destAEFile, "w")
        destHandle.writelines(finalContent)
        destHandle.close()
        
def fillNodes(attDict):
    print "Fill node h"
    
    sourceFileNameH = baseSourcePath + "/src/shaders/materialBase.h"
  
    allDict = {}
    for key in attDict.keys():
        if key == "all":
            allDict = attDict[key]
    for key in attDict.keys():
        if key == "all":
            continue
        destFileNameH = path.path(baseDestPath + "/src/shaders/" + key + "Material.h")
        sourceFileNameH = baseSourcePath + "/src/shaders/materialBase.h"
        destFileNameC = path.path(baseDestPath + "/src/shaders/" + key + "Material.cpp")
        sourceFileNameC = baseSourcePath + "/src/shaders/materialBase.cpp"
        
        destFileNameO = path.path(baseDestPath + "/src/shaders/" + key + "MaterialOverride.cpp")
        sourceFileNameO = baseSourcePath + "/src/shaders/materialBaseOverride.cpp"
        destFileNameOH = path.path(baseDestPath + "/src/shaders/" + key + "MaterialOverride.h")
        sourceFileNameOH = baseSourcePath + "/src/shaders/materialBaseOverride.h"

        if not destFileNameOH.exists():
            createNode(sourceFileNameOH, destFileNameOH, key)
        if not destFileNameO.exists():
            createNode(sourceFileNameO, destFileNameO, key)
        
        print "source", sourceFileNameH, "dest", destFileNameH
        print "source", sourceFileNameC, "dest", destFileNameC
        if not destFileNameH.exists():
            createNode(sourceFileNameH, destFileNameH, key)
        if not destFileNameC.exists():
            createNode(sourceFileNameC, destFileNameC, key)

        thisDict = attDict[key]
        for k, v in allDict.iteritems():
            thisDict[k] = v
        insertAttributes(destFileNameH, thisDict, key)
        insertAttributes(destFileNameC, thisDict, key)
    
        

def pyRGCreator(pypath, attArray):
    print "pyRGCreator with file ", pypath
    
    fh = open(pypath, "w")
    fh.write("UIList = []\n")
    
    for att in attArray:
        if att[0].startswith("#"):
            if att[0].endswith("Tab"):
                fh.write("tab = {}\n")
                fh.write("UIList.append(tab)\n")
                tabName = "tab['name'] = '{0}'".format(att[0].replace("#", "").replace("Tab", ""))
                fh.write(tabName + "\n")
                fh.write("tab['entries'] = []\n")
            else:
                if att[0].startswith("#frame"):
                    fh.write('entry = {}\n')
                    fh.write("entry['name'] = 'None'\n")
                    fh.write("entry['type'] = 'frame'\n")
                    fh.write("tab['entries'].append(entry)\n")
                else:
                    fh.write('entry = {}\n')
                    fh.write("entry['name'] = 'None'\n")
                    fh.write("entry['type'] = 'separator'\n")
                    fh.write("tab['entries'].append(entry)\n")
        else:
            fh.write('entry = {}\n')
            fh.write("entry['name'] = '{0}'\n".format(att[0]))
            fh.write("entry['type'] = '{0}'\n".format(att[1]))
            fh.write("entry['displayName'] = '{0}'\n".format(att[2]))
            if len(att) > 3:
                fh.write("entry['default'] = '{0}'\n".format(att[3]))
            if len(att) > 4:
                fh.write("entry['addInfo'] = '{0}'\n".format(att[4]))
            fh.write("tab['entries'].append(entry)\n")
            
    fh.close()

    for att in attArray:
        if att[0].startswith("#"):
            if att[0].endswith("Tab"):
                print att[0]
            else:
                if att[0].startswith("#frame"):
                    print  "with pm.frameLayout(label='{0}', collapsable = True, collapse=False):\n".format(att[0].split(" ")[1])
                    print  "\twith pm.columnLayout(self.rendererName + 'ColumnLayout', adjustableColumn = True, width = 400):\n"
                else:
                    print "pm.separator()\n"
        else:
            if len(att) > 4:
                print "self.addRenderGlobalsUIElement(attName = '{0}', uiType = '{1}', displayName = '{2}', default='{3}', data='{4}', uiDict=uiDict)\n".format(att[0],att[1],att[2],att[3],att[4])
            else:
                print "self.addRenderGlobalsUIElement(attName = '{0}', uiType = '{1}', displayName = '{2}', default='{3}', uiDict=uiDict)\n".format(att[0],att[1],att[2],att[3])

def createShaderDefinitionFile(attDict, renderer, shortcut, append):
    destDefPath = path.path(r"{2}OpenMaya\src\mayaTo{0}\{1}_devmodule\ressources\ShaderDefinitions.txt".format(renderer, shortcut, CODINGROOT))
    fh = open(destDefPath, "r")
    content = fh.readlines()
    fh.close()
    
    startIndex = -1
    endIndex = -1
    oldContent = []
    for index, line in enumerate(content):
        if "automatically created attributes start" in line:
            print "Start new content"
            startIndex = index
        if "automatically created attributes end" in line:
            print "End new content"
            endIndex = index
        if endIndex == -1 and startIndex > 0 and index > startIndex:
            oldContent.append(line)

    if not append:
        oldContent = [] 
                   
    allContent = []
        
    newContent = []
    for key in attDict.keys():
        
        if key.lower() == "all":
            continue
        
        attrs = attDict[key]
        
#        shader_start:lambert
#            inatt:color:color
#            inatt:transparency:color
#            outatt:outColor:color
#        shader_end        
        print key, attrs
        cls = "shader_start:{0}\n".format(key)
        for param in attrs.keys():
            cls += "\tinatt:{0}:{1}\n".format(param, attrs[param][0])
        cls += "\toutatt:{0}:{1}\n".format("outColor", "color")
        cls += "shader_end\n\n"
        newContent.append(cls)
        
    finalContent = []
    finalContent.extend(content[:startIndex+1])   
    finalContent.extend(allContent) 
    finalContent.extend(oldContent)     
    finalContent.extend(newContent)    
    finalContent.extend(content[endIndex:])    

    print "Writing data to file", destDefPath
    fh = open(destDefPath, "w")
    content = fh.writelines(finalContent)
    fh.close()

    
def shaderCreator(renderer, shortCut, mtype):
    log.debug("shaderCreator " + renderer)

    global baseSourcePath
    global baseDestPath
    
    if os.name == 'nt':
        baseDestPath = path.path(CODINGROOT + "OpenMaya/src/mayaTo" + renderer.capitalize())
        baseSourcePath = path.path(CODINGROOT + "OpenMaya/src/mayaToBase")
    else:
        pass
            
    materialsFile = baseDestPath + "/" + COMPILER_VERSION + "/sourceCodeDocs/Materials.txt"
    if mtype == "volumes":
        materialsFile = baseDestPath + "/" + COMPILER_VERSION + "/sourceCodeDocs/volumes.txt"
    
#    baseMaterialDestNodeCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_baseMaterial.cpp"
    baseMaterialSourceNodeCpp = baseSourcePath + "/src/shaders/materialBase.cpp"

#    globalsCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.cpp"
#    globalsH = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.h"

#    pyGlobals = basePath + "/" + shortCut + "_devModule/scripts/renderGlobalsUIInfo.py"

    
    fh = open(materialsFile, "r")
    attributes = fh.readlines()
    fh.close()
    attClean = []
    for a in attributes:
        if not a.startswith("//"):
            attClean.append(a)
    attributes = attClean
    
    attArray = []
    for att in attributes:
        att = att.strip()
        values = att.split(",")
        values = [a.strip() for a in values]
        #print values
        if len(values) > 0:
            if len(values[0]) > 0:
                attArray.append(values)
    
    attrDict = {}
    newDict = None
    for att in attArray:
        if att[0].startswith("#"):
            newDict = {}
            attrDict[att[0].replace("#", "").strip()] = newDict
        else:
            if newDict is not None:
                newDict[att[0]] = att[1:]
    
    fillNodes(attrDict)
    pluginLoaders(attrDict, renderer)    
    #pyRGCreator(pyGlobals, attArray)
    aeTemplateCreator(attrDict, renderer, shortCut)
    #attrIncludeCreator(attrDict, renderer, shortCut)
    createShaderDefinitionFile(attrDict, renderer, shortCut, mtype == "volumes")
    
if __name__ == "__main__":
    shaderCreator("Corona", "mtco", "materials")
    #shaderCreator("Indigo", "mtin", "materials")
    #shaderCreator("Indigo", "mtin", "volumes")
    #shaderCreator("Thea", "mtth", "materials")
    #START_NODE_ID
    print "ID: --> 0x%08X" % START_NODE_ID