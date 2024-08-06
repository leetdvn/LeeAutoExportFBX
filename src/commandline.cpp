#include "commandline.h"

#include <QDir>
#include <QXmlStreamReader>

CommandLine::CommandLine(QObject *parent)
    :QObject(parent)
{
    this->moveToThread(&mThread);

    connect(&mThread,SIGNAL(started()),this,SLOT(ReadSTDIN()));
    connect(this,SIGNAL(OnReadLine(QString)),this,SLOT(HandleSTDIN(QString)));

    mThread.start();
}

CommandLine::~CommandLine()
{
    if(mThread.isRunning())
        mThread.quit();
}

void CommandLine::ReadSTDIN()
{
    QTextStream stream(stdin);
    QString Line = stream.readLine();
    if(Line.isEmpty()|| Line.isNull()) return;
    qDebug() << Line;
    emit OnReadLine(Line);
    ReadSTDIN();
}

void CommandLine::HandleSTDIN(QString Line)
{
    qDebug() << Line;

    //WriteSTDOUT("You entered : " + Line);
}

void CommandLine::WriteSTDOUT(QString Line)
{
    //QTextStream stream(stdout);
    //qDebug() << Line;
    //stream << Line;
}

void CommandLine::StopThread()
{
    if(mThread.isRunning()) mThread.quit();
}

QString CommandLine::MakeCmdAsScript(const QString inProgram,const QString inSourceFile, const QString inExportFile,bool &isSuccess)
{
    QString Cmd;
    if(inSourceFile.endsWith(".ma") || inSourceFile.endsWith(".mb")){
        ScriptPath= InitMelScript(inExportFile);
        Cmd =  inProgram + " -file \""  + inSourceFile + "\" -script " ;
        Cmd += "\"" + ScriptPath + "\"";
        isSuccess=true;
    }
    else if(inSourceFile.endsWith(".blend")){
        //Blender Export Scripts Init
        ScriptPath = InitBlenderScript(inExportFile);
        //BlenderScripts.append(ScriptF);
        Cmd = inProgram +  " -b \"" + inSourceFile;
        Cmd += "\" -P \"" + ScriptPath + "\"";
        isSuccess=true;
    }
    else{
        isSuccess= false;
    }

    return Cmd;
}

QString CommandLine::MakeMayaCommand(const QString inSourceFile, const QString inExportFile)
{   
    QString iProgram = IsValidProgram(inSourceFile);
    QString Cmd;
    ScriptPath= InitMelScript(inExportFile);
    Cmd =  iProgram + " -file \""  + inSourceFile + "\" -script " ;
    Cmd += "\"" + ScriptPath + "\"";

    return Cmd;
}

QString CommandLine::MakeBlenderCommand(const QString inSourceFile, const QString inExportFile)
{
    return QString();
}

QString CommandLine::GetMelCommand(const QString inMelScript)
{
    QFile melFile(inMelScript);

    if(!melFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", melFile.errorString());
    }

    QTextStream melStream(&melFile);

    return melStream.readAll();
}

QString CommandLine::IsValidProgram(const QString inSourceFile)
{
    return inSourceFile.endsWith(".blend") ?
        BlenderProgram:
        MayaProgram;

}

QString CommandLine::InitBlenderScript(const QString inExportFile)
{

    QStringList nName = inExportFile.split("/");

    QString fileName = nName[nName.count()-1];

    QString newName = fileName.left(fileName.lastIndexOf("."));

    QDir sDir(LOCALSCRIPTS);
    if(!sDir.exists())
        sDir.mkdir(LOCALSCRIPTS);

    QString ScriptFile = LOCALSCRIPTS + newName + ".py";
    BlenderScripts.append(newName + ".py");
    //qDebug() << "file Name : "  + newName ;
    //Blender Export Scripts
    QFile blendExp(ScriptFile);
    if(blendExp.open(QIODevice::WriteOnly))
    {
        QString Cmd = "import bpy\nbpy.ops.export_scene.fbx(filepath='"+ inExportFile + "')";
        blendExp.write(Cmd.toLocal8Bit());
        blendExp.close();
    }

    return ScriptFile;
}

QString CommandLine::InitMelScript(const QString inExportFile)
{
    QStringList nName = inExportFile.split("/");

    QString fileName = nName[nName.count()-1];

    QString newName = fileName.left(fileName.lastIndexOf("."));

    QDir sDir(LOCALSCRIPTS);
    if(!sDir.exists())
        sDir.mkdir(LOCALSCRIPTS);

    QString ScriptFile = LOCALSCRIPTS + newName + ".mel";
    BlenderScripts.append(newName + ".mel");
    // QString melS = GetMelCommand(MELEXPORTSCRIPT);
    // qDebug() << "mel : " << melS.arg(inExportFile);
    //Blender Export Scripts
    QFile MelExp(ScriptFile);
    if(MelExp.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        QString Cmd = GetMelCommand(MELEXPORTSCRIPT).arg(inExportFile);//"file -force -options \"v=0\" -type \"FBX export\" -pr -ea \""+ inExportFile + "\";";
        MelExp.write(Cmd.toLocal8Bit());
        MelExp.close();
    }

    return ScriptFile;
}

void CommandLine::AddToLog(QString inContentLine)
{
    QFile fbxlog(MASSFBXLOG);
    inContentLine +="\n";
    if(fbxlog.open(QIODevice::WriteOnly | QIODevice::Append)){
        fbxlog.write(inContentLine.toLocal8Bit());
        fbxlog.close();
    }
}

void CommandLine::OnSentCRFile()
{
    ErrorStr= ExportProcess.readAllStandardError();

    OnError();
    //make signal
    emit SendCRFile(CRFile,CSFile);
    emit OnSendId();

    StopThread();
    isRunning=false;

    if(!ClearOnCompleted) return;

    if(BlenderScripts.count() > 0){
        for(auto s : BlenderScripts)
        {
            QDir dir(LOCALSCRIPTS);
            QFile file(LOCALSCRIPTS + s);

            if(file.exists())
                dir.remove(s);
        }
    }
}

void CommandLine::OnError()
{
    emit SendErrorStr(ErrorStr);
}

void CommandLine::OnSendId()
{
    emit SendId(commandId);
}

void CommandLine::CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,SoftwereType inSType)
{
    //init Programs
    QString mProgram = IsValidProgram(inSourceFile);

    ;

    //Log
    QFile ExportLog(MASSFBXLOG);

    //Create Command
    bool isSuccess;
    QString Cmd = MakeCmdAsScript(mProgram,inSourceFile,inExportFile,isSuccess);
    //log
    if(!isSuccess){
        qDebug() << inSourceFile << " Not Blender or Maya file format software." << Qt::endl;
        OutLog += "fle : " +  inSourceFile + " Not Blender file format software. <br>";
        return;
    }

    AddToLog(Cmd);
    qDebug() << "command : "<< Cmd << Qt::endl;
    //QProcess


    QStringList params = QStringList() << Cmd;

    //run command..//
    CRFile = inExportFile;
    CSFile = inSourceFile;
    connect(&ExportProcess,&QProcess::finished,this,&CommandLine::OnSentCRFile);
    connect(&ExportProcess,&QProcess::errorOccurred,this,&CommandLine::OnError);
    ExportProcess.start(Cmd);
    ExportProcess.waitForStarted();
    isRunning=true;
    //ExportProcess.setProperty("CRFile",QVariant(inExportFile));
    // ExportProcess->start(mProgram,params);

    // ExportProcess->waitForFinished(-1);
    OutLog += ExportProcess.readAllStandardOutput();
    qDebug() << OutLog << Qt::endl;

    //Write to log file
    // if(ExportLog.open(QIODevice::WriteOnly)){
    //     // ExportLog.write(OutLog.toLocal8Bit());
    //     // ExportLog.close();
    // }

    QFile completedFile(inExportFile);
    if(completedFile.exists())
        qDebug() << "Exported : " + inExportFile << Qt::endl;

    //ExportProcess->deleteLater();
    //QString stderrStr = ExportProcess->readAllStandardError();
}

void CommandLine::UpdateLogString(QPlainTextEdit *inText, QString &LogStr)
{
    if(!inText) return;

    return inText->setPlainText(LogStr);
}

void CommandLine::SetClearOnComplete(bool isDelete)
{
    ClearOnCompleted = isDelete;
}

void SendCRFile(QString SendPath,QString SendSFile){
}
