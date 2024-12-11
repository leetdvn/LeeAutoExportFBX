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

for i,arm in enumerate(armature):
    if arm.als.turn_on:
        set_active_object(arm.name)
        # for i,bone in enumerate(arm.data.bones):
        #     arm.data.bones.active= bone
        #     print('Bone Hide',bone.name)
        arm.Anim_Layers[0].mute=True
        #print()
            # if bone.active: continue
            # bone.active=True
        #try:
        #except: pass
        #arm.Anim_Layers[0].name = "Leetdvn"
        # bpy.ops.anim.layers_merge_down()
        #bpy.context.object.als.layer_index=0
        #info = str("Layer Number : {LayerNum}").format(LayerNum)
        #print(arm.name,arm.Anim_Layers[arm.als.layer_index])
        #print(arm.name,arm.Anim_Layers)
