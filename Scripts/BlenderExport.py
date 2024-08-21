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

LeeMassExport()
#Bake Layer Clear Contrains
#bpy.ops.nla.bake(frame_start=112, frame_end=225, visual_keying=True, clear_constraints=True, clear_parents=True, use_current_action=True, clean_curves=True, bake_types={'POSE'})
