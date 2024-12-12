# self.ExportDir=str('D:/GalaxySS4/Kun_GLX_SS2_Ep11_Shot043_HIJK_F_V3/{prefix}_{fName}_{suffix}')
# self.ScriptDir=str('C:/Users/leepl/AppData/Local/LeeMassFbx/Scripts/{py}')
# self.blProgram =str('C:/Program Files/Blender Foundation/Blender 3.4/blender.exe')
# self.Skeletal = str('AnimOnly')
# self.prefix =str('shot043_hijk_f_v3_')
# self.suffix=str('')
#==============================================================================
# self.ExportDir=str('%1{prefix}_{fName}_{suffix}')
# self.ScriptDir=str('%4{py}')
# self.blProgram =str('%5')
# self.Skeletal = str('%6')
# self.prefix =str('%7')
# self.suffix=str('%8')

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
