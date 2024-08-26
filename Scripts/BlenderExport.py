import bpy
import math
import addon_utils

#Export Cmd ALL
##bpy.ops.export_scene.fbx(filepath='%1')
#-----------Export Selection
##bpy.ops.export_scene.fbx(filepath='C:/Users/leepl/OneDrive/Desktop/Present_Export/Taoday.fbx',use_selection=True)

#Selection Object
##obj = bpy.context.scene.objects['20B']
##bpy.data.objects[obj.name].select_set(True); 

#Create Cube
# bpy.ops.mesh.primitive_cube_add(size=4,name="taoday")
# cube_obj = bpy.context.active_object
class MassExportFbx():

    def __init__(self) -> None:
        pass

    def GetSelections(self):
        return bpy.context.selected_objects

    def ClearSelection(self): return bpy.ops.object.select_all(action='DESELECT')

    def isObjectType(self,inObject): return type(inObject) == bpy.types.Object

    def isCollection(self,inObject): return type(inObject) == bpy.types.Collection

    def GetObjectsInCollection(self,collection):
        if collection is None: return []
        return collection.all_objects

    def set_active_object(self,object_name):
        try:
            bpy.context.view_layer.objects.active = bpy.data.objects.get(object_name)
            bpy.data.objects.get(object_name).select_set(state=1)
        except: pass

    def SelectAllObjsInCollection(self,inCollection):
        if inCollection is None: return

        self.ClearSelection()
        objecsList = []
        for obj in self.GetObjectsInCollection(inCollection):
            try:
                if obj.type == 'MESH' or obj.type=='ARMATURE':
                    #bpy.context.scene.objects[obj.name]
                    #bpy.data.objects[obj.name].select_set(True)
                    self.set_active_object(obj.name)
                    if obj.name is not None:
                        objecsList.append(obj.name)
                print("info : ",obj.type)


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

        bpy.context.scene.arp_engine_type = 'unreal'
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

        bpy.context.scene.arp_ge_sel_only = True

        # run export
        bpy.ops.id.arp_export_fbx_panel(filepath=fileoutput)

    def ArpIsLoaded(self):
        '''
        check Addon
        '''
        arpAddon ='auto_rig_pro-master'
        return addon_utils.enable(arpAddon)
        
    ####################MASSEXPORT FUNC###########################################
    def LeeMassExport(self,Fbx_platform='AutoRigPro'):
        Export = "MassExport"
        col = bpy.data.collections[Export]

        if not self.ArpIsLoaded():
            print("AutoRigPro Addons: is Not Found..")
            return
        bpy.ops.object.mode_set(mode = 'OBJECT')
        ##=================BAKE====================
        #bpy.ops.object.mode_set(mode = 'OBJECT')
        # for o in col.objects:
        #     self.ClearSelection()
        #     bpy.context.scene.objects[o.name]
        #     bpy.data.objects[o.name].select_set(True)
        #     expPath = 'C:/Users/thang/Documents/Exports/'  + o.name + ".fbx"
        #     bpy.ops.export_scene.fbx(filepath=expPath,use_selection=True)
        #     print("Exported  : " + expPath + "\n")

        #ChildCollections  = [c for c in col.children if self.isCollection(c)]
        #if ChildCollections.__len__() <= 0: return
        armature = self.GetObjectTypes(col,'ARMATURE')
        if armature.__len__() <= 0: return

        self.LeeBakeFunc(Export)

        for arm in armature:
            self.ClearSelection()
            Geos = MassFbx.GetAllGeometryAttachedArmature(arm)
            for geo in Geos:
                self.set_active_object(geo.name)
            self.set_active_object(arm.name)
            expPath = '%1'  + arm.name + ".fbx"
            selects = self.GetSelections()
            print("Sel : ",arm.name,selects)
            if Fbx_platform=="Blender":
                bpy.ops.export_scene.fbx(filepath=expPath,
                                            use_selection=True,
                                            use_active_collection=True,
                                            object_types={'ARMATURE','MESH','OTHER'},
                                            use_custom_props=True,
                                            bake_anim_force_startend_keying=False
                                            )
                
            elif Fbx_platform=="AutoRigPro":
                self.LeeArpExport(expPath)

            else:
                print("Fbx Platform Not Found")
            print("Exported  : " + expPath + "\n")
        #print(ChildCollections)

fbx_Addon = '%2'
MassFbx = MassExportFbx()
MassFbx.LeeMassExport(fbx_Addon)
