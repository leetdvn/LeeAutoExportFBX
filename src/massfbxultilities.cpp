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

QString MassFbxUltilities::GetContentFile(const QString inPath)
{
    QFile melFile(inPath);

    if(!melFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", melFile.errorString());
    }

    QTextStream melStream(&melFile);

    return melStream.readAll();
}

QString MassFbxUltilities::MakeTreeDirectory(const QString inSourceF, const QString inSourceFol,const QString inExpFol)
{
    if(inSourceF =="") return QString();

    QString detach = inSourceF;


    QStringList sPlit = detach.split("/");

    detach = detach.remove(inSourceFol);
    detach = detach.replace(sPlit[sPlit.count()-1],"");

    QStringList SplitF = sPlit[sPlit.count()-1].split(".");

    QString Result = inExpFol + detach + SplitF[0] + "/";

    QDir nDir(Result);

    if(!IsValidPath(Result)){
        //qDebug() << " Final 1: " << Result << Qt::endl;
        nDir.mkpath(Result);
    }

    //qDebug() << "Final 2 : " << detach << Qt::endl;

    return Result;
}

QString MassFbxUltilities::DetachSourceFolder(const QString inSourceFile, const QString inSourceFol, bool includeFile)
{
    if(inSourceFile =="") return QString();

    QString detach = inSourceFile;

    QStringList sPlit = detach.split("/");

    detach = detach.remove(inSourceFol);
    detach = detach.replace(sPlit[sPlit.count()-1],"");

    QStringList SplitF = sPlit[sPlit.count()-1].split(".");

    QString Result = includeFile ?  detach + SplitF[0] + "/" : detach ;

    qDebug() << "Detach : " << Result << Qt::endl;

    return Result;
}

bool MassFbxUltilities::CheckSourceExists(QStringList inList, const QString inData)
{
    if(inList.isEmpty()) return false;

    for(auto f : inList)
        if(f == inData) return true;
    return false;
}
