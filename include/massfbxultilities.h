#ifndef MASSFBXULTILITIES_H
#define MASSFBXULTILITIES_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <Definitions.h>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFileSystemModel>


enum SoftwereType
{
    Maya,
    Blender,
    Max3D,
    None
};

enum LogType{
    Log,
    Warning,
    Error,
    Completed
};


enum DataInfo
{
    LOwner,
    LCreated,
    LTimModifier,
    LPCName,
    LSize

};


extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;


namespace MassFbxUltilities
{


bool IsValidPath(QString inPaths);

void MakeScript(QString inPath,QString inContent);

QJsonObject LoadObjectFromFile(const QString infile);

QStringList GetFilesInDirectory(const QString inDir);

void AddToLogData(QString inContentLine);

//Create File Path
void MakeFile(const QString inPath);

//Get Text in file
QString GetContentFile(const QString inPath);

//Make Tree Folder
QString MakeTreeDirectory(const QString inSourceF, const QString inSourceFol,const QString inExpFol,bool isMakeBase=false);

//Detach Source
QString DetachSourceFolder(const QString inSourceFile,const QString inSourceFol,bool includeFile=false);

// Data Exists in StringList
bool CheckSourceExists(QStringList inList,const QString inData);

//get File Owner
QString GetFileOwner(const QString inFile);

QJsonObject GetFileInfo(const QString inFile);

//void Create LogInfo
QStringList GetFileOwners(const QStringList infiles);

//JSon Object Exist

int ExistObject(QJsonArray inArray,QJsonObject inObject,QString inKey);

void GetFilesInDir(const QString inDir,QStringList &OutFiles,QStringList inFilters);

//filter file
QStringList GetFileNameFromDir(const QString inPathDir,QStringList inNames);

QString GetDirectoryFromFile(const QString inFile);

bool ShowMessageConfirmBox();

};

#endif // MASSFBXULTILITIES_H
