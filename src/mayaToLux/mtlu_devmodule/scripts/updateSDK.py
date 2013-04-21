import shutil
import sys
import os
import path

binDir = path.path("C:/Users/haggi/coding/lux/windows/Projects/luxrender/x64")
binDirA = path.path("C:/Users/haggi/coding/lux/windows/Projects/luxrays/x64")

binDest = path.path("C:/Users/haggi/coding/OpenMaya/src/mayaToLux/devkit/luxsdk/bin")
libDest = path.path("C:/Users/haggi/coding/OpenMaya/src/mayaToLux/devkit/luxsdk/lib")

shutil.copy(binDir / "Debug/lux.dll", binDest / "Debug/lux.dll")
shutil.copy(binDir / "Release/lux.dll", binDest / "Release/lux.dll")
shutil.copy(binDir / "Release NoOpenCL/lux.dll", binDest / "Release NoOpenCL/lux.dll")

for d in [binDir, binDirA]:
    for l in ["liblux.lib", "lux.lib", "libsgl.lib", "lux.pdb", "luxrays.lib"]:
        try:
            shutil.copy(d / "Debug/" + l, libDest / "Debug/" + l)
            shutil.copy(d / "Release/" + l, libDest / "Release/" + l)
            shutil.copy(d / "Release NoOpenCL/" + l, binDest / "Release NoOpenCL/" + l)
        except:
            pass



