import bpy
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

####################MASSEXPORT FUNC###########################################
def LeeMassExport():
    Export = "MassExport"
    col = bpy.data.collections[Export]
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

# Export = "MassExport"
# col = bpy.data.collections[Export]
# #for o in col.children:
#     #print(o.name)
#     #print(o.objects)
#     #bpy.data.objects[o.name].select_set(True)
#     #for x in o.children:
#         #print(x.objects.data)
# chopchop = col.children[0].objects.data
# chopchop = bpy.data.collections[chopchop.name]

# for o in chopchop.children:
#     print(type(o))
#print(chopchop)
#bpy.ops.outliner.item_activate(deselect_all=True)
#bpy.ops.nla.bake(frame_start=5, frame_end=100, visual_keying=True, clear_constraints=True, clear_parents=True, use_current_action=True, clean_curves=True, bake_types={'POSE'})


##AutoRig Pro FBX EXPORT
# import bpy
# import os

# character_names = [i.name for i in bpy.context.selected_objects]

# def set_active_object(object_name):
#     bpy.context.view_layer.objects.active = bpy.data.objects.get(object_name)
#     bpy.data.objects.get(object_name).select_set(state=1)

# for char_name in character_names:
#     bpy.ops.object.select_all(action='DESELECT')
#     set_active_object(char_name)
#     # set the file path output here
#     file_output = 'C:/Users/leepl/Documents/Exports/Shot 005_Final_V6/'+char_name+".fbx"
#     # export it
#     bpy.ops.arp.arp_export_fbx_panel(filepath=file_output)

##############ADD-ON CHECKED#########################
# import addon_utils

# addon_name = 'some_addon'

# success = addon_utils.enable('some_addon')
# if success:
#     print("enabled", success.bl_info['name'])
# else:
#     print(addon_name, "is not found")