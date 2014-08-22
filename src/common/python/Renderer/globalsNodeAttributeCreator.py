import path
import logging
import shutil
import os

log = logging

START_ID = "automatically created attributes start"
END_ID = "automatically created attributes end"

# an automatic attibute is defined as follows:
# attributeName, type, displayName, defaultValue, options
# e.g.
# samples, int, Shading Samples, 2
# filters, enum, Pixel Filter, 0, Mitchell:Gauss:Triangle
# bgColor, color, Background Color, 0.4:0.5:0.7

def makeEnum(att):
    default = att[3]
    if not default.isdigit():
        default = att[4].split(":").index(default)
    string = "\t{0} = eAttr.create(\"{0}\", \"{0}\", {1}, &stat);\n".format(att[0], default)
    for index, v in enumerate(att[4].split(":")):
        string += "\tstat = eAttr.addField( \"{0}\", {1} );\n".format(v, index) 
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeInt(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(att[0], att[3])
    if len(att) > 4:
        if "minmax" in att[4]:
            string += "\tnAttr.setMin({0});\n".format(att[4].split(":")[1])
            string += "\tnAttr.setMax({0});\n".format(att[4].split(":")[2])
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(att[0], att[3])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string
    
def makeFloat(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(att[0], att[3])
    if len(att) > 4:
        if "minmax" in att[4]:
            string += "\tnAttr.setMin({0});\n".format(att[4].split(":")[1])
            string += "\tnAttr.setMax({0});\n".format(att[4].split(":")[2])
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
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeVector(att):
    string  = "\tMObject {0}X = nAttr.create(\"{0}X\", \"{0}X\",  MFnNumericData::kFloat);\n".format(att[0])
    string += "\tMObject {0}Y = nAttr.create(\"{0}Y\", \"{0}Y\",  MFnNumericData::kFloat);\n".format(att[0])
    string += "\tMObject {0}Z = nAttr.create(\"{0}Z\", \"{0}Z\",  MFnNumericData::kFloat);\n".format(att[0])
    string += "\t{0} = nAttr.create(\"{0}\", \"{0}\",  {0}X,  {0}Y,  {0}Z );\n".format(att[0])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeColor(att):
#    environmentColor = nAttr.createColor("environmentColor", "environmentColor");
#    nAttr.setDefault(0.6f, 0.7f, 0.9f);
#    nAttr.setConnectable(false);
#    CHECK_MSTATUS(addAttribute( environmentColor ));
    string = "\t{0} = nAttr.createColor(\"{0}\", \"{0}\");\n".format(att[0])
    string += "\tnAttr.setDefault({0});\n".format(",".join(att[3].split(":")))
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeMessage(att):
    string = "\t{0} = mAttr.create(\"{0}\", \"{0}\");\n".format(att[0])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def fillNodeCPP(renderer, fileName, attArray):
    print "Fill node cpp"
    fh = open(fileName, "r")
    content = fh.readlines()
    fh.close()

    newContent = []
    
    start_id_found = False  
    staticDefToDo = True
    initDefToDo = True
    
    globalsName = "MayaTo" + renderer.capitalize() + "Globals"
    
    for index, value in enumerate(content):
        
        #first search for static defs
        if START_ID in value:
            start_id_found = True
            newContent.append(value + "\n")
            for att in attArray:
                print att
                if att[0].startswith("#"):
                    continue
                #MObject MayaToKrayGlobals::caustics;
                if staticDefToDo:
                    attString = "MObject " + globalsName + "::" + att[0] + ";\n"
                    newContent.append(attString)
                if not staticDefToDo and initDefToDo:
                    attString = ""
                    if att[1] == "enum":
                        attString = makeEnum(att)
                    if att[1] == "int":
                        attString = makeInt(att)
                    if att[1] == "float":
                        attString = makeFloat(att)
                    if att[1] == "bool":
                        attString = makeBool(att)
                    if att[1] == "color":
                        attString = makeColor(att)
                    if att[1] == "message":
                        attString = makeMessage(att)
                    if att[1] == "string":
                        attString = makeString(att)
                    if att[1] == "vector":
                        attString = makeVector(att)
                    newContent.append(attString)                    
            if staticDefToDo:
                staticDefToDo = False
        else:
            if END_ID in value:
                start_id_found = False
            if not start_id_found: 
                newContent.append(value)

    fh = open(fileName, "w")
    fh.writelines(newContent)
    fh.close()


def fillNodeH(fileName, attArray):
    print "Fill node h"
    fh = open(fileName, "r")
    content = fh.readlines()
    fh.close()
    
    newContent = []
    #static      MObject environmentColor;  
    start_id_found = False  
    for index, value in enumerate(content):
        if START_ID in value:
            print "Start id found"
            start_id_found = True
            newContent.append(value)
            for att in attArray:
                if att[0].startswith("#"):
                    continue
                print "AttArray", att
                attString = "static    MObject " + att[0] + ";\n"
                newContent.append(attString)
        else:
            if END_ID in value:
                start_id_found = False
            if not start_id_found: 
                newContent.append(value)

    fh = open(fileName, "w")
    fh.writelines(newContent)
    fh.close()
    
def fillCPP(renderer, fileName, attArray):
    print "Fill cpp"
    fh = open(fileName, "r")
    content = fh.readlines()
    fh.close()
    
    newContent = []
    #static      MObject environmentColor;  
    start_id_found = False  
    for index, value in enumerate(content):
        if START_ID in value:
            start_id_found = True
            newContent.append(value)
            for att in attArray:
                if att[0].startswith("#"):
                    continue
                attString = ""
                
                if att[1] == "enum":
                    attString = "\t\tif(!getEnum(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "int":
                    attString = "\t\tif(!getInt(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "float":
                    attString = "\t\tif(!getFloat(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "bool":
                    attString = "\t\tif(!getBool(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "color":
                    attString = "\t\tif(!getColor(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "string":
                    attString = "\t\tif(!getString(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "vector":
                    attString = "\t\tif(!getVector(MString(\"{1}\"), {0}Globals, this->{1}))\n".format(renderer.capitalize(), att[0])
                    attString += "\t\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                newContent.append(attString)                    
        else:
            if END_ID in value:
                start_id_found = False
            if not start_id_found: 
                newContent.append(value)

    fh = open(fileName, "w")
    fh.writelines(newContent)
    fh.close()
    
def fillH(fileName, attArray):
    print "Fill h"
    fh = open(fileName, "r")
    content = fh.readlines()
    fh.close()
    
    newContent = []
    #static      MObject environmentColor;  
    start_id_found = False  
    for index, value in enumerate(content):
        if START_ID in value:
            start_id_found = True
            newContent.append(value)
            for att in attArray:
                if att[0].startswith("#"):
                    continue
                attString = ""
                if att[1] == "enum":
                    attString = "\tint {0};\n".format(att[0])
                if att[1] == "int":
                    attString = "\tint {0};\n".format(att[0])
                if att[1] == "float":
                    attString = "\tfloat {0};\n".format(att[0])
                if att[1] == "bool":
                    attString = "\tbool {0};\n".format(att[0])
                if att[1] == "color":
                    attString = "\tMColor {0};\n".format(att[0])
                if att[1] == "string":
                    attString = "\tMString {0};\n".format(att[0])
                if att[1] == "vector":
                    attString = "\tMVector {0};\n".format(att[0])
                newContent.append(attString)                    
        else:
            if END_ID in value:
                start_id_found = False
            if not start_id_found: 
                newContent.append(value)

    fh = open(fileName, "w")
    fh.writelines(newContent)
    fh.close()

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
                print "self.addRenderGlobalsUIElement(attName = '{0}', uiType = '{1}', displayName = '{2}', default='{3}', data='{4}', uiDict=uiDict)".format(att[0],att[1],att[2],att[3],att[4])
            elif len(att) > 3:
                print "self.addRenderGlobalsUIElement(attName = '{0}', uiType = '{1}', displayName = '{2}', default='{3}', uiDict=uiDict)".format(att[0],att[1],att[2],att[3])
            else:
                print "self.addRenderGlobalsUIElement(attName = '{0}', uiType = '{1}', displayName = '{2}', default='', uiDict=uiDict)".format(att[0],att[1],att[2])
    
def attributeCreator(renderer, shortCut):
    log.debug("attribute creator for renderer " + renderer)

    basePath = None
    if os.name == 'nt':
        basePath = path.path("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaTo" + renderer.capitalize())
    else:
        pass
    if not basePath:
        print "No base path"
        return
        
    attributesFile = basePath + "/vs2010/sourceCodeDocs/globalsNodeAttributes.txt"
    
    globalsNodeCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobalsNode.cpp"
    globalsNodeH = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobalsNode.h"

    globalsCpp = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.cpp"
    globalsH = basePath + "/src/" + shortCut + "_common/" + shortCut + "_renderGlobals.h"

    pyGlobals = basePath + "/" + shortCut + "_devModule/scripts/renderGlobalsUIInfo.py"

    
    fh = open(attributesFile, "r")
    attributes = fh.readlines()
    fh.close()

    attArray = []
    for att in attributes:
        att = att.strip()
        values = att.split(",")
        values = [a.strip() for a in values]
        print values
        if len(values) > 0:
            if len(values[0]) > 0:
                attArray.append(values)

    fillNodeH(globalsNodeH, attArray)
    fillNodeCPP(renderer, globalsNodeCpp, attArray)
    fillH(globalsH, attArray)
    fillCPP(renderer, globalsCpp, attArray)
    
    pyRGCreator(pyGlobals, attArray)
    
if __name__ == "__main__":
    #attributeCreator("lux", "mtlu")
    #attributeCreator("fuji", "mtfu")
    #attributeCreator("indigo", "mtin")
    attributeCreator("corona", "mtco")
    #attributeCreator("thea", "mtth")
    
    