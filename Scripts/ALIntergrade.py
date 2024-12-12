import bpy
import addon_utils
import math

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
def GetStartEndFrame(inBone):
    if inBone is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

    act = inBone.animation_data.action
    
    if act is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

    return act.frame_range[0],act.frame_range[1]

def LeeArmatureBake(arm):
    if arm is None: return
    
    set_active_object(arm.name)
    bpy.data.objects[arm.name].select_set(True)
    ###Bake###########
    startf,endf = GetStartEndFrame(arm)
    startf = math.ceil(startf)
    endf = math.ceil(endf)

    act = arm.animation_data.action
    if not act: return

    # for iAct in bpy.data.actions:
    #     if not self.isArmatureAction(iAct,arm): continue
        #active Armature and Change to POSE MODE


    # for bone in arm.data.bones:
    #     bone.select=True
        #bpy.data.objects[bone.name].select_set(True)
    arm.Anim_Layers[0].disable=True
    #bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'POSE'})

    info = str("Baking Action : {}").format(act.name)
    print (info)
    #Back to Object Mode
    bpy.ops.object.mode_set(mode = 'OBJECT')

# for mod in addon_utils.modules():
#     if mod.bl_info.get('name')== "Animation Layers":
#         print(mod.bl_info.get('name'))


Export,col = GetMassFbxCollection("massexport") #"MassExport"
armature = GetObjectTypes(col,'ARMATURE')
# bpy.context.object.als.operator='NEW'
# bpy.ops.anim.layers_merge_down()
#bpy.context.object.als.blend_types='COMBINE'


#bpy.context.scene.arp_export_name_string = 'Layer'
#bpy.ops.pose.select_all(action='DESELECT')
count=0
for i,arm in enumerate(armature):
    if arm.als.turn_on:
        bpy.ops.object.select_all(action='DESELECT')
        set_active_object(arm.name)
        #bpy.ops.object.mode_set(mode = 'POSE')
        #print(arm.name)
        if i==2:
            # for i,bone in  enumerate(arm.data.bones):
            #     if not bone.hide:
            #         count+=1

            strips = arm.animation_data.nla_tracks[arm.als.layer_index].strips

            for fcu in strips[0].action.fcurves:
                if 'pose.bones' in fcu.data_path:
                    bone = fcu.data_path.split('"')[1]
                    if bone in arm.data.bones:
                        count+=1        
            #bpy.ops.object.mode_set(mode = 'OBJECT')

            #bpy.context.scene.arp_export_actlist_idx = i+1
            #bpy.context.scene.arp_export_actlist[i].action_to_add = bpy.data.actions[nla.name]
            #print(nla.name)
        #set_active_object(arm.name)
        # for i,bone in enumerate(arm.data.bones):
        #     arm.data.bones.active= bone
        #     print('Bone Hide',bone.name)
#         arm.Anim_Layers[0].mute=True
#         #bpy.context.scene.frame_start,bpy.context.scene.frame_end
#         startf,endf = GetStartEndFrame(arm)
#         startf = math.ceil(bpy.context.scene.frame_start)
#         endf = math.ceil(bpy.context.scene.frame_end)
#         info = str('start : {} end {}').format(startf,endf)

#         for i,al in enumerate(arm.Anim_Layers):
#             arm.als.layer_index=0
#             bpy.ops.anim.remove_anim_layer()

#         for act in arm.animation_data.nla_tracks:
#             arm.Anim_Layers[0].delete()
#             arm.animation_data.nla_tracks.remove(act)
#         print(info)
        #print(arm.animation_data.nla_tracks)




print(count)
