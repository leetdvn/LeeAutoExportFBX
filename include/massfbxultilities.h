#ifndef MASSFBXULTILITIES_H
#define MASSFBXULTILITIES_H

#include <QObject>
#include <QDir>
#include <QFile>

enum SoftwereType
{
    Maya,
    Blender,
    MayaAndBlender,
    None
};

namespace MassFbxUltilities
{


bool IsValidPath(QString inPaths);

void MakeScript(QString inPath,QString inContent);

QStringList GetFilesInDirectory(const QString inDir);

};

#endif // MASSFBXULTILITIES_H
