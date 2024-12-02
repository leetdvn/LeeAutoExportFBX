#pragma once


#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_


#include <QDir>

//Data Dir
#define MASSFBXDIR QString("C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/")

//Data Dir
#define SKELETALSCR QString("C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/Scripts/BaseExport.py")

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

// Mass Version
#define MASSVERSION "LeeMassFbx Tools Beta Version 1.18"

//Log
#define MASSLOGDIR QString("C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/Logs/")

//Maya Log
#define MAYALOG QString("MayaConsole : ")

//Blender Log
#define BLENDELOG QString("Blender Console : ")

//Base Console Log
#define BASECONSOLE QString("Base Console : ")

//Maya Console commd
#define MAYACONSOLE QString("\"%1\" -file \"%2\" -noAutoloadPlugins -script \"%3\" -log \"%4\"")

//Blender Console Command
#define BLENDERCONS QString("\"%1\" -b \"%2\" -P \"%3\" --log-file \"%4\" -Y")

//Mass Info
#define MASSINFO QString("PC : %1 <br>USERS : %2 <br>DOMAIN : %4")

//Mass Info
#define LEEARTURL QString("https://raw.githubusercontent.com/leetdvn/UEFakeAPIData/refs/heads/main/LeeArtLisences.json")

#endif
