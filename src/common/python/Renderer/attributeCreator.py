import path
import logging
import shutil

log = logging

START_ID = "automatically created attributes start"
END_ID = "automatically created attributes end"

def makeEnum(att):
    string = "\t{0} = eAttr.create(\"{0}\", \"{0}\", {1}, &stat);\n".format(att[0], att[2])
    for index, v in enumerate(att[3].split(":")):
        string += "\tstat = eAttr.addField( \"{0}\", {1} );\n".format(v, index) 
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeInt(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kInt, {1});\n".format(att[0], att[2])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string
    
def makeFloat(att):
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kFloat, {1});\n".format(att[0], att[2])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeBool(att):
    ba = ["false", "true"]
    string = "\t{0} = nAttr.create(\"{0}\", \"{0}\",  MFnNumericData::kBoolean, {1});\n".format(att[0], ba[int(att[2])])
    string += "\tCHECK_MSTATUS(addAttribute( {0} ));\n\n".format(att[0])
    return string

def makeColor(att):
#    environmentColor = nAttr.createColor("environmentColor", "environmentColor");
#    nAttr.setDefault(0.6f, 0.7f, 0.9f);
#    nAttr.setConnectable(false);
#    CHECK_MSTATUS(addAttribute( environmentColor ));
    string = "\t{0} = nAttr.createColor(\"{0}\", \"{0}\");\n".format(att[0])
    string += "\tnAttr.setDefault({0});\n".format(att[2])
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
                attString = ""
                
                if att[1] == "enum":
                    attString = "\t\tif(!getEnum(MString(\"{1}\"), {0}Globals, this->{1}));\n".format(renderer.lower(), att[0])
                    attString += "\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "int":
                    attString = "\t\tif(!getInt(MString(\"{1}\"), {0}Globals, this->{1}));\n".format(renderer.lower(), att[0])
                    attString += "\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "float":
                    attString = "\t\tif(!getFloat(MString(\"{1}\"), {0}Globals, this->{1}));\n".format(renderer.lower(), att[0])
                    attString += "\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "bool":
                    attString = "\t\tif(!getBool(MString(\"{1}\"), {0}Globals, this->{1}));\n".format(renderer.lower(), att[0])
                    attString += "\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
                if att[1] == "color":
                    attString = "\t\tif(!getColor(MString(\"{1}\"), {0}Globals, this->{1}));\n".format(renderer.lower(), att[0])
                    attString += "\t\tthrow(\"problem reading {0}Globals.{1}\");\n\n".format(renderer.lower(), att[0])
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
                newContent.append(attString)                    
        else:
            if END_ID in value:
                start_id_found = False
            if not start_id_found: 
                newContent.append(value)

    fh = open(fileName + "_t", "w")
    fh.writelines(newContent)
    fh.close()

def attributeCreator(renderer, shortCut):
    log.debug("attribute creator for renderer " + renderer)
    
    basePath = path.path("C:/users/haggi/coding/OpenMaya/src/mayaTo" + renderer.capitalize())
    
    attributesFile = basePath + "/vs2010/sourceCodeDocs/globalsNodeAttributes.txt"
    
    globalsNodeCpp = basePath + "/src/" + shortCut + "_common/mtco_renderGlobalsNode.cpp"
    globalsNodeH = basePath + "/src/" + shortCut + "_common/mtco_renderGlobalsNode.h"

    globalsCpp = basePath + "/src/" + shortCut + "_common/mtco_renderGlobals.cpp"
    globalsH = basePath + "/src/" + shortCut + "_common/mtco_renderGlobals.h"
    
    fh = open(attributesFile, "r")
    attributes = fh.readlines()
    fh.close()

    attArray = []
    for att in attributes:
        att = att.strip()
        values = att.split(" ")
        attArray.append(values)

    fillNodeH(globalsNodeH, attArray)
    fillNodeCPP(renderer, globalsNodeCpp, attArray)
    fillH(globalsNodeH, attArray)
    fillCPP(renderer, globalsCpp, attArray)
    
if __name__ == "__main__":
    attributeCreator("corona", "mtco")