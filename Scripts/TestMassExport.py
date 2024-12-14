from LeetdMassExport import MassExportFbx
MassFbx = MassExportFbx()
expdir='C:/Users/thang/Documents/Exports/Kun_GLX_SS2_Ep11_Shot043_HIJK_F_V2/' #1 export directory
scriptdir='C:/Users/thang/AppData/Local/LeeMassFbx/Scripts/' #4 location Script directory
prefix='shot043_hijk_f_v21_'#7 name file prefix
suffix='' #8 name file suffix
blprogram='C:/Program Files/Blender Foundation/Blender 3.4/blender.exe' #5 
skel='AnimOnly' #6
leeGeo='' #3
blAddon='AutoRigPro' #2
#requirement Init Directories
'''
Requirement Initialize Funtion with String Path and Definations
'''
Message="================================================MassExporting=================================================="
MassFbx.MassTestModules(Message)
MassFbx.SetInittDir(expdir,scriptdir)
MassFbx.SetStringFix(prefix,suffix,skel,blprogram)
MassFbx.SetMassGeo(leeGeo)

## Set Testing Export Index inside Mass Collection
isDebug=True
if isDebug:
    MassFbx.SetTestExportIdx(4)
## Set Animaion Layer Bake Setting Operator and Bake Direction
## Operator NEW | MERGE 
## Direction UP | DOWN | ALL
##Func MassFbx.SetALBakeDirection('DOWN') ------> default value
##Func MassFbx.SetALBakeOperator('MERGE') ------> default value

#Set Skip Exist File ( When Crash tool or reopen Tool will be Skipp file Exported)
MassFbx.SetSmartExport(False)
MassFbx.LeeMassExport(blAddon,isDebug)