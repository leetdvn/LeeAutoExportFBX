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


# for mod in addon_utils.modules():
#     if mod.bl_info.get('name')== "Animation Layers":
#         print(mod.bl_info.get('name'))


Export,col = GetMassFbxCollection("massexport") #"MassExport"
armature = GetObjectTypes(col,'ARMATURE')
# bpy.context.object.als.layer_index=0
bpy.context.object.als.operator='MERGE'
bpy.context.object.als.direction='ALL'
bpy.ops.anim.layers_merge_down()
for arm in armature:
    if arm.als.turn_on:
        #bpy.context.object.als.layer_index=0
        print(arm.name,arm.Anim_Layers[arm.als.layer_index])
        print(arm.name,arm.Anim_Layers)
#         #print(bpy.ops.anim.layers_merge_down())
#         arm.als.operator='NEW'
#         arm.als.direction='ALL'
#         print(arm.als.layer_index,arm.als.data_type)
#         print(bpy.ops.anim.layers_merge_down())
# al = bpy.types.Object.als
# for obj in GetSelections():
#     if not obj.als.turn_on:
#         print(obj.als.turn_on)