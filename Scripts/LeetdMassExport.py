import bpy
import math
import addon_utils
from pathlib import Path
import os
from multiprocessing import Process

class MassExportFbx():
    ExportDir=''
    ScriptDir=''
    blProgram =''
    Skeletal =''
    prefix =''
    suffix=''
    MassGeo=''
    isCustomBake=False
    def __init__(self) -> None:
        # self.ExportDir=str
        # self.ScriptDir=str
        # self.blProgram =str
        # self.Skeletal = str
        # self.prefix =str
        # self.suffix=str
        # self.MassGeo=str
        pass

    def SetInittDir(self,inExportDir=str,inScriptDir=str):
        '''
        Init Directories
        '''
        self.ExportDir = inExportDir
        self.ScriptDir = inScriptDir

    def SetStringFix(self,inPrefix=str,inSuffix=str,inSkel=str,inBlenderP=str):
        '''
        Init String Format Requirements
        '''
        self.prefix =str('{}').format(inPrefix)
        self.suffix=str('{}').format(inSuffix)
        self.Skeletal = str('{}').format(inSkel)
        self.blProgram =str('{}').format(inBlenderP)

    def SetMassGeo(self,inMassGeo=str):
        self.MassGeo=inMassGeo

    def GetSelections(self):
        return bpy.context.selected_objects

    def ClearSelection(self): return bpy.ops.object.select_all(action='DESELECT')

    def isObjectType(self,inObject): return type(inObject) == bpy.types.Object

    def isCollection(self,inObject): return type(inObject) == bpy.types.Collection

    def GetObjectsInCollection(self,collection):
        if collection is None: return []
        return [obj for obj in collection.all_objects if obj.visible_get()]

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
        if not inType or not inCollection: return []

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

    def isArmatureAction(self,inAct,inArmature):
        if inAct is None or inArmature is None: return False

        try:
            for layer in inArmature.Anim_Layers:
                if layer.name == inAct.name: 
                    info = str("ALayer : {A} BLayer : {B}").format(A=layer.name,B=inAct.name)
                    print(info)
                    return True
        except:
            return False
        return False

    ##################################################################
    def LeeBakeFunc(self,Collection=None,isTest=False,inIdx=2):
        if Collection is None: return
        col = bpy.data.collections[Collection]
        armature = self.GetObjectTypes(col,'ARMATURE')

        Processes = [Process(target=self.BakeAnimLayer(bone)) for i,bone in enumerate(armature) if bone.als.turn_on]  

        for i,p in enumerate(Processes):
            if isTest:
                if i != inIdx:
                    continue
            p.start()

        for p in Processes: p.join()
   
    def LeeNewBake(self,arm):
        '''
        Bake Handle
        '''
        if arm is None: return

        #timeline get start end 
        startf = math.ceil(bpy.context.scene.frame_start)
        endf = math.ceil(bpy.context.scene.frame_end)
        self.set_active_object(arm.name)
        #select bone of armature
        for bone in arm.data.bones:
            bone.select=True

        #bake nla func
        bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True,use_current_action=True, bake_types={'POSE'})


        for i,al in enumerate(arm.Anim_Layers):
            arm.als.layer_index=0
            bpy.ops.anim.remove_anim_layer()

        for act in arm.animation_data.nla_tracks:
            arm.Anim_Layers[0].delete()
            arm.animation_data.nla_tracks.remove(act)

    def LeeArmatureBake(self,arm):
        if arm is None: return
        
        self.ClearSelection()
        self.set_active_object(arm.name)
        ###Bake###########
        startf,endf = self.GetStartEndFrame(arm)
        startf = math.ceil(startf)
        endf = math.ceil(endf)

        act = arm.animation_data.action
        if not act: return

        #for iAct in bpy.data.actions:
        #     if not self.isArmatureAction(iAct,arm): continue
            #active Armature and Change to POSE MODE
        bpy.ops.object.mode_set(mode = 'POSE')
   
        for bone in arm.data.bones:
            if not bone.hide:
                bone.select=True
            #bpy.data.objects[bone.name].select_set(True)

        bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True,use_current_action=True, bake_types={'POSE'})

        info = str("Baking Action : {}").format(act.name)
        print (info)
        #Back to Object Mode
        bpy.ops.object.mode_set(mode = 'OBJECT')
        #bpy.ops.anim.remove_anim_layer()

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
        #=======================================
        bpy.context.scene.arp_ge_sel_only = True
        #bpy.context.scene.arp_bake_actions = True
        bpy.context.scene.arp_frame_range_type='SCENE'
        #bpy.context.scene.arp_bake_only_active=True
        bpy.context.scene.arp_only_containing=True
        #bpy.context.scene.arp_rename_for_ue=True
        bpy.context.scene.arp_apply_mods = False
        bpy.context.scene.arp_fix_fbx_rot=True
        bpy.context.scene.arp_export_tex=False
        bpy.context.scene.arp_see_actions = True

        #===================================
        # bpy.context.scene.arp_apply_mods = True
        # bpy.context.scene.arp_ge_sel_only = True
        # bpy.context.scene.arp_rename_for_ue=True
        # run export

        try:
            bpy.ops.arp.arp_export_fbx_panel(filepath=fileoutput)
        except:
            bpy.ops.id.arp_export_fbx_panel(filepath=fileoutput)
        finally:
            pass


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
            if not checkloaded:
                try:
                    checkloaded = addon_utils.enable(addon)
                    return checkloaded
                except:
                    pass
                finally:
                    pass
        return checkloaded

    def select_layer_bones(self,arm):
        '''
        Select Bone In Animation Layers ( nla tracks)
        '''
        if not arm.als.turn_on: return

        strips = arm.animation_data.nla_tracks[arm.als.layer_index].strips
        if len(strips) != 1 or strips[0].action is None:
            return
        for fcu in strips[0].action.fcurves:
            if 'pose.bones' in fcu.data_path:
                bone = fcu.data_path.split('"')[1]
                if bone in arm.data.bones:
                    arm.data.bones[bone].select = True

    def InitScriptExpSkeletal(self,scriptLoc=''):
        #dir = self.ScriptDir #os.path.dirname(os.path.abspath(__file__))

        baseScript = str(self.ScriptDir + '{py}')
        SkeBaseScr  = str("{sdir}BaseExport.py").format(sdir=baseScript.format(py=""))

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
                scrPath = str(self.ScriptDir + '{py}').format(py=srcPy)
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
                print("Command  : " ,iCmd ,infoCmd)


    def GetMassFbxCollection(self,inCollection=str):
        '''
        Get MassExport Collection in View_layers
        '''
        collections = bpy.data.collections
        for i,obj in enumerate(collections):
            if str(obj.name).lower() == inCollection: return obj.name,obj
        
        raise Exception("Sorry, No Collection MassExport Check your Animator Memer")
    
    def BlenderExport(self,expPath,isCamera=False):
        if not expPath: return
        Exportypes={}
        if isCamera: Exportypes = {'CAMERA'}
        else: Exportypes={'ARMATURE','MESH','OTHER'}

        bpy.ops.export_scene.fbx(filepath=expPath,
                            use_selection=True,
                            object_types=Exportypes,
                            use_custom_props=True,
                            bake_anim_force_startend_keying=False,
                            use_mesh_modifiers=True
                            )


    def BakeAnimLayer(self,arm):
        if arm is None: return
        #select Layer 0
        self.ClearSelection()
        #Set Select Armature 
        self.set_active_object(arm.name)
        LayerCount=len(arm.Anim_Layers)

        ##Count Layer  > 0
        if LayerCount > 1:
            bpy.ops.object.mode_set(mode = 'POSE')
            #arm.als.layer_index=LayerCount-1
            self.select_layer_bones(arm)
            # for i,bone in enumerate(arm.data.bones): 
            #     if not bone.hide:
            #         bone.select=True
            #bpy.context.object.als.layer_index=LayerCount-1
            bpy.context.object.als.mergefcurves = True
            bpy.context.object.als.baketype = 'AL'
            bpy.context.scene.als.bake_range_type = 'SCENE'
            #arm.als.blend_type='MULTIPLY'
            arm.als.operator='MERGE'
            arm.als.direction='ALL'
            #bpy.ops.anim.layers_merge_down()
            result = None
            try:
                result = bpy.ops.anim.layers_merge_down()
                if 'FINISHED' in result:
                    print(str('merge all Animation {}').format(arm.name))
            except:
                print(str('merge all Animation failures {}').format(arm.name))
                pass
            arm.Anim_Layers.update()
            bpy.ops.object.mode_set(mode = 'OBJECT')
        NewLayerName = str("Bake_{bone}").format(bone=arm.name)
        arm.Anim_Layers[len(arm.Anim_Layers)-1].name = NewLayerName
        bpy.context.scene.arp_export_name_string = NewLayerName
        arm.als.turn_on=False



    def SetActiveAllBoneInAmature(self,arm):
        if arm is None: return


        try:
            for x in range(7):
                arm.data.layers[x]=True
        except: pass

        bpy.ops.object.mode_set(mode = 'POSE')
        bpy.ops.pose.select_all(action='SELECT')
        bpy.ops.pose.bone_layers(layers=(True, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False, False))
        #print(bpy.context.object.data.layers[1])
        bones = [b for b in arm.data.bones]
        for b in bones:
            b.select=True
        bpy.ops.object.mode_set(mode = 'OBJECT')



    def GetAddonLayerName(self,arm):
        if arm is None or not arm.als.turn_on: return
        #arm.Anim_Layers[0].name = str("{bone}_{MassLayer}").format(bone=arm.name,MassLayer="MassLayer")
        arm.als.turn_on=False

    ####################MASSEXPORT FUNC###########################################
    def LeeMassExport(self,Fbx_platform='AutoRigPro',isTesting=False):
        Export,col = self.GetMassFbxCollection("massexport") #"MassExport"

        if col is None :
            raise Exception("Sorry, No Collection MassExport Check your Animator Memer")
        #col = bpy.data.collections[Export]

        #self.ArpIsLoaded()
        #bpy.ops.object.make_local(type='ALL')
        try:
            bpy.ops.object.mode_set(mode = 'OBJECT')
        except: pass

        armature = self.GetObjectTypes(col,'ARMATURE')
        
        cameras = self.GetObjectTypes(col,'CAMERA')

        if not isTesting:
            print('=================EXPORT CAMERA====================')
            for cam in cameras:
                self.set_active_object(cam.name)
                startf,endf = self.GetStartEndFrame(cam)
                print("Frame : ",startf,endf)
                startf = math.ceil(startf)
                endf = math.ceil(endf)
                bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'OBJECT'})
                expPath = str('{dir}{prefix}_{fName}_{suffix}').format(dir=self.ExportDir,fName=cam.name,prefix=self.prefix,suffix=self.suffix) + ".fbx"
                if cam: self.BlenderExport(expPath,True)

            print('=================END EXPORT CAMERA====================')

        if armature.__len__() <= 0: return

        if not self.isCustomBake:
            self.LeeBakeFunc(Export,isTesting,2)
        else: self.LeeArmatureBake()

        
        if isTesting: return
        # self.ArpIsLoaded()
        if self.Skeletal.endswith("BaseSkeleton"):
            self.InitScriptExpSkeletal()

        ##=================BAKE====================
        if self.MassGeo.startswith("On"):
            bpy.ops.object.make_local(type='ALL')

        print('=================EXPORT ARMATURES====================')

        for i,arm in enumerate(armature):
            self.ClearSelection()
            Geos = self.GetAllGeometryAttachedArmature(arm)
            arm.make_local()
            for geo in Geos:
                try:
                    geo.make_local()
                except: pass
                if self.MassGeo.startswith("On"):
                    self.set_active_object(geo.name)


            self.set_active_object(arm.name)
            expPath =  str('{dir}{prefix}_{fName}_{suffix}').format(dir=self.ExportDir,fName=arm.name,prefix=self.prefix,suffix=self.suffix) + ".fbx"
            
            if Fbx_platform=="Blender":
                bpy.ops.export_scene.fbx(filepath=expPath,
                                            use_selection=True,
                                            object_types={'ARMATURE','MESH','OTHER'},
                                            use_custom_props=True,
                                            bake_anim_force_startend_keying=False,
                                            use_mesh_modifiers=False,
                                            use_visible=True
                                            )
                
            elif Fbx_platform=="AutoRigPro":
                for bone in arm.data.bones:
                    bone.select=True

                try:
                    if isTesting: continue
                    self.LeeArpExport(expPath)
                    print("Exported  : " + expPath + "\n")
                    self.ClearSelection()         
                except:
                    print("Issue Export: " + expPath)

        print('=================END EXPORT ARMATURES====================')