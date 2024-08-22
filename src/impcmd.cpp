#include <impcmd.h>

ImpCmd::ImpCmd(QString inSourceFile,QString inExportDir)
    :Super(inSourceFile,inExportDir)
{
}

ImpCmd::~ImpCmd()
{
   this->deleteLater();
}

void ImpCmd::InItProgram()
{
    return MayExecuteExp();
}

void ImpCmd::MayExecuteExp()
{
    QFile programF(MProgram);
    if(!programF.exists()){

        qDebug() << MAYALOG << "does'nt exists.."  << Qt::endl;
    }

    SoftwereType soft = MSourcePath.endsWith(".blend") ? Blender :  Maya;
    QString Cmd= MakeConsoleCmd(soft);


    MExProcess = new QProcess();
    connect(MExProcess,&QProcess::readyReadStandardOutput,this,&ImpCmd::ReadLogs);
    connect(MExProcess,&QProcess::errorOccurred,this,&ImpCmd::OnError);
    connect(MExProcess,&QProcess::finished,this,&ImpCmd::OnExpFinish);

    MExProcess->start(Cmd);
    MExProcess->waitForStarted();
    Message = QString("Exporting : %1").arg(MSourcePath);
    IsRuning=true;

    //Check Log Cmd
    AddToLogData(Cmd);

    //

    //Debug
    qDebug() << MAYALOG << Cmd << Qt::endl;

}

void ImpCmd::ClearOnFinish()
{
    QFile logf(LogPath);
    QFile ScriptF(ScriptPath);

    qDebug() << "Cleanup Script " << ScriptPath;

    if(logf.exists()) logf.remove();
    if(ScriptF.exists()) ScriptF.remove();
}

void ImpCmd::OnExpStart()
{
    emit OnStart();
}

void ImpCmd::OnExpFinish()
{

    emit OnFinish(ExportResult);
}

void ImpCmd::OnExpError()
{
    emit OnError();
}

void ImpCmd::ReadLogs()
{
    QString line = MExProcess->readAllStandardOutput();
    line += MExProcess->readAllStandardError();
    QFile log(LogPath);
    if(log.exists()){
        if(log.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            log.write(line.toLocal8Bit());
            log.close();
        }
    }
    //qDebug() << line << Qt::endl;

    //emit OnReadLogs(line);
}

void ImpCmd::SetProgram(const QString inMayaPath,const QString inBlenderPath)
{
    if(!IsValidPath(inMayaPath))
    {
        //Debug
        return;
    }
    if(!IsValidPath(inBlenderPath)){
        //Debug
        return;
    }
    MProgram = inMayaPath;
    BProgram = inBlenderPath;

    InItProgram();
}

bool ImpCmd::ConsoleExists(QList<ImpCmd *> inList)
{
    if(inList.isEmpty()) return false;

    for(auto cons : inList){
        if(cons->GetSourceFile() == this->GetSourceFile())
            return true;
    }
    return false;
}
