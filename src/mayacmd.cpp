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
    QString Script = MASSFBXDIR +  "Scripts/"+  GetSourceName() + ".mel";

    //Log Maya Console
    QString LogF =  MASSFBXDIR +  "Logs/"+  GetSourceName() + "Log.txt";
    //Cmd
    QString Cmd = MAYACONSOLE.arg(MProgram,MSourcePath,Script,LogF);

    //Check Log Cmd
    AddToLogData(Cmd);

    //Debug
    qDebug() << MAYALOG << Cmd << Qt::endl;
}
