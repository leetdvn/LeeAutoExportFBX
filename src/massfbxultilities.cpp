#include "massfbxultilities.h"
#include <Windows.h>


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

QString MassFbxUltilities::MakeTreeDirectory(const QString inSourceF, const QString inSourceFol,const QString inExpFol,bool isMakeBase)
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

    if(isMakeBase)
        if(!IsValidPath(Result + "BaseSkeletal/"))
            nDir.mkpath(Result+"BaseSkeletal/");

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

QStringList MassFbxUltilities::GetFileOwners(const QStringList infiles)
{
    if(infiles.isEmpty())
    {
        qDebug() << "Empty Files " << Qt::endl;
        QStringList();
    }

    QStringList owners;

    for(auto file : infiles){
        QString own = GetFileOwner(file);
        owners.push_back(own);
    }
    return owners;
}

QString MassFbxUltilities::GetFileOwner(const QString inFile)
{
    if(inFile.isEmpty())
    {
        qDebug() << "Empty Files " << Qt::endl;
        return QString();
    }

    QFileInfo fInfo(inFile);

    if(!fInfo.isFile()) return QString();
    return fInfo.owner();
}

QJsonObject MassFbxUltilities::LoadObjectFromFile(const QString infile)
{
    QJsonDocument jdoc;
    QFile jfile(infile);
    if(jfile.exists()){
        if(jfile.open(QIODevice::ReadOnly))
        {
            QString content = jfile.readAll();
            jdoc=QJsonDocument::fromJson(content.toUtf8());
        }
        jfile.close();
    }

    return jdoc.object();
}

QJsonObject MassFbxUltilities::GetFileInfo(const QString inFile)
{

    QJsonObject obj;
    if(inFile.isEmpty())
    {
        qDebug() << "Empty Files " << Qt::endl;
        return obj;
    }

    QFileInfo fInfo(inFile);

    if(!fInfo.isFile()) return obj;
    qt_ntfs_permission_lookup++;

    obj["File"] = inFile;
    obj["Owner"] = fInfo.owner();
    obj["Time Created"] = fInfo.birthTime().toString();
    obj["Last Modifier"] = fInfo.lastModified().toString();
    obj["Size"]= QString::number((fInfo.size()/1024),'f',1) + " Kb";
    qt_ntfs_permission_lookup--;
    return obj;
}

int MassFbxUltilities::ExistObject(QJsonArray inArray, QJsonObject inObject, QString inKey)
{
    if(inArray.isEmpty() || inKey.isEmpty() || inObject.isEmpty()) return -1;

    int count=0;
    for(auto js : inArray) {
        auto obj = js.toObject();
        if(obj.value(inKey).toString() == inObject.value(inKey).toString()){
            qDebug() << "JS Arr 1 : " << obj.value(inKey).toString() << "JS inObj 2 : " << inObject.value(inKey).toString() << Qt::endl;
            return count;
        }
        count++;
    }
    return -1;
}

QStringList MassFbxUltilities::GetFileNameFromDir(const QString inPathDir, QStringList inNames)
{
    QDir mDir(inPathDir);
    QStringList files,Results;
    QStringList filters;
    filters << "*.ma" << "*.mb" << "*.blend";
    if(!mDir.exists()) return files;

    GetFilesInDir(inPathDir,files,filters);

    for(int i = 0 ; i < inNames.count() ; ++i){

        for(auto f : files)
        {
            QStringList split = f.split("/");
            if(split[split.count()-1] != inNames[i]) continue;

            qDebug() << "F : " << f << " Name : " << inNames[i] << Qt::endl;
            Results.push_back(f);

        }

    }

    return Results;

}

void MassFbxUltilities::GetFilesInDir(const QString inDir, QStringList &OutFiles, QStringList inFilters)
{
    QDir dir(inDir);
    if(!dir.exists()) return;

    QStringList files = dir.entryList(inFilters);

    QStringList folders = dir.entryList(QDir::Dirs);


    for(auto f : files){
        QString sFile = inDir + f;
        OutFiles.push_back(sFile);
    }

    for(auto fo : folders){
        if(fo.endsWith(".") || fo.endsWith("..")) continue;
        QString dirPath = inDir + fo + "/";
        GetFilesInDir(dirPath,OutFiles,inFilters);
    }

    OutFiles.removeDuplicates();
}

QString MassFbxUltilities::GetDirectoryFromFile(const QString inFile)
{
    if(inFile.isEmpty()) return QString();

    QStringList result = inFile.split("/");

    QString dirpath ;
    int count{};
    for(auto str : result){
        if(str != result[result.count()-1])
            dirpath += str + "/";
        count++;
    }
    return dirpath;
}

bool MassFbxUltilities::ShowMessageConfirmBox()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Lee Confirm Popup");
    msgBox.setText("This mode will be export all Maya And Blender files \n              click yes for export no for cancel");
    QString style = "font: 700 9pt \"Times New Roman\";\n QMessageBox::title{ font: 700 9pt\"Times New Roman\"}";
    msgBox.setStyleSheet(style);
    msgBox.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if(msgBox.exec() == QMessageBox::Yes){
        // do something
        return true;
    }else {
        // do something else
        return false;
    }
    return false;
}

QString MassFbxUltilities::GetBlenderVersion(QString inPath)
{
    if(inPath.isEmpty()) {
        return QString();
    }

    inPath = inPath.replace("\\","/");
    inPath = inPath.replace("Blender.exe","");
    QStringList splitPath = inPath.split("/");

    QString findVer;
    int count{};
    for(auto str : splitPath)
    {
        qDebug() << " LeeInfo : " << str << Qt::endl;
        QStringList dotDetach = str.split(".");
        if(dotDetach.length() > 1){
            QStringList spaceDetach=str.split(" ");
            return spaceDetach[spaceDetach.length()-1];
        }
        count++;
    }
    return QString();
}
