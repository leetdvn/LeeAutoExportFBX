import bpy
import math
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
def GetSelections():
    return bpy.context.selected_objects

def ClearSelection(): return bpy.ops.object.select_all(action='DESELECT')

def isObjectType(inObject): return type(inObject) == bpy.types.Object

def isCollection(inObject): return type(inObject) == bpy.types.Collection

def GetObjectsInCollection(collection):
    if collection is None: return []
    return collection.all_objects

def SelectAllObjsInCollection(inCollection):
    if inCollection is None: return

    ClearSelection()

    for obj in GetObjectsInCollection(inCollection):
        try:
            bpy.context.scene.objects[obj.name]
            bpy.data.objects[obj.name].select_set(True)
        except:
            pass

def GetActionsFromBone(inBone):
    if inBone is None: return None

    return inBone.animation_data.action

def GetStartEndFrame(inBone):
    if inBone is None: return

    act = inBone.animation_data.action

    return act.frame_range[0],act.frame_range[1]


def LeeBakeAllActions():
    for a in bpy.data.actions:
        if not a: continue
        firstFrame = 0.0
        lastFrame = 100.0
            # for keyframe in fcu.keyframe_points:
                
            #     x, y = keyframe.co
            #     k = math.ceil(x)

            #     if k &lt;firstFrame:
            #         firstFrame = k
                
            #     if k &gt; lastFrame:
            #         lastFrame = k
        #bpy.ops.nla.bake(frame_start=firstFrame, frame_end=lastFrame, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'POSE'})
        
##################################################################
def LeeBakeFunc(Collection=None):
    if Collection is None: return
    col = bpy.data.collections[Collection]
    listColl = [c for c in col.children if isCollection(c)]
    for obj in listColl:
        armature =[b for b in GetObjectsInCollection(obj) if b.type =='ARMATURE']
        
        for bone in armature:
            #print(bone.name)
            #if bone.name != 'ChopChop_VD_Rig_rig': continue
            #print(bone)
            #act = GetActionsFromBone(bone)
            startf,endf = GetStartEndFrame(bone)
            startf = math.ceil(startf)
            endf = math.ceil(endf)
            bpy.context.scene.objects[bone.name]
            bpy.data.objects[bone.name].select_set(True)

            for a in bpy.data.actions:
                bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'POSE'})
                print ("Action: {}, First frame: {}, Second frame: {}".format(a.name, startf, endf))
####################MASSEXPORT FUNC###########################################
def LeeMassExport():
    Export = "MassExport"
    col = bpy.data.collections[Export]

    ##=================BAKE====================
    LeeBakeFunc(Export)
    for o in col.objects:
        bpy.ops.object.select_all(action='DESELECT')
        bpy.context.scene.objects[o.name]
        bpy.data.objects[o.name].select_set(True)
        expPath = '%1'  + o.name + ".fbx"
        fbx=bpy.ops.export_scene.fbx(filepath=expPath,use_selection=True)
        print("Exported  : " + expPath + "\n")

    ChildCollections  = [c for c in col.children if isCollection(c)]
    if ChildCollections.__len__() <= 0: return

    for child in ChildCollections:
        SelectAllObjsInCollection(child)
        expPath = '%1'  + child.name + ".fbx"
        fbx=bpy.ops.export_scene.fbx(filepath=expPath,
                                    use_selection=True,
                                    use_active_collection=True,
                                    object_types={'ARMATURE','MESH','OTHER'},
                                    use_custom_props=True,
                                    bake_anim_force_startend_keying=False
                                    )
        print("Exported  : " + expPath + "\n")
    #print(ChildCollections)

LeeMassExport()
#LeeBakeFunc("MassExport")


#Loop in all actions
# for a in bpy.data.actions:
    
#     if not a:
#         continue
    
#     bpy.context.active_object.animation_data.action = a
    
#     firstFrame = 9999999
#     lastFrame = -9999999
    
#     #Get the first and last keyframes of the current action
#     for fcu in a.fcurves:
#                 for keyframe in fcu.keyframe_points:
                    
#                     x, y = keyframe.co
#                     k = math.ceil(x)
                    
#                     if k &lt; firstFrame:
#                         firstFrame = k
                    
#                     if k &gt; lastFrame:
#                         lastFrame = k
    
#     #Bake it
    
#     print ("Action: {}, First frame: {}, Second frame: {}".format(a.name, firstFrame, lastFrame))
    