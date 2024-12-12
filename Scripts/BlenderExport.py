from LeetdMassExport import MassExportFbx
MassFbx = MassExportFbx()
expdir='%1' #1
scriptdir='%4' #4
prefix='%7'#7
suffix='%8' #8
blprogram='%5' #5
skel='%6' #6
leeGeo='%3' #3
blAddon='%2' #2
'''
Requirement Initialize Funtion with String Path and Definations
'''
MassFbx.SetInittDir(expdir,scriptdir)
MassFbx.SetStringFix(prefix,suffix,skel,blprogram)
MassFbx.SetMassGeo(leeGeo)
isDebug=False
MassFbx.LeeMassExport(blAddon,isDebug)
