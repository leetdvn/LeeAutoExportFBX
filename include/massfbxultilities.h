#ifndef MASSFBXULTILITIES_H
#define MASSFBXULTILITIES_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <Definitions.h>
#include <QMessageBox>

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

namespace MassFbxUltilities
{


bool IsValidPath(QString inPaths);

void MakeScript(QString inPath,QString inContent);

QStringList GetFilesInDirectory(const QString inDir);

void AddToLogData(QString inContentLine);

void MakeFile(const QString inPath);

QString GetContentFile(const QString inPath);

QString MakeTreeDirectory(const QString inSourceF, const QString inSourceFol,const QString inExpFol);

QString DetachSourceFolder(const QString inSourceFile,const QString inSourceFol,bool includeFile=false);

bool CheckSourceExists(QStringList inList,const QString inData);

};

#endif // MASSFBXULTILITIES_H
