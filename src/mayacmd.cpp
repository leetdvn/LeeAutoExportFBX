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
    QFile programF(MProgram);
    if(!programF.exists()){

        qDebug() << MAYALOG << "does'nt exists.."  << Qt::endl;
    }

    //Script Files Path;
    //QString Script = MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".mel";

    //Log Maya Console
    //QString LogF =  MASSFBXDIR +  "Logs/"+  GetSourceName() + "Log.txt";
    //Cmd
    //QString Cmd = MAYACONSOLE.arg(MProgram,MSourcePath,Script,LogF);

    QString Cmd= MakeConsoleCmd(Maya);

    connect(&MExProcess,&QProcess::started,this,&MayaCmd::OnExpStart);
    connect(&MExProcess,&QProcess::errorOccurred,this,&MayaCmd::OnError);
    connect(&MExProcess,&QProcess::finished,this,&MayaCmd::OnExpFinish);
    MExProcess.start(Cmd);
    MExProcess.waitForStarted();
    IsRuning=true;

    //Check Log Cmd
    AddToLogData(Cmd);

    //

    //Debug
    qDebug() << MAYALOG << Cmd << Qt::endl;
}

void MayaCmd::MayExecuteExp()
{
    // MExProcess.start(Cmd);
    // ExportProcess.waitForStarted();
    // IsRuning=true;

}


void MayaCmd::OnExpStart()
{
    emit OnStartExc();
}

void MayaCmd::OnExpFinish()
{
    emit OnFinish(ExpLogs);
}

void MayaCmd::OnExpError()
{
    emit OnError();
}
