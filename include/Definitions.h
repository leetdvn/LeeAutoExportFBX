#pragma once


#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_


#include <QDir>

//Data Dir
#define MASSFBXDIR = "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/"

//Mel Script path
#define MELEXPORTSCRIPT  "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/MayaExportCmd.mel"

//Loger
#define MASSFBXLOG "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/MassFbxLog.txt"

//Blender Python Smart Export
#define BLENDERSMARTEXPORT QDir::currentPath() + QString("/Scripts/") + "BlenderExport.py"

//Blender Python Local
#define BLENDERSINGLEEXPORT "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/BlenderExportFile.py"

//Base Script Dir
#define SCRIPTDIR  QDir::currentPath() + QString("/Scripts/")// "C:/Users/leepl/Documents/GitHub/LeeAutoExportFBX/Scripts/"

#define MASSVERSION "LeeMassFbx Tools  Version 1.02 "

#endif
