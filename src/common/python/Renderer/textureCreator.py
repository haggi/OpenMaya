
import path
import logging
import shutil
import os

log = logging

START_ID = "automatically created attributes start"
END_ID = "automatically created attributes end"
START_STATIC_ID = "automatically created static attributes start"
END_STATIC_ID = "automatically created static attributes end"

START_NODE_ID = 0x0011EF5E

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

def aeTemplateCreator(attDict, renderer, shortCut):
    
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
        
    
class Attribute(object):
    def __init__(self, aName, aType, aDisplayName, default, data=None):
        self.name = aName
        self.type = aType
        self.displayName = aDisplayName
        self.default = default
        self.data = data
        print "Attribute", self.name, self.type, self.displayName, self.default, self.data
        print "make func", "make{0}".format(self.type.capitalize())

    def getDefForDefFile(self):
        return "inAtt:{0}:{1}".format(self.name, self.type)
    
    def getDefinition(self):
        return "\tstatic    MObject {0};".format(self.name)
    
    def getAEDefinition(self):
        return '        self.addControl("{0}", label="{1}")'.format(self.name, self.displayName)

    def getStaticDefinition(self):
        return "MObject\tTextureBase::{0};".format(self.name)

    def getImplementation(self):
        methodName = "make" + self.type.capitalize()
        print "Calling", methodName, "for", self.name
        return getattr(Attribute, methodName)(self)

    def makeInt(self):
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(self.name, self.default)
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(self.name)
        return string

    def makeBool(self):
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kBoolean, {1});\n".format(self.name, self.default)
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(self.name)
        return string

    def makeFloat(self):
        string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(self.name, self.default)
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(self.name)
        return string

    def makeColor(self):
        string = "\t{0} = nAttr.createColor(\"{0}\", \"{0}\");\n".format(self.name)
        if len(self.default.split(":")) == 1:
            a = self.default.split(":")[0]
            self.default = "{0}:{0}:{0}".format(a)
        string += "\tnAttr.setDefault({0});\n".format(",".join(self.default.split(":")))
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(self.name)
        return string
    
    def makeVector(self):
        string = "\tMObject {0}X = nAttr.create(\"{0}X\", \"{0}x\", MFnNumericData::kDouble, 0.0);\n".format(self.name)
        string += "\tMObject {0}Y = nAttr.create(\"{0}Y\", \"{0}y\", MFnNumericData::kDouble, 0.0);\n".format(self.name)
        string += "\tMObject {0}Z = nAttr.create(\"{0}Z\", \"{0}z\", MFnNumericData::kDouble, 0.0);\n".format(self.name)
        string += "\t{0} = nAttr.create(\"{0}\", \"{0}\", {0}X, {0}Y, {0}Z);\n".format(self.name)
        if len(self.default.split(":")) == 1:
            a = self.default.split(":")[0]
            self.default = "{0}:{0}:{0}".format(a)
        string += "\tMAKE_INPUT(nAttr);\n"
        if self.type.endswith("Array"):
            string += "\tnAttr.setArray(true);\n"
        string += "\tnAttr.setDefault({0});\n".format(",".join(self.default.split(":")))
        string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(self.name)
        return string

class ShaderNode(object):
    def __init__(self, name):
        self.name = name
        self.attributeList = []
        self.pluginId = 0
        self.path = None
        
class TextureCreator(object):
    def __init__(self, startId, name, shortcut):
        self.pluginStartId = startId
        self.rendererName = name
        self.codingRoot = path.path(__file__).parent.parent.parent.parent
        self.rendererCodingRoot = self.codingRoot + "/mayaTo" + self.rendererName.capitalize() + "/src"
        self.capitalName = self.rendererName.capitalize()
        self.shortCut = shortcut
        self.baseDestination = path.path("{0}/mayaTo{1}".format(self.codingRoot , self.capitalName))
        self.mayaToBaseTexH = "{0}/mayaToBase/src/shaders/textureBase.h".format(self.codingRoot )
        self.mayaToBaseTexCPP = "{0}/mayaToBase/src/shaders/textureBase.cpp".format(self.codingRoot )
        self.rendererMatDefs = path.path("{0}/mayaTo{1}/vs2010/sourceCodeDocs/textures.txt".format(self.codingRoot , self.capitalName))
        self.destinationDir = path.path("{0}/mayaTo{1}/src/textures".format(self.codingRoot , self.capitalName))
        self.nodesToCreate = []
        self.texFileContent = None
        self.textureFileHandle = None
        self.mayaToBaseTexHContent = None
        self.mayaToBaseTexCPPContent = None
        fh = open(self.mayaToBaseTexH)
        self.mayaToBaseTexHContent = fh.readlines()
        fh.close()
        fh = open(self.mayaToBaseTexCPP)
        self.mayaToBaseTexCPPContent = fh.readlines()
        fh.close()        
        self.createTextureFiles()
        self.printIdInfo()
        self.printPluginLoadInfo()
        self.createAETemplates()
        self.printShaderDefinitions()
        
    def printShaderDefinitions(self):
        for node in self.nodesToCreate:
            print "shader_start:{0}".format(node.name)
            for att in node.attributeList:
                print "\t{0}".format(att.getDefForDefFile())
            print "\toutAtt:outColor:color"
            print "shader_end"
#    outatt:displacement:float
    def createAETemplates(self):
        sourceAEFile = self.codingRoot + "/mayaToBase/mt@_devmodule/scripts/@/AETemplate/AE@shaderTemplate.py"
        sourceContent = []
        fh = open(sourceAEFile, "r")
        sourceContent = fh.readlines()
        fh.close()
        
        for node in self.nodesToCreate:
            destAEPath = path.path(self.baseDestination + "/{0}_devmodule/scripts/{1}/AETemplate/AE{1}{2}Template.py".format(self.shortCut, self.rendererName.capitalize(), node.name.capitalize()))
            content = sourceContent
            newContent = []
            if destAEPath.exists():
                fh = open(destAEPath, "r")
                content = fh.readlines()
                fh.close()
            replaceInProgress = False
            for line in content:
                line = line.replace('AE@shaderTemplate', "AE{0}{1}Template".format(self.rendererName.capitalize(), node.name.capitalize()))
                if not replaceInProgress:
                    newContent.append(line)
                if "#autoAddBegin" in line:
                    replaceInProgress = True
                    for att in node.attributeList:
                        newContent.append(att.getAEDefinition() + "\n")
                if "#autoAddEnd" in line:
                    replaceInProgress = False
                    newContent.append(line)
                    
            fh = open(destAEPath, "w")
            fh.writelines(newContent)
            fh.close()
            
            
    def printPluginLoadInfo(self):
        for node in self.nodesToCreate:
            includeString = '#include "textures/{0}.h"'.format(node.name)
            print includeString
        print ""
        for node in self.nodesToCreate:
            regId = 'static const MString {1}Classification("{0}/texture/{0}{1}");'.format(self.rendererName.capitalize(), node.name.capitalize())
            print regId
        print ""
        for node in self.nodesToCreate:
            register = 'CHECK_MSTATUS( plugin.registerNode( "{1}{0}", {0}::id, {0}::creator, {0}::initialize, MPxNode::kDependNode, &{0}Classification ));'.format(node.name.capitalize(), self.rendererName.capitalize())
            print register
        print ""
        for node in self.nodesToCreate:
            deregister = 'CHECK_MSTATUS( plugin.deregisterNode({0}::id));'.format(node.name.capitalize())
            print deregister
        
    def printIdInfo(self):
        #0x0011EF55    0x0011EF55    mayaToLux    wrinkledTexture.cpp    H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToLux\src\textures\wrinkledTexture.cpp
        for node in self.nodesToCreate:
            print node.pluginId, "mayaTo"+self.rendererName.capitalize(), node.name+".cpp", node.path
                
    def createTextureFiles(self):
        self.parseTextureDefinitions()
        self.createCPPFiles()
        
    def createCPPFiles(self):
        for node in self.nodesToCreate:
            self.createHFile(node)
            self.createCPPFile(node)
    
    def createHFile(self, node):
        print "Creating header file for node", node.name
        destinationHeaderFile = self.destinationDir + "/" + node.name + ".h"
        headerFileContent = self.mayaToBaseTexHContent
        if destinationHeaderFile.exists():
            print "Found existing header file"
            fh = open(destinationHeaderFile)
            headerFileContent = fh.readlines()
            fh.close()
            
        newHFileContent = []
        replaceInProgress = False
        for index, line in enumerate(headerFileContent):
            line = line.strip()
            line = line.replace("MAYATO@_TextureBase", "{0}_{1}".format(self.shortCut, node.name))
            line = line.replace("TextureBase", "{0}".format(node.name.capitalize()))
            if not replaceInProgress:
                newHFileContent.append(line)
            if START_ID in line:
                replaceInProgress = True
                for att in node.attributeList:
                    newHFileContent.append(att.getDefinition())
            if replaceInProgress and not END_ID in line:
                continue
            if END_ID in line:
                replaceInProgress = False
                newHFileContent.append(line)
        
        print "Writing file", destinationHeaderFile
        fh = open(destinationHeaderFile, "w")
        for line in newHFileContent:
            fh.write(line + "\n")        
        fh.close()
        
    def createCPPFile(self, node):
        print "Creating cpp file for node", node.name
        self.pluginStartId += 1
        nodeId = "0x%08X" % self.pluginStartId            
        node.pluginId = nodeId
        destinationCppFile = self.destinationDir + "/" + node.name + ".cpp"
        node.path = destinationCppFile
        cppFileContent = self.mayaToBaseTexCPPContent
        if destinationCppFile.exists():
            print "Found existing header file"
            fh = open(destinationCppFile)
            headerFileContent = fh.readlines()
            fh.close()
            
        newCppFileContent = []
        replaceInProgress = False
        for index in range(len(cppFileContent)):
            line = cppFileContent[index]
            #line = line.strip()
            line = line.replace("textureBase", node.name)
            line = line.replace("TextureBase", node.name.capitalize())
            line = line.replace("0x00000", nodeId)

            if not replaceInProgress:
                newCppFileContent.append(line)
            if START_STATIC_ID in line:
                replaceInProgress = True
                for att in node.attributeList:
                    newCppFileContent.append(att.getStaticDefinition().replace("TextureBase", node.name.capitalize()) + "\n")
                while not END_STATIC_ID in line:
                    index += 1
                    line = cppFileContent[index]
                replaceInProgress = False
                
            if START_ID in line:
                replaceInProgress = True
                for att in node.attributeList:
                    newCppFileContent.append(att.getImplementation() + "\n")
            if replaceInProgress and not END_ID in line:
                continue
            if END_ID in line:
                replaceInProgress = False
                newCppFileContent.append(line)
        
        print "Writing file", destinationCppFile
        #print newCppFileContent
        fh = open(destinationCppFile, "w")
        for line in newCppFileContent:
            fh.write(line)
        fh.close()

    def parseTextureDefinitions(self):
        print "Load texture def file", self.rendererMatDefs
        if not self.rendererMatDefs.exists():
            print "File does not exist"
            return
        self.textureFileHandle = open(self.rendererMatDefs, "r")
        self.texFileContent = self.textureFileHandle.readlines()
        self.textureFileHandle.close()
    
        currentTexture = None
        for line in self.texFileContent:
            line = line.strip()
            if len(line) == 0:
                continue
            if line.startswith("//"):
                continue
            if line.startswith("#"):
                if currentTexture is not None:
                    self.nodesToCreate.append(currentTexture)
                currentTexture = ShaderNode(line.replace("#", ""))
                print "Found texture", line.replace("#", "")
                continue
            values = line.split(",")
            values = [a.strip() for a in values]
            if len(values) < 4:
                print "Attribute needs at least 4 values", values
                continue
            data = None
            if len(values) > 4:
                data = ":".join(values[4:])
            att = Attribute(values[0], values[1], values[2], values[3], data)
            currentTexture.attributeList.append(att)
        
        
if __name__ == "__main__":
    tc = TextureCreator(0x0011EF5E, "thea", "mtth")
    print "LastID", "0x%08X" % tc.pluginStartId
    
    #global START_NODE_ID
    #print "ID: --> 0x%08X" % START_NODE_ID