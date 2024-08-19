#include <consoleCmd.h>

ConsoleCmd::ConsoleCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir)
    :MProgram(inMayaBatch),
    MSourcePath(inSourceFile),
    MExportDir(inExportDir)
{
    this->moveToThread(&mThread);
    qDebug() << "Maya Console .." << Qt::endl;
}

ConsoleCmd::ConsoleCmd(const ConsoleCmd &cCmd)
    :MProgram(cCmd.MProgram),
    MSourcePath(cCmd.MSourcePath),
    MExportDir(cCmd.MExportDir)
{

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

    qDebug() << BASECONSOLE << result << Qt::endl;
    return SourceF[0];
}

bool ConsoleCmd::VerifiedExported(const QString inLogFile)
{
    int Total = FilterCompleted(inLogFile,ExportResult);

    for(auto Ex : ExportResult){
        qDebug() << "Export Completed : " << Ex << Qt::endl;
    }

    qDebug() << "Total Fbx in File : " << Total << Qt::endl;
    return false;
}

int ConsoleCmd::FilterCompleted(const QString inLogFile,QStringList &LogResult)
{

    QFile log(inLogFile);

    if(!log.exists()) {
        qDebug() << MAYACONSOLE << inLogFile << "file does'nt Exists." <<  Qt::endl;
        return 0;
    }


    QString CompletedCase = "Exported";
    if(log.open(QIODevice::ReadOnly)){
        QTextStream textfile(&log);
        while(!textfile.atEnd())
        {
            QString line = textfile.readLine();
            line.replace("\n","");
            if(line.startsWith(CompletedCase)){
                LogResult.push_back(line);
            }
            else if(line.startsWith("MassFbxNumber")){
                QString lineX = line.replace("\n","");
                NumFbx = lineX.right(1).toInt();
                qDebug() << "Member Of Layer : " << NumFbx << Qt::endl;
            }
            IsNotFound  = line.endsWith("MassExport") ? 1 : 0;


        }
        log.close();

        qDebug() << "Layer Not Found : " << IsNotFound << Qt::endl;

    }

    return LogResult.count();
}

QString ConsoleCmd::InitExportScript()
{
    return QString();
}

QString ConsoleCmd::MakeConsoleCmd(SoftwereType inType)
{
    if(MProgram.isNull() || MProgram =="" || mCmd.isNull() || mCmd !="") {

        qDebug() << MAYACONSOLE << "MayaBatch File not found.." << Qt::endl;
        return QString();
    }

    //Init Exp Scripts
    QString ScriptPath = InitExportScript();

    QString Cmd = MProgram +  " -file \""  + MSourcePath + "\" -noAutoloadPlugins -script " ;
    Cmd += "\"%1\"";
    Cmd+= " -log \"%2\"";

    QString result = Cmd.arg(ScriptPath,LogPath);
    return result;
}
