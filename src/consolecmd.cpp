#include <consoleCmd.h>

ConsoleCmd::ConsoleCmd(QString inSourceFile,QString inExportDir)
    :MSourcePath(inSourceFile),
    MExportDir(inExportDir)
{
    this->moveToThread(&mThread);
    mThread.start();

   // MProgram =

}

ConsoleCmd::ConsoleCmd(const ConsoleCmd &cCmd)
    :MProgram(cCmd.MProgram),
    MSourcePath(cCmd.MSourcePath),
    MExportDir(cCmd.MExportDir)
{
}

ConsoleCmd::ConsoleCmd(const QString inMaya, const QString inBlend, QString inSourceFile, QString inExportDir)
    :MProgram(inMaya),
    BProgram(inBlend),
    MSourcePath(inSourceFile),
    MExportDir(inExportDir)
{
    // this->moveToThread(&mThread);
    // mThread.start();

}

ConsoleCmd::~ConsoleCmd()
{
    if(mThread.isRunning()){
        mThread.terminate();
        mThread.quit();
    }
}

void ConsoleCmd::SetProgram(const QString inProgram)
{
    QFile fileProgram(inProgram);
    if(!fileProgram.exists()) return;

    if(!inProgram.endsWith("mayabatch.exe"))
        qDebug() << inProgram << "not mayabatch file.." << Qt::endl;

    MProgram = inProgram;


}

void ConsoleCmd::SetMSourcePath(const QString inSourcePath)
{
    QFile souce(inSourcePath);
    if(!souce.exists()){

        qDebug() << MAYALOG << inSourcePath;
    }

    MSourcePath = inSourcePath;
}

void ConsoleCmd::SetMExportDir(const QString inExportDir)
{
    QDir ExpDir(inExportDir);
    if(!ExpDir.exists()){
        qDebug() << MAYACONSOLE << ExpDir;
    }

    MExportDir = inExportDir;

}

void ConsoleCmd::SetSourceDir(const QString inSourceDir)
{
    MSourceDir = inSourceDir;
    DetachDir = DetachSourceFolder(MSourcePath,MSourceDir);
}

QString ConsoleCmd::GetSourceName()
{
    if(MSourcePath.isNull() ||
        MSourcePath.isEmpty() ||
        MSourcePath =="")
    {
        qDebug() << BASECONSOLE << "Source file is empty." << Qt::endl;
        return QString();
    }
    QString PartStr = MSourcePath.replace("\\","/");
    QStringList SplitPath = PartStr.split("/");
    QStringList SourceF = SplitPath[SplitPath.count()-1].split(".");

    QString result = SourceF[0];

    //qDebug() << BASECONSOLE << result << Qt::endl;
    return SourceF[0];
}

bool ConsoleCmd::VerifiedExported()
{
    if(LogPath =="") return false;

    int Total = FilterCompleted(LogPath,ExportResult);

    // for(auto Ex : ExportResult){
    //     qDebug() << "Export Completed : " << Ex << Qt::endl;
    // }

    //qDebug() << "Total Fbx in File : " << Total << Qt::endl;
    return ExportResult.count() > 0 ? true : false;
}

void ConsoleCmd::SetScriptPlatForm()
{
    if(BaseScr.isEmpty() || BaseScr=="") return;

    QString ScriptCont = GetContentFile(BaseScr);
    QString BlKit = property("FbxKit").toString();
    QString FbxOpt = property("FbxOpt").toString();
    QString Mesh = property("Mesh").toString();
    //preifx suffix
    QString prefix = property("prefix").toString();
    QString suffix = property("suffix").toString();

    QString sContent = BaseScr.endsWith(".py") ?
                        ScriptCont.arg(MExportDir,BlKit,Mesh,MASSFBXDIR+ "Scripts/",BProgram,FbxOpt,prefix,suffix):
                        ScriptCont.arg(MExportDir,FbxOpt,Mesh,prefix,suffix);


    QFile Script(ScriptPath);

    if(Script.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        Script.write(sContent.toLocal8Bit());
        Script.close();
    }

}

void ConsoleCmd::LSetProperty(QString key, QVariant invalue)
{
    setProperty(key.toLocal8Bit(),invalue);
}

int ConsoleCmd::FilterCompleted(const QString inLogFile,QStringList &LogResult)
{

    QFile log(LogPath);

    if(!log.exists()) {
        qDebug() << MAYACONSOLE << inLogFile << "file does'nt Exists." <<  Qt::endl;
        return 0;
    }


    QString CompletedCase = mCSoft ==Maya?  "Exported" : "FBX export starting... ";
    if(log.open(QIODevice::ReadOnly)){
        QTextStream textfile(&log);
        while(!textfile.atEnd())
        {
            QString line = textfile.readLine();
            line.replace("\n","");
            line.replace("\r","");
            if(line.startsWith(CompletedCase)){
                QString result = line.replace(CompletedCase + "  : ","");
                result.replace(CompletedCase,"");
                LogResult.push_back(result);
            }
            else if(line.startsWith("MassFbxNumber")){
                QString lineX = line.replace("\n","");
                NumFbx = lineX.right(1).toInt();
                qDebug() << "Member Of Layer : " << NumFbx << Qt::endl;
            }
            if(line.endsWith("MassExport")){
                IsNotFound = true;
            }
            //IsNotFound = line.endsWith("MassExport") ? 1 : 0;
        }
        log.close();

        qDebug() << "Layer Not Found : " << IsNotFound << Qt::endl;

    }

    return NumFbx;
}

QString ConsoleCmd::InitExportScript(const QString inBaseScript)
{
    //Get Content from Base
    QString ScriptContent = GetContentFile(inBaseScript);

    QString sContent = ScriptContent.arg(MExportDir);

    //QString BaseScrDir = SCRIPTDIR + "MayaExportCmd.mel";;
    qDebug() << "SDir : " << MSourceDir << "Result : " << DetachDir << Qt::endl;

    //Write to Location
    QString LocalDir = MASSFBXDIR + "Scripts/" + DetachDir ;
    QString LocalPath = LocalDir  + GetSourceName();

    QDir mDir(LocalDir);

    if(!mDir.exists()){
        mDir.mkpath(LocalDir);
    }

    //transient Dir;
    QStringList split = DetachDir.split("/");
    TransientDir.push_back(MASSFBXDIR + "Scripts/" + split[0] + "/");

    LocalPath += inBaseScript.endsWith(".py") ? ".py" : ".mel";
    //File
    QFile Script(LocalPath);
    if(!Script.exists())
    {
        //qDebug() << "Paht Does not exists" << Qt::endl;
    }

    if(Script.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        Script.write(sContent.toLocal8Bit());
        Script.close();
    }

    return LocalPath;
}

QString ConsoleCmd::MakeConsoleCmd(SoftwereType inType)
{
    if(MProgram.isNull() || MProgram =="") {

        qDebug() << MProgram << "MayaBatch File not found.." << Qt::endl;
        return QString();
    }

    //init Dir
    InitDirectories();

    //Script Export
    QString Mel ;
    //Base Script
    //Program
    QString inProg;

    //Log Maya Console
    LogPath =  MASSLOGDIR +  DetachDir  + GetSourceName() + "Log.txt";

    QString Cmd;

    switch (inType) {
        case Maya:{
            BaseScr = SCRIPTDIR + "MayaExportCmd.mel";
            ScriptPath = InitExportScript(BaseScr);//MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".mel";

            Cmd = MAYACONSOLE.arg(MProgram,MSourcePath,ScriptPath,LogPath);
            break;
        }
        case Blender:{
            BaseScr = SCRIPTDIR + "BlenderExport.py";
            ScriptPath = InitExportScript(BaseScr);//MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".py";

            Cmd = BLENDERCONS.arg(BProgram,MSourcePath,ScriptPath,LogPath);
            qDebug() << "Script : " << LogPath << Qt::endl;
            break;
        }
    }
    mCSoft = inType;

    return Cmd;
}

void ConsoleCmd::InitDirectories()
{
    //init Dir
    QStringList listDir =
        {
            MASSLOGDIR + DetachDir ,
            MASSFBXDIR + "Scripts/" + DetachDir
        };

    for(auto d : listDir){
        QDir mDir(d);
        if(!mDir.exists()){
            mDir.mkpath(d);

        }

        //transient Dir;
        QStringList split = DetachDir.split("/");
        QString transDir = d.remove(DetachDir);
        TransientDir.push_back(transDir + split[0] +"/");
    }
}
//private slots emit Logs
void ConsoleCmd::ExportLogResult()
{
    emit OnLogResult(ExportResult);
}
