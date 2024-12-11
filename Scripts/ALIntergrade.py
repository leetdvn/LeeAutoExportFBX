import bpy
import addon_utils


def GetMassFbxCollection(inCollection=str):
    collections = bpy.data.collections
    for i,obj in enumerate(collections):
        if str(obj.name).lower() == inCollection: return obj.name,obj

def GetObjectsInCollection(collection):
    if collection is None: return []
    return [obj for obj in collection.all_objects if obj.visible_get()]

def GetObjectTypes(inCollection,inType=''):
    if not inType or not inCollection: return

    objects= GetObjectsInCollection(inCollection)

    return [obj for obj in objects if obj.type == inType]

def GetSelections():
    return bpy.context.selected_objects

def set_active_object(object_name):
    try:
        bpy.context.view_layer.objects.active = bpy.data.objects.get(object_name)
        bpy.data.objects.get(object_name).select_set(state=1)
    except: pass

def SelectAllObjsInCollection(inCollection):
    if inCollection is None: return

    objecsList=bpy.context.selected_objects
    for obj in GetObjectsInCollection(inCollection):
        try:
            if obj.type == 'MESH' or obj.type=='ARMATURE':
                #bpy.context.scene.objects[obj.name]
                #bpy.data.objects[obj.name].select_set(True)
                set_active_object(obj.name)
                if obj.name is not None:
                    objecsList.append(obj.name)
            #print("info : ",obj.type)


        except:
            pass
    return objecsList
# for mod in addon_utils.modules():
#     if mod.bl_info.get('name')== "Animation Layers":
#         print(mod.bl_info.get('name'))


Export,col = GetMassFbxCollection("massexport") #"MassExport"
armature = GetObjectTypes(col,'ARMATURE')
# bpy.context.object.als.operator='NEW'
# bpy.ops.anim.layers_merge_down()
#bpy.context.object.als.blend_types='COMBINE'
bpy.context.scene.arp_bake_only_active=False
bpy.context.scene.arp_only_containing=True

for i,arm in enumerate(armature):
    if arm.als.turn_on:
        set_active_object(arm.name)
        # for i,bone in enumerate(arm.data.bones):
        #     arm.data.bones.active= bone
        #     print('Bone Hide',bone.name)
        print(arm.name)
        bpy.context.object.als.operator='MERGE'
        bpy.context.object.als.direction='ALL'
        result = bpy.ops.anim.layers_merge_down()
        arm.Anim_Layers.update()
        #print(arm.data.animation_data)
        #print(bpy.context.active_object.animation_data.action)
        print(result)
        arm.Anim_Layers[len(arm.Anim_Layers)-1].name = str("Baked_{armName}").format(armName=arm.name)
        bpy.context.scene.arp_export_name_string = str("Baked_{armName}").format(armName=arm.name)

            # if bone.active: continue
            # bone.active=True
        #try:
        #except: pass
        #arm.Anim_Layers[0].name = "Leetdvn"
        # bpy.ops.anim.layers_merge_down()
        #bpy.context.object.als.layer_index=0
        #info = str("Layer Number : {LayerNum}").format(LayerNum)
        for layer in arm.Anim_Layers:
            print(str("Layer :  {LayerName} BoneArm : {Arm} index : {idx}").format(LayerName=layer,Arm=arm.name,idx=arm.als.layer_index))
        #print(arm.name,arm.Anim_Layers[arm.als.layer_index])
        #print(arm.name,arm.Anim_Layers)
#         #print(bpy.ops.anim.layers_merge_down())
#         arm.als.operator='NEW'
#         arm.als.direction='ALL'
#         print(arm.als.layer_index,arm.als.data_type)
#         print(bpy.ops.anim.layers_merge_down())
# al = bpy.types.Object.als
# for obj in GetSelections():
#     if not obj.als.turn_on:
#         print(obj.als.turn_on)
bpy.context.scene.arp_apply_mods = True
bpy.context.scene.arp_ge_sel_only = True
#bpy.context.scene.arp_bake_actions = True
bpy.context.scene.arp_frame_range_type='SCENE'
bpy.context.scene.arp_bake_only_active=False
bpy.context.scene.arp_only_containing=True
#bpy.context.scene.arp_fix_fbx_rot=True
bpy.context.scene.arp_rename_for_ue=True
bpy.context.scene.arp_engine_type = 'unreal'
bpy.context.scene.arp_apply_mods = False
bpy.context.scene.arp_fix_fbx_rot=True
bpy.context.scene.arp_export_tex=False