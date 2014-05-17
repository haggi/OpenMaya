import os
import path

THEA_VERSION="v1.3.05.1086"

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

class Param(object):
    def __init__(self, paramName, paramType, default = None, enumValues=[]):
        self.paramName = paramName
        self.paramType = paramType
        self.enumValues = enumValues
        self.default = default
    def getParamString(self):
        paramString = "{name},{type},{displayName},{default}".format(name=self.paramName, type=self.paramType, displayName="".join([self.paramName[0].capitalize(),self.paramName[1:]]), default=self.default) 
        if self.paramType == "enum":
            paramString += ",{data}".format(data = ":".join(self.enumValues))
        return paramString
        
def readBSDFs():
    bsdfs = []
    
    basicBSDF="""
        absorptionColor(0,0,0), absorptionDensity(100), sigma(0.0f), roughness(0.1f), anisotropy(0), rotation(0), bump(1.0f), ior(1.5f), kappa(0.0f), microRoughnessWidth(10.0f), microRoughnessHeight(0.25f), microRoughness(false), normalMapping(false), traceReflections(true) {}
    """
    bsdfs.append({'name':"BasicBSDF", 'params':basicBSDF})
    glossyBSDF="""
        roughness(0.1f), transmittedRoughness(0.1f), anisotropy(0), rotation(0), bump(1.0f), ior(1.5f), kappa(0.0f), abbe(50.0f), iorSource(Constant), customTransmittedRoughness(false), normalMapping(false), traceReflections(true), traceRefractions(true), absorptionColor(0,0,0), absorptionDensity(100) {}
    """
    bsdfs.append({'name':"GlossyBSDF", 'params':glossyBSDF})
    bssdfBSDF="""
       absorptionColor(1,1,1), scatteringColor(1,1,1), absorptionDensity(100.0f), scatteringDensity(1000.0f), asymmetry(0.0f), roughness(0.0f), transmittedRoughness(0.0f), anisotropy(0), rotation(0), bump(1.0f), ior(1.3f), customTransmittedRoughness(false), normalMapping(false), traceReflections(true), traceRefractions(true) {}
    """
    bsdfs.append({'name':"BssdfBSDF", 'params':bssdfBSDF})
    thinFilmBSDF="""
        ior(1.52f), thickness(500), bump(1.0f), interference(false), normalMapping(false) {}
    """
    bsdfs.append({'name':"ThinFilmBSDF", 'params':thinFilmBSDF})
    coatingBSDF="""
        roughness(0.1f), anisotropy(0), rotation(0), bump(1.0f), ior(1.5f), kappa(0.0f), thickness(100), microRoughnessWidth(10.0f), microRoughnessHeight(0.25f), microRoughness(false), coatingAbsorption(false), normalMapping(false), traceReflections(true) {}
    """
    bsdfs.append({'name':"CoatingBSDF", 'params':coatingBSDF})
    
    fh = open("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToThea/vs2010/sourceCodeDocs/materials.txt", "a")     

    for bsdf in bsdfs:
        params = []
        for p in bsdf['params'].split("),"):
            if "{}" in p:
                continue
            p = p.strip()
            p = p.replace("unsigned(-1)", "-1")
            paramName = p.split("(")[0]
            paramDefault = p.split("(")[1]
            paramType = None
            if paramDefault in ['true', 'false']:
                paramType = "bool"
            elif paramDefault.endswith("f"):
                paramDefault = paramDefault.replace("f", "")
                paramType = "float"
            elif paramDefault.count(",") == 2:
                paramType = "color"
                paramDefault = paramDefault.replace(",", ":")
            else:
                try:
                    paramDefault = int(paramDefault)
                    paramType = "int"
                except:
                    pass
            params.append(Param(paramName, paramType, default=paramDefault))
         
        fh.write("\n#" + bsdf['name'] + "\n")
        for p in params:
            fh.write(p.getParamString())
            fh.write("\n")
    fh.close()
    
    
def readMaterials():
    materialParams="""
twosided(true), shadowCatcher(false), passiveEmitter(false), ambientEmitter(false), globalEmitter(true), causticEmitter(true), repaintable(false), 
activeDirt(false),dirtLevel(1.0f), ambientLevel(1.0f), perceptualLevel(1.0f), emitterAccuracy(1.0f), emitterMinRays(8), emitterMaxRays(100),
blurredMinSubdivs(unsigned(-1)), blurredMaxSubdivs(unsigned(-1)), tracingDepth(unsigned(-1))
    """
    params = []
    
    for p in materialParams.split(","):
        p = p.strip()
        p = p.replace("unsigned(-1)", "-1")
        paramName = p.split("(")[0]
        paramDefault = p.split("(")[1].split(")")[0]
        paramType = None
        if paramDefault in ['true', 'false']:
            paramType = "bool"
        elif paramDefault.endswith("f"):
            paramType = "float"
        else:
            try:
                paramDefault = int(paramDefault)
                paramType = "int"
            except:
                pass
        params.append(Param(paramName, paramType, default=paramDefault))
    
    fh = open("H:/UserDatenHaggi/Documents/coding/OpenMaya/src/mayaToThea/vs2010/sourceCodeDocs/materials.txt", "w")   
    fh.write("#TheaMaterial\n")  
    for p in params:
        fh.write(p.getParamString())
        fh.write("\n")
    fh.close()
    
readMaterials()
readBSDFs()