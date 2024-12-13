import bpy
import sys
import os
import importlib
dir = os.path.dirname(bpy.data.filepath)
if not dir in sys.path:
    sys.path.append('./Libs')
from LeetdMassExport import MassExportFbx
MassFbx = MassExportFbx()
expdir='D:/GalaxySS4/Kun_GLX_SS2_Ep11_Shot043_HIJK_F_V3/' #1 export directory
scriptdir='C:/Users/leepl/AppData/Local/LeeMassFbx/Scripts/' #4 location Script directory
prefix='shot043_hijk_f_v3_'#7 name file prefix
suffix='' #8 name file suffix
blprogram='C:/Program Files/Blender Foundation/Blender 3.4/blender.exe' #5 
skel='AnimOnly' #6
leeGeo='' #3
blAddon='AutoRigPro' #2

#requirement Init Directories
'''
Requirement Initialize Funtion with String Path and Definations
'''
MassFbx.SetInittDir(expdir,scriptdir)
MassFbx.SetStringFix(prefix,suffix,skel,blprogram)
MassFbx.SetMassGeo(leeGeo)
isDebug=True
MassFbx.LeeMassExport(blAddon,isDebug)
print("================================================MassExporting==================================================")