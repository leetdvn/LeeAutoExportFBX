#include "massfbxultilities.h"


bool MassFbxUltilities::IsValidPath(QString inPaths)
{
    return QDir(inPaths).exists() ? true :
               QFile(inPaths).exists() ? true : false;
}

void MassFbxUltilities::MakeScript(QString inPath, QString inContent)
{
    QFile ScriptFile(inPath);
    if(ScriptFile.open(QIODevice::ReadWrite)){
        ScriptFile.write(inContent.toLocal8Bit());
        ScriptFile.close();
    }

}

QStringList MassFbxUltilities::GetFilesInDirectory(const QString inDir)
{
    QDir dir(inDir);
    QStringList Files = QStringList();
    if(!dir.exists()) return Files;

    Files=dir.entryList(QDir::Files);
    qDebug() << "file Dir : " << inDir <<  Qt::endl;

    for(auto f : Files){
        QString fPath = inDir + f;
        Files.push_back(fPath);
        qDebug() << "file Name : " << fPath << Qt::endl;
    }
    return Files;
}


void MassFbxUltilities::AddToLogData(QString inContentLine)
{
    QFile fbxlog(MASSFBXLOG);
    inContentLine +="\n";
    if(fbxlog.open(QIODevice::WriteOnly | QIODevice::Append)){
        fbxlog.write(inContentLine.toLocal8Bit());
        fbxlog.close();
    }
}

void MassFbxUltilities::MakeFile(const QString inPath)
{
    QFile infile(inPath);

    if(infile.open(QIODevice::ReadWrite | QIODevice::Truncate))
        infile.close();
}
