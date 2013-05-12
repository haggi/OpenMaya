import path

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
    
if __name__ == "__main__":
    luxMatTextFile()