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
    if inBone is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

    act = inBone.animation_data.action
    
    if act is None: return bpy.context.scene.frame_start,bpy.context.scene.frame_end

    return act.frame_range[0],act.frame_range[1]

##################################################################
def LeeBakeFunc(Collection=None):
    if Collection is None: return
    col = bpy.data.collections[Collection]
    listColl = [c for c in col.children if isCollection(c)]
    bpy.ops.object.mode_set(mode = 'OBJECT')
    for obj in listColl:
        armature =[b for b in GetObjectsInCollection(obj) if b.type =='ARMATURE']
        
        for bone in armature:
            bpy.ops.object.select_all(action='DESELECT')
            startf,endf = GetStartEndFrame(bone)
            startf = math.ceil(startf)
            endf = math.ceil(endf)
            bpy.context.scene.objects[bone.name]
            bpy.data.objects[bone.name].select_set(True)

            #========Loop Action =================
            # for a in bpy.data.actions:
            ###Bake###########
            #bpy.ops.nla.bake(frame_start=startf, frame_end=endf, only_selected=True, visual_keying=True, clear_constraints=True, use_current_action=True, bake_types={'POSE'})
            #     print ("Action: {}, First frame: {}, Second frame: {}".format(a.name, startf, endf))


##===================AutoRigProInterGrade======================
def LeeArpExport(fileoutput):
    '''
    Auto Rig Pro Fbx Intergrade
    '''
    
    if not fileoutput: return

    # set some settings...
    scn = bpy.context.scene
    scn.arp_export_rig_type = 'universal'
    # types: 'humanoid' for humanoid characters, 'mped' for universal skeletons

    scn.arp_engine_type = 'unreal'
    # other useful settings
    # scn.arp_keep_bend_bones = True
    # scn.arp_units_x100 = True
    # scn.arp_bake_actions = True
    # scn.arp_export_name_actions = True
    # scn.arp_export_name_string = "test"
    # scn.arp_mesh_smooth_type = 'EDGE'
    # scn.arp_ue_root_motion = True
    # scn.arp_export_noparent = True
    scn.arp_export_twist = True
    scn.arp_fix_fbx_matrix=True
    scn.arp_ge_sel_only =True

    # run export
    bpy.ops.id.arp_export_fbx_panel(filepath=fileoutput)

def ArpIsLoaded():
    '''
    check Addon
    '''
    arpAddon ='auto_rig_pro'
    return addon_utils.enable(arpAddon)
    
        

####################MASSEXPORT FUNC###########################################
def LeeMassExport(Fbx_platform='AutoRigPro'):
    Export = "MassExport"
    col = bpy.data.collections[Export]

    if not col or not ArpIsLoaded():
        print("AutoRigPro Addons: is Not Found..")
        return

    if Fbx_platform == "%2": Fbx_platform = "AutoRigPro"
    ##=================BAKE====================
    LeeBakeFunc(Export)
    bpy.ops.object.mode_set(mode = 'OBJECT')
    for o in col.objects:
        bpy.ops.object.select_all(action=['DESELECT'])
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
        
        if Fbx_platform=="Blender":
            bpy.ops.export_scene.fbx(filepath=expPath,
                                        use_selection=True,
                                        use_active_collection=True,
                                        object_types={'ARMATURE','MESH','OTHER'},
                                        use_custom_props=True,
                                        bake_anim_force_startend_keying=False
                                        )
            
        elif Fbx_platform=="AutoRigPro":
            LeeArpExport(expPath)
        else:
            print("Fbx Platform Not Found")
        print("Exported  : " + expPath + "\n")
    #print(ChildCollections)

fbx_Addon = '%2'
LeeMassExport(fbx_Addon)


    
