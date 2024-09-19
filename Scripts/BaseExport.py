import bpy
import os

class BaseSkeletal():

    def __init__(self):
        pass
        #.__init__(self)

    def BaseSkeletalExport(self,fileoutput):
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

    def set_active_object(self,object_name):
        try:
            bpy.context.view_layer.objects.active = bpy.data.objects.get(object_name)
            bpy.data.objects.get(object_name).select_set(state=1)
        except: pass

    def BaseGetObjectsInCollection(self,collection):
        if collection is None: return []
        return collection.all_objects
    
    def BaseAllObjsInCollection(self,inCollection):
        if inCollection is None: return

        objecsList=bpy.context.selected_objects
        for obj in self.BaseGetObjectsInCollection(inCollection):
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

    def BaseExportBaseSkeletal(self):
        try:
            bpy.ops.object.mode_set(mode = 'OBJECT')
        except: pass
        colls = bpy.data.collections

        arms = [arm for arm in bpy.data.objects if  arm.type =='ARMATURE']
        for col in colls:
            self.BaseAllObjsInCollection(col)

        arm = ""
        if arms.__len__() > 0:
            arm = arms[0]
        elif arms.__len__() < 0:
            return
        expPath = '{fbxpath}.fbx'

        self.BaseSkeletalExport(expPath)
Base =BaseSkeletal()
Base.BaseExportBaseSkeletal()
#Base.LeeMassExport('AutoRigPro')