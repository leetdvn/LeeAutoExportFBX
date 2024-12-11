import bpy
import sys
import os
import importlib
dir = os.path.dirname(bpy.data.filepath)
if not dir in sys.path:
    sys.path.append('C:/Users/thang/Documents/GitHub/LeeAutoExportFBX/Build/Release/Scripts/libs')
    #print(sys.path)

from BlenderExport import MassExportFbx
importlib.reload(MassExportFbx)
MFBX=MassExportFbx
isDebug=True
fbx_Addon='AutoRigPro'
MFBX.LeeMassExport(fbx_Addon,isDebug)