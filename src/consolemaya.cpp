#include <consolemaya.h>

ConsoleMaya::ConsoleMaya(QString inMayaBatch,QString inSourceFile,QString inExportDir)
    :MMayaBatch(inMayaBatch),
    MSourcePath(inSourceFile),
    MExportDir(inExportDir)

{
    this->moveToThread(&mThread);

    qDebug() << "Maya Console .." << Qt::endl;
}

ConsoleMaya::~ConsoleMaya()
{
    if(mThread.isRunning()){
        mThread.terminate();
        mThread.quit();
    }
}

void ConsoleMaya::SetProgram(const QString inProgram)
{
    QFile fileProgram(inProgram);
    if(!fileProgram.exists()) return;

    if(!inProgram.endsWith("mayabatch.exe"))
        qDebug() << inProgram << "not mayabatch file.." << Qt::endl;

    MMayaBatch = inProgram;
}

void ConsoleMaya::SetMSourcePath(const QString inSourcePath)
{
    QFile souce(inSourcePath);
    if(!souce.exists()){

        qDebug() << MAYACONSOLE << inSourcePath;
    }

    MSourcePath = inSourcePath;
}

void ConsoleMaya::SetMExportDir(const QString inExportDir)
{
    QDir ExpDir(inExportDir);
    if(!ExpDir.exists()){
        qDebug() << MAYACONSOLE << ExpDir;
    }

    MExportDir = inExportDir;
}



bool ConsoleMaya::VerifiedExported(const QString inLogFile)
{
    int Total = FilterCompleted(inLogFile,ExportResult);

    for(auto Ex : ExportResult){
        qDebug() << "Export Completed : " << Ex << Qt::endl;
    }

    qDebug() << "Total Fbx in File : " << Total << Qt::endl;
    return false;
}

int ConsoleMaya::FilterCompleted(const QString inLogFile,QStringList &LogResult)
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
            if(line.startsWith(CompletedCase)){
                LogResult.push_back(line);
            }
        }
        log.close();
    }

    return LogResult.count();
}

QString ConsoleMaya::InitExportScript()
{
    return QString();
}

QString ConsoleMaya::MakeConsoleCmd()
{
    if(MMayaBatch.isNull() || MMayaBatch =="") {

        qDebug() << MAYACONSOLE << "MayaBatch File not found.." << Qt::endl;
        return QString();
    }

    //Init Exp Scripts
    QString ScriptPath = InitExportScript();

    QString Cmd = MMayaBatch +  " -file \""  + MSourcePath + "\" -noAutoloadPlugins -script " ;
    Cmd += "\"%1\"";
    Cmd+= " -log \"%2\"";

    QString result = Cmd.arg(ScriptPath,LogPath);
    return result;
}
