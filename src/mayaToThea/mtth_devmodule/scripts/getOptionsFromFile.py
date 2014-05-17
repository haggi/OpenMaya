import os
import path

TYPE_DICT = {"setRealParameter" : "float",
             "setStringParameter" : "string",
             "setIntegerParameter" : "int",
             "setBooleanParameter" : "bool",
             "setRgbParameter" : "color",
             "setStringParameterEnum" : "enum"
             }

PARAM_TYPE = {
              "Precision" : "float",
              "unsigned" : "int",
              "unsigned short": "int",
              "Rgb" : "color",
              "std::string": "string",
              "float": "float",
              "bool": "bool",
              "integer": "integer"
              }

class Attribute(object):
    def __init__(self, setCmd):
        self.setCmd = setCmd
        self.atype = None
        self.enumValues = []
        self.default = None
        self.name = None
    
        self.atype = TYPE_DICT[setCmd.split("(")[0].strip()]
        self.name = setCmd.split('"')[1]
        self.paramName = self.name.replace(" ", "#").replace("/", "_").replace(".", "")

class Param(object):
    def __init__(self, paramName, paramType, enumValues=[]):
        self.paramName = paramName
        self.paramType = paramType
        self.enumValues = enumValues
        self.default = None
        
    
def getOptions():
    THEA_VERSION="v1.3.05.1086"
    optionsDefinitionFile = path.path(__file__).parent.parent.parent / "devkit/TheaSDK_{version}/TheaSDK_{version}/Source/SDK/Integration/sdk.render-options.h".format(version=THEA_VERSION)
    xmlRendDefinitionFile = path.path(__file__).parent.parent.parent / "devkit/TheaSDK_{version}/TheaSDK_{version}/Source/SDK/XML/sdk.xml.render-options.h".format(version=THEA_VERSION)
    enumsFile =             path.path(__file__).parent.parent.parent / "devkit/TheaSDK_{version}/TheaSDK_{version}/Source/SDK/Integration/sdk.basics.h".format(version=THEA_VERSION)

    definitionContent = None
    fh = open(optionsDefinitionFile, "r")
    definitionContent = fh.readlines()
    fh.close()

    xmlContent = None
    fh = open(xmlRendDefinitionFile, "r")
    xmlContent = fh.readlines()
    fh.close()

    enumContent = None
    fh = open(enumsFile, "r")
    enumContent = fh.readlines()
    fh.close()

#     attrArray = []
#     
#     for line in definitionContent:
#         line = line.strip()
#         if line.startswith("void set"):
#             setCmd = line.split("{")[-1].split("}")[0]
#             attrArray.append(Attribute(setCmd))
#     
#     for a in attrArray:
#         print a.paramName

    enumDict = {}
    enumFound = False
    enumName = ""
    enum = []
    for line in enumContent:
        line = line.strip()
        if line.endswith(","):
            line = line[:-1]
        if line.startswith("enum"):
            enumFound = True
            enumName = line.split(" ")[1]
            if not "{" in line:
                continue
        if enumFound and "{" in line and len(line) == 1:
            continue
        if enumFound and "{" in line:
            data = line.split("{")[1]
            data = data.split("}")[0]
            enum.extend(x.strip() for x in data.split(","))
        if enumFound and "}" in line:
            enumDict[enumName] = enum
            enumFound = False
            enum = []
        if enumFound and not "{" in line and not "}" in line:
            enum.extend(x.strip() for x in line.split(","))

    #for key, value in enumDict.iteritems():
    #    print key, value
        
    params = None
    for line in xmlContent:
        line = line.strip()
        if line.startswith("//"):
            continue
        if "};" in line and params is not None:
            break
        if line.startswith("public: // main options."):
            params = ""
        else:
            if params is not None and not line.startswith("public:"):
                line = line.split(";")[0]
                params += line + ";"
     
    paramDict = {}
    params = params.split(";")
    for param in params:
        if len(param) == 0:
            continue
        #print param
        paramType = " ".join(param.split(" ")[:-1])
        paramName = param.split(" ")[-1]
        #print param
        if PARAM_TYPE.has_key(paramType):
            #print "\tparamType", paramType, "name", paramName
            #print "\trealType", PARAM_TYPE[paramType]
            p = Param(paramName, PARAM_TYPE[paramType])
            paramDict[paramName] = p
        else:
            if enumDict.has_key(paramType):
                #print "\trealType: ENUM", enumDict[paramType]
                ed = []
                for ev in enumDict[paramType]:
                    ed.append(ev.split("=")[0])
                p = Param(paramName, "enum", enumValues = ed)
                paramDict[paramName] = p

    constructor = None
    for line in xmlContent:
        line = line.strip()
        if line.startswith("RenderOptions()"):
            constructor = ""
        if constructor is not None:
            constructor += line
        if "{" in line and constructor is not None:
            break
     
    constructor = constructor.replace("{", "")
    constructorElements = constructor.replace("RenderOptions() :", "").split("),")
    for line in constructorElements:
        line = line.strip()
        #print line
        if "seed" in line:
            print "seed 1"
            continue
        name, value = line.split("(")
        value = value.replace('"', "")
        if value.endswith("f"):
            try:
                wert = int(value[-2])
                value = value[:-1]
            except:
                print "nixwert"
                pass
        if value.endswith(")"):
            value = value[:-1]
        value = value.replace(",", ":")
        if paramDict.has_key(name):
            paramDict[name].default = value
            
    #fh = open("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToThea/vs2010/sourceCodeDocs/globalsNodeAttributes.txt", "w")
    keys = paramDict.keys()
    keys.sort()
    for key in keys:
        p = paramDict[key]
        paramString = "{name},{type},{displayName},{default}".format(name=p.paramName, type=p.paramType, displayName="".join([p.paramName[0].capitalize(),p.paramName[1:]]), default=p.default) 
        if p.paramType == "enum":
            paramString += ",{data}".format(data = ":".join(p.enumValues))
        print paramString
        #fh.write(paramString + "\n")
    #fh.close()
    #renderer, enum, Renderer, 0, sampler:hybridsampler:hybridsppm
        
getOptions()