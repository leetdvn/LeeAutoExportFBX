import bpy
import math
import addon_utils
from pathlib import Path
import os


class MassExportFbx():

    def __init__(self) -> None:
        self.ExportDir=str('%1{fName}')
        self.ScriptDir=str('%4{py}')
        self.blProgram =str('%5')
        self.Skeletal = str('%6')
        pass

    def GetSelections(self):
        return bpy.context.selected_objects

    def ClearSelection(self): return bpy.ops.object.select_all(action='DESELECT')

    def isObjectType(self,inObject): return type(inObject) == bpy.types.Object

    def isCollection(self,inObject): return type(inObject) == bpy.types.Collection

    def GetObjectsInCollection(self,collection):
        if collection is None: return []
        return collection.all_objects

    def OpenBlenderFile(self,path):
        iPath = Path(path)
        if not iPath.is_file(): return
        bpy.ops.wm.open_mainfile(filepath=path)

    def GetAllLibraries(self):
        libraries = bpy.context.blend_data.libraries
        libs = []
        for lib in libraries:
            libs.append(lib)
        return libs

    def set_active_object(self,object_name):
        try:
            bpy.context.view_layer.objects.active = bpy.data.objects.get(object_name)
            bpy.data.objects.get(object_name).select_set(state=1)
        except: pass

    def SelectAllObjsInCollection(self,inCollection):
        if inCollection is None: return

        objecsList=bpy.context.selected_objects
        for obj in self.GetObjectsInCollection(inCollection):
            try:
                if obj.type == 'MESH' or obj.type=='ARMATURE':
                    #bpy.context.scene.objects[obj.name]
                    #bpy.data.objects[obj.name].select_set(True)
                    self.set_active_object(obj.name)
                    if obj.name is not None:
                        objecsList.append(obj.name)
                #print("info : ",obj.type)


            except:
                pass
        return objecsList
    
    def GetObjectTypes(self,inCollection,inType=''):
        if not inType or not inCollection: return

        objects= self.GetObjectsInCollection(inCollection)

        return [obj for obj in objects if obj.type == inType]

    def GetActionsFromBone(self,inBone):
        if inBone is None: return None

        return inBone.animation_data.action

    def GetStartEndFrame(self,inBone):
        if inBone is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

        act = inBone.animation_data.action
        
        if act is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

        return act.frame_range[0],act.frame_range[1]

    def GetAllArmatureInColl(self,inCollectioName=''):
        if not inCollectioName: return []
        col = bpy.data.collections[inCollectioName]

        return self.GetObjectTypes(col,'ARMATURE')
    
    def GetAllGeometryAttachedArmature(self,inARMATURE):
        Geos = []
        if not inARMATURE: return Geos
        
        Geos = [obj for obj in bpy.data.objects if obj.type == 'MESH' and inARMATURE in [m.object for m in obj.modifiers if m.type == 'ARMATURE']]

        print(Geos)

        return Geos

    ##################################################################
    def LeeBakeFunc(self,Collection=None):
        if Collection is None: return
        col = bpy.data.collections[Collection]
        armature = self.GetObjectTypes(col,'ARMATURE')
            
        for bone in armature:
            self.set_active_object(bone.name)
            bpy.ops.object.mode_set(mode = 'POSE')
            startf,endf = self.GetStartEndFrame(bone)
            startf = math.ceil(startf)
            endf = math.ceil(endf)

            #========Loop Action =================
            # for a in bpy.data.actions:
            ###Bake###########
            a = bone.animation_data.action
            if not a: continue
            bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'POSE'})

            print ("Action: {}, First frame: {}, Second frame: {}".format(a.name, startf, endf))

        bpy.ops.object.mode_set(mode = 'OBJECT')

    ##===================AutoRigProInterGrade======================
    def LeeArpExport(self,fileoutput):
        '''
        Auto Rig Pro Fbx Intergrade
        '''
        
        if not fileoutput: return

        # set some settings...
        #scn = bpy.context.scene
        #scn.arp_export_rig_type = 'mped'
        # types: 'humanoid' for humanoid characters, 'mped' for universal skeletons

        try:
            bpy.context.scene.arp_engine_type = 'unreal'
        except:
            bpy.context.scene.arp_engine_type = 'UNREAL'
        # other useful settings
        # scn.arp_keep_bend_bones = True
        # scn.arp_units_x100 = True
        # scn.arp_bake_actions = True
        # scn.arp_export_name_actions = True
        # scn.arp_export_name_string = "test"
        # scn.arp_mesh_smooth_type = 'EDGE'
        # scn.arp_ue_root_motion = True
        # scn.arp_export_noparent = True
        #scn.arp_export_twist = True
        #scn.arp_fix_fbx_matrix=True
        #scn.arp_ge_sel_only =True
        bpy.context.scene.arp_apply_mods = True
        bpy.context.scene.arp_ge_sel_only = True

        # run export
        try:
            bpy.ops.arp.arp_export_fbx_panel(filepath=fileoutput)
        except:
            bpy.ops.id.arp_export_fbx_panel(filepath=fileoutput)


    def ArpIsLoaded(self):
        '''
        check Addon
        '''
        Addons = ['auto_rig_pro-master','Animation_Layers','Multikey']
        addon_utils.modules_refresh()
        BAddons = [mod.__name__ for mod in addon_utils.modules()]

        checkloaded =False
        for addon in Addons:
            if addon not in BAddons: continue
            
            checkloaded = addon_utils.check(addon)[1]
            print("check : " ,checkloaded)
            if not checkloaded:
                try:
                    checkloaded = addon_utils.enable(addon)
                    return checkloaded
                except:
                    pass
                finally:
                    pass
        return checkloaded

    def InitScriptExpSkeletal(self,scriptLoc=''):
        #dir = self.ScriptDir #os.path.dirname(os.path.abspath(__file__))

        SkeBaseScr  = str("{sdir}BaseExport.py").format(sdir=self.ScriptDir.format(py=""))

        #Script Exists check
        #if not os.path.exists(SkeBaseScr): return
        print("Scripts",SkeBaseScr)
        f = open(SkeBaseScr,'r')
        text= f.read()
        f.close()

        Libs= self.MassFbx.GetAllLibraries()
        transientPath= []
        for i,lib in enumerate(Libs):
            scr = bpy.path.display_name(lib.filepath)
            if str(scr).endswith("Rig"):
                srcPy = scr + ".py"
                srcPy = str(srcPy).replace(" ","")
                scrPath = str(self.ScriptDir).format(py=srcPy)
                fbxPath = self.ExportDir.format(fName=str("BaseSkeletal/" +scr))
                f = open(scrPath,'w')
                f.write(text.format(fbxpath=fbxPath))
                f.close()
                fpath = bpy.path.abspath(lib.filepath)
                iCmd = str('\"{blender}\" -b ').format(blender=str(self.blProgram))
                iCmd += str('{file}  -P ').format(file=fpath)
                iCmd += str("{Str} -Y").format(Str=scrPath)
                iCmd = iCmd.replace("\\","/")
                infoCmd = os.system(iCmd)
                print("Exported : " + fpath)
                transientPath.append(scrPath)
                #print("Command  : " ,iCmd ,infoCmd)


    def GetMassFbxCollection(self,inCollection=str):
        collections = bpy.data.collections
        for i,obj in enumerate(collections):
            if str(obj.name).lower() == inCollection: return obj.name,obj
        
    ####################MASSEXPORT FUNC###########################################
    def LeeMassExport(self,Fbx_platform='AutoRigPro'):
        Export,col = self.GetMassFbxCollection("massexport") #"MassExport"
        #col = bpy.data.collections[Export]
        
        #self.ArpIsLoaded()
        # if not self.ArpIsLoaded():
        #     print("AutoRigPro Addons: is Not Found..")
        #     return
        #bpy.ops.object.make_local(type='ALL')
        try:
            bpy.ops.object.mode_set(mode = 'OBJECT')
        except: pass

        armature = self.GetObjectTypes(col,'ARMATURE')

        if armature.__len__() <= 0: return
        
        self.LeeBakeFunc(Export)

        if self.Skeletal.endswith("BaseSkeleton"):
            self.InitScriptExpSkeletal()

        ##=================BAKE====================

        if str("%3").startswith("On"):
            bpy.ops.object.make_local(type='ALL')


        for arm in armature:
            self.ClearSelection()
            Geos = self.GetAllGeometryAttachedArmature(arm)
            arm.make_local()
            for geo in Geos:
                try:
                    geo.make_local()
                except: pass
                if str("%3").startswith("On"):
                    self.set_active_object(geo.name)

            self.set_active_object(arm.name)
            expPath = str(self.ExportDir).format(fName=arm.name) + ".fbx"
            if Fbx_platform=="Blender":
                bpy.ops.export_scene.fbx(filepath=expPath,
                                            use_selection=True,
                                            object_types={'ARMATURE','MESH','OTHER'},
                                            use_custom_props=True,
                                            bake_anim_force_startend_keying=False,
                                            use_mesh_modifiers=True
                                            )
                
            elif Fbx_platform=="AutoRigPro":
                try:
                    self.LeeArpExport(expPath)
                    print("Exported  : " + expPath + "\n")
                except:
                    print("Issue Export: " + expPath)
        #print(ChildCollections)
        # for f in transientPath:
        #     pyf = open(f)
        #     if pyf.closed:
        #         os.remove(f)

fbx_Addon = '%2'
MassFbx = MassExportFbx()
MassFbx.LeeMassExport(fbx_Addon)