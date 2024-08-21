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
    //Write to Location
    QString LocalPath = MASSFBXDIR + "Scripts/" + GetSourceName();

    //QString BaseScrDir = SCRIPTDIR + "MayaExportCmd.mel";;
    //QString nFol = DetachSourceFolder(MSourcePath,MASSFBXDIR);
    //qDebug() << "Tree : " << nFol << Qt::endl;

    LocalPath += inBaseScript.endsWith(".py") ? ".py" : ".mel";
    //File
    QFile Script(LocalPath);
    if(!Script.exists())
    {
        qDebug() << "Paht Does not exists" << Qt::endl;
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

    //Script Export
    QString Mel ;
    //Base Script
    QString BaseScr ;
    //Program
    QString inProg;

    //Log Maya Console
    LogPath =  MASSLOGDIR + GetSourceName() + "Log.txt";

    QString Cmd;

    switch (inType) {
        case Maya:{
            ScriptPath = MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".mel";
            BaseScr = SCRIPTDIR + "MayaExportCmd.mel";
            Cmd = MAYACONSOLE.arg(MProgram,MSourcePath,ScriptPath,LogPath);
            break;
        }
        case Blender:{
            ScriptPath = MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".py";
            BaseScr = SCRIPTDIR + "BlenderExport.py";
            Cmd = BLENDERCONS.arg(BProgram,MSourcePath,ScriptPath,LogPath);
            break;
        }
    }
    mCSoft = inType;
    //Init Create Scripts Files;
    InitExportScript(BaseScr);
    qDebug() << "Tree : " << BaseScr << Qt::endl;

    //Debug

    //AddToLogData(Cmd);
    qDebug() << Cmd << Qt::endl;
    return Cmd;
}

//private slots emit Logs
void ConsoleCmd::ExportLogResult()
{
    emit OnLogResult(ExportResult);
}
