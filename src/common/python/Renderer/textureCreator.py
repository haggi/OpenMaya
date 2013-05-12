
import path
import logging
import shutil
import os

log = logging

START_ID = "automatically created attributes start"
END_ID = "automatically created attributes end"

START_NODE_ID = 0x0011CF60

COMPILER_VERSION = "vs2010"
baseDestPath = None
baseSourcePath = None

# an automatic attibute is defined as follows:
# attributeName, type, displayName, defaultValue, options
# e.g.
# samples, int, Shading Samples, 2
# filters, enum, Pixel Filter, 0, Mitchell:Gauss:Triangle
# bgColor, color, Background Color, 0.4:0.5:0.7

def makeEnum(att):
    string = "\t{0} = eAttr.create(\"{0}\", \"{0}\", {1}, &status);\n".format(att[0], att[3])
    for index, v in enumerate(att[4].split(":")):
        string += "\tstatus = eAttr.addField( \"{0}\", {1} );\n".format(v, index) 
    string += "\tMAKE_INPUT(eAttr);\n"
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeInt(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(att[0], att[3])
    string += "\tMAKE_INPUT(nAttr);\n"
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string
    
def makeFloat(att):
    if att[0] == "luxOutFloat":
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat);\n".format(att[0])
        string += "\tMAKE_OUTPUT(nAttr);\n"
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    else:
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(att[0], att[3])
        if att[1].endswith("Array"):
            string += "\tnAttr.setArray(true);\n"
        string += "\tMAKE_INPUT(nAttr);\n"
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeFresnel(att):
    if att[0] == "luxOutFresnel":
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat);\n".format(att[0])
        string += "\tMAKE_OUTPUT(nAttr);\n"
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    else:
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(att[0], att[3])
        string += "\tMAKE_INPUT(nAttr);\n"
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeBool(att):
    ba = ["false", "true"]
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kBoolean, {1});\n".format(att[0], att[3])
    string += "\tMAKE_INPUT(nAttr);\n"
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeString(att):
#    exportXMLFileName = tAttr.create("exportXMLFileName", "exportXMLFileName",  MFnNumericData::kString);
#    tAttr.setUsedAsFilename(true);
#    CHECK_MSTATUS(addAttribute( exportXMLFileName ));
#        
    string = "\t{0} = tAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kString);\n".format(att[0])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeColor(att):
#    environmentColor = nAttr.createColor("environmentColor", "environmentColor");
#    nAttr.setDefault(0.6f, 0.7f, 0.9f);
#    nAttr.setConnectable(false);
#    CHECK_MSTATUS(addAttribute( environmentColor ));

    print att
    string = "\t{0} = nAttr.createColor(\"{0}\", \"{0}\");\n".format(att[0])
    if att[0] == "luxOutColor":
        string += "\tMAKE_OUTPUT(nAttr);\n"
    else:
        string += "\tMAKE_INPUT(nAttr);\n"
        if len(att[3].split(":")) == 1:
            a = att[3].split(":")[0]
            att[3] = "{0}:{0}:{0}".format(a)
        string += "\tnAttr.setDefault({0});\n".format(",".join(att[3].split(":")))
        if att[1].endswith("Array"):
            string += "\tnAttr.setArray(true);\n"
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeMessage(att):
    string = "\t{0} = mAttr.create(\"{0}\", \"{0}\");\n".format(att[0])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeVector(att):
    string = "\tMObject {0}X = nAttr.create(\"{0}X\", \"{0}x\", MFnNumericData::kDouble, 0.0);\n".format(att[0])
    string += "\tMObject {0}Y = nAttr.create(\"{0}Y\", \"{0}y\", MFnNumericData::kDouble, 0.0);\n".format(att[0])
    string += "\tMObject {0}Z = nAttr.create(\"{0}Z\", \"{0}z\", MFnNumericData::kDouble, 0.0);\n".format(att[0])
    string += "\t{0} = nAttr.create(\"{0}\", \"{0}\", {0}X, {0}Y, {0}Z);\n".format(att[0])
    if len(att[3].split(":")) == 1:
        a = att[3].split(":")[0]
        att[3] = "{0}:{0}:{0}".format(a)
    string += "\tMAKE_INPUT(nAttr);\n"
    if att[1].endswith("Array"):
        string += "\tnAttr.setArray(true);\n"
    string += "\tnAttr.setDefault({0});\n".format(",".join(att[3].split(":")))
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string


def pluginLoaders(attDict, renderer):
    print "pluginLoaders"
    
    for key in attDict.keys():
        if key == "all":
            continue    
        print '#include "textures/{0}Texture.h"'.format(key)
        
    for key in attDict.keys():
        if key == "all":
            continue    
        print 'static const MString {0}sRegistrantId("{0}Plugin");'.format(key)
        print 'static const MString {0}sDrawDBClassification("drawdb/shader/surface/{0}");'.format(key)
        print 'static const MString {0}sFullClassification("{1}/texture:shader/surface:" + {0}sDrawDBClassification);'.format(key, renderer.lower())
        
    for key in attDict.keys():
        if key == "all":
            continue    
        print 'CHECK_MSTATUS( plugin.registerNode( "{1}{2}", {0}::id, {0}::creator, {0}::initialize, MPxNode::kDependNode, &{0}sFullClassification ));'.format(key, renderer.lower(), key.capitalize())
        #print 'CHECK_MSTATUS( MHWRender::MDrawRegistry::registerSurfaceShadingNodeOverrideCreator( {0}sDrawDBClassification, {0}sRegistrantId,{0}Override::creator));'.format(key)

    for key in attDict.keys():
        if key == "all":
            continue    
        print 'CHECK_MSTATUS( plugin.deregisterNode( {0}::id ) );'.format(key)
        #print 'CHECK_MSTATUS(MHWRender::MDrawRegistry::deregisterSurfaceShadingNodeOverrideCreator({0}sDrawDBClassification, {0}sRegistrantId));'.format(key)
    

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
        line = line.replace("TextureBase", materialName)
        line = line.replace("textureBase", materialName)
        
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

    firstInput = None            
    outputs = []

    # for .h header files    
    if isH:
        for key, v in attDict.iteritems():
            if key == "out":
                for outType in v[0].split(":"):
                    attName = "luxOut" + outType.capitalize()
                    attString = "\tstatic    MObject " + attName + ";\n"
                    newContent.append(attString)                    
            else:
                attString = "\tstatic    MObject " + key + ";\n"
                newContent.append(attString)                    
            print attString
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
            if key == "out":
                for outType in v[0].split(":"):
                    attName = "luxOut" + outType.capitalize()
                    attString = "MObject " + nodeName + "::" + attName + ";\n"
                    newContent.append(attString)                    
            else:
                attString = "MObject " + nodeName + "::" + key + ";\n"
                newContent.append(attString)
            print attString

        for key, v in attDict.iteritems():
            att = [key]
            if key == "out":
                att.extend("color")
                attString = ""
                for outType in v[0].split(":"):
                    att[0] = "luxOut" + outType.capitalize()
                    att[1] = outType
                    if att[1] == "float":
                        attString += makeFloat(att)
                    if att[1] == "color":
                        attString += makeColor(att)
                    if att[1] == "fresnel":
                        attString += makeFresnel(att)
                    outputs.append(outType)
                    #print "Outputs", outputs
            else:
                att.extend(v)
                attString = ""
                if att[1] == "enum":
                    attString = makeEnum(att)
                if att[1] == "int":
                    attString = makeInt(att)
                if att[1] == "float":
                    attString = makeFloat(att)
                if att[1] == "floatArray":
                    attString = makeFloat(att)
                if att[1] == "bool":
                    attString = makeBool(att)
                if att[1] == "color":
                    attString = makeColor(att)
                if att[1] == "colorArray":
                    attString = makeColor(att)
                if att[1] == "message":
                    attString = makeMessage(att)
                if att[1] == "string":
                    attString = makeString(att)
                if att[1] == "vector":
                    attString = makeVector(att)
                if firstInput == None:
                    firstInput = att[0]
            newContentI.append(attString)                    
        
    outputContent = []
    
    if not isH:
        print "NoH", outputs
        for output in outputs:
            outputString = "\tCHECK_MSTATUS ( attributeAffects( {0}, luxOut{1}));\n".format(firstInput, output.capitalize())
            outputContent.append(outputString)
        if firstInput is not None:
            outputString = "\tCHECK_MSTATUS ( attributeAffects( {0}, {1}));\n".format(firstInput, "outColor")
            outputContent.append(outputString)
            
    contentOut = content[:(start_index + 1)]
    contentOut.extend(newContent)

    if isH:
        contentOut.extend(content[end_index:])
    else:
        contentOut.extend(content[end_index:start_indexI+1])
        contentOut.extend(newContentI)
        contentOut.extend(outputContent)
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
    if att == "floatArray":
        return "FLOATARRAY"
    if att == "bool":
        return "BOOL"
    if att == "color":
        return "COLOR"
    if att == "colorArray":
        return "COLORARRAY"
    if att == "string":
        return "STRING"
    if att == "vector":
        return "VECTOR"

def attrIncludeCreator(attDict, renderer, shortCut):
    global baseDestPath
    
    destAEPath = path.path(baseDestPath + "/src/{0}/{0}TextureInclude.h".format(renderer.capitalize()))
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
        cls = "class\t{0}Texture : public LuxTexture\n".format(key.capitalize())
        cls += "{\npublic:\n"
        cls += "\t{0}Texture(MObject mObject, Instance l) : LuxTexture(mObject, l)\n".format(key.capitalize())
        cls += "\t" + "{\n"
        cls += "\t\t" + "textureName = \"{0}\";\n".format(key)        
        cls += "\t\t" + "MFnDependencyNode dn(mObject);\n"
        cls += "\t\t" + "AttrParam p;\n"
        for attKey in attrs.keys():
            if attKey == "out":
                continue
            cls += "\t\t" + 'p.paramName = "{0}";\n'.format(attKey)
            cls += "\t\t" + 'p.ptype = AttrParam::{0};\n'.format(getPType(attrs[attKey][0]))
            cls += "\t\t" + 'this->params.push_back(p);\n'
            
        cls += "\t" + "}\n"
        cls += "\t" + "virtual void defineParams()\n\t{\n\t\tcreateParamsFromMayaNode();\n\t}\n\n"
        #cls += "\t" + "virtual void defineShader()\n\t{\n"
        #cls += "\t\t" + 'this->lux->texture(nodeName.asChar(),  boost::get_pointer(this->shaderParams));\n\t}\n'   
        #cls += "\t" + "\n\t}\n"     
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
                if attKey == "out":
                    continue
                attName = attKey
                attDisplayName = attDict[key][attKey][1]
                allContent.append('        self.addControl("{0}", label="{1}")\n'.format(attName, attDisplayName))
    
    for key in attDict.keys():
        newContent = []
        
        aeFileName = "AE" + renderer.lower() + key.capitalize() + "Template.py"
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
        noColorOut = False
        for attKey in attDict[key].keys():
            if attKey == "out":
                if not "color" in attDict[key][attKey][0]:
                    noColorOut = True
        for index, line in enumerate(content):
            if "AE@shaderTemplate" in line:
                content[index] = line.replace("AE@shaderTemplate", "AE" + renderer.lower() + key.capitalize() + "Template")                
            #if noColorOut:
            #    if "pm.mel.AEswatchDisplay(nodeName)" in line:
            #        content[index] = "#"+line
            if "#autoAddBegin" in line:
                print "Start new content"
                startIndex = index
            if "#autoAddEnd" in line:
                print "End new content"
                endIndex = index

        #print "Creating data for", key
        #print attDict[key] 
        for attKey in attDict[key].keys():
            if attKey == "out":
                continue
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
        destHandle = open(destAEFile, "w")
        destHandle.writelines(finalContent)
        destHandle.close()
        
def fillNodes(attDict):
    
    sourceFileNameH = baseSourcePath + "/src/shaders/textureBase.h"
  
    allDict = {}
    for key in attDict.keys():
        if key == "all2d" or key == "all3d":
            allDict = attDict[key]
    for key in attDict.keys():
        if key == "all":
            continue
        destFileNameH = path.path(baseDestPath + "/src/textures/" + key + "Texture.h")
        sourceFileNameH = baseSourcePath + "/src/shaders/textureBase.h"
        destFileNameC = path.path(baseDestPath + "/src/textures/" + key + "Texture.cpp")
        sourceFileNameC = baseSourcePath + "/src/shaders/textureBase.cpp"
        
#        destFileNameO = path.path(baseDestPath + "/src/shaders/" + key + "MaterialOverride.cpp")
#        sourceFileNameO = baseSourcePath + "/src/shaders/materialBaseOverride.cpp"
#        destFileNameOH = path.path(baseDestPath + "/src/shaders/" + key + "MaterialOverride.h")
#        sourceFileNameOH = baseSourcePath + "/src/shaders/materialBaseOverride.h"

#        if not destFileNameOH.exists():
#            createNode(sourceFileNameOH, destFileNameOH, key)
#        if not destFileNameO.exists():
#            createNode(sourceFileNameO, destFileNameO, key)
        
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
    
    
def textureCreator(renderer, shortCut):

    global baseSourcePath
    global baseDestPath
    
    if os.name == 'nt':
        baseDestPath = path.path("C:/users/haggi/coding/OpenMaya/src/mayaTo" + renderer.capitalize())
        baseSourcePath = path.path("C:/users/haggi/coding/OpenMaya/src/mayaToBase")
    else:
        pass
            
    materialsFile = baseDestPath + "/" + COMPILER_VERSION + "/sourceCodeDocs/Textures.txt"
    
#    baseMaterialDestNodeCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_baseMaterial.cpp"
    baseMaterialSourceNodeCpp = baseSourcePath + "/src/shaders/textureBase.cpp"

#    globalsCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.cpp"
#    globalsH = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.h"

#    pyGlobals = basePath + "/" + shortCut + "_devModule/scripts/renderGlobalsUIInfo.py"

    
    fh = open(materialsFile, "r")
    attributes = fh.readlines()
    fh.close()

    attArray = []
    for att in attributes:
        att = att.strip()
        if att.startswith("//"):
            continue
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
            attrDict[att[0].replace("#", "").strip().split(" ")[0]] = newDict
        else:
            if newDict is not None:
                newDict[att[0]] = att[1:]
    
    print attrDict
    fillNodes(attrDict)
    pluginLoaders(attrDict, renderer)    
    #pyRGCreator(pyGlobals, attArray)
    aeTemplateCreator(attrDict, renderer, shortCut)
    attrIncludeCreator(attrDict, renderer, shortCut)
    
if __name__ == "__main__":
    textureCreator("lux", "mtlu")
    global START_NODE_ID
    print "ID: --> 0x%08X" % START_NODE_ID