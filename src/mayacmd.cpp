#include <mayacmd.h>

MayaCmd::MayaCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir)
    :Super(inMayaBatch,inSourceFile,inExportDir)
{
    InItProgram();
}

MayaCmd::~MayaCmd()
{

}

void MayaCmd::InItProgram()
{
    return MayExecuteExp();
}

void MayaCmd::MayExecuteExp()
{
    QFile programF(MProgram);
    if(!programF.exists()){

        qDebug() << MAYALOG << "does'nt exists.."  << Qt::endl;
    }

    QString Cmd= MakeConsoleCmd(Maya);
    MExProcess = new QProcess();
    connect(MExProcess,&QProcess::readyReadStandardOutput,this,&MayaCmd::ReadLogs);
    connect(MExProcess,&QProcess::errorOccurred,this,&MayaCmd::OnError);
    connect(MExProcess,&QProcess::finished,this,&MayaCmd::OnExpFinish);

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


void MayaCmd::OnExpStart()
{
    emit OnStart();
}

void MayaCmd::OnExpFinish()
{

    emit OnFinish(ExportResult);
}

void MayaCmd::OnExpError()
{
    emit OnError();
}

void MayaCmd::ReadLogs()
{
    QString line = MExProcess->readLine();
    qDebug() << line << Qt::endl;
}
