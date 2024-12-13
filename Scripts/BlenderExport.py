import sys
libdir = '%9'
if not libdir in sys.path:
    sys.path.append(libdir)
from LeetdMassExport import MassExportFbx
MassFbx = MassExportFbx()
expdir='%1' #1 export directory
scriptdir='%4' #4 location Script directory
prefix='%7'#7 name file prefix
suffix='%8' #8 name file suffix
blprogram='%5' #5 
skel='%6' #6
leeGeo='%3' #3
blAddon='%2' #2

#requirement Init Directories
'''
Requirement Initialize Funtion with String Path and Definations
'''
Message="================================================MassExporting=================================================="
MassFbx.MassTestModules(Message)
MassFbx.SetInittDir(expdir,scriptdir)
MassFbx.SetStringFix(prefix,suffix,skel,blprogram)
MassFbx.SetMassGeo(leeGeo)
isDebug=False
MassFbx.LeeMassExport(blAddon,isDebug)