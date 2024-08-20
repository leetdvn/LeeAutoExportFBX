#ifndef MASSFBXULTILITIES_H
#define MASSFBXULTILITIES_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <Definitions.h>

enum SoftwereType
{
    Maya,
    Blender,
    MayaAndBlender,
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

};

#endif // MASSFBXULTILITIES_H
