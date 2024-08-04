#include "commandline.h"

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

QString CommandLine::MakeCmd(const QString inSourceFile, const QString inExportFile,bool isOverride)
{
    QFile Sfile(inSourceFile);
    if(!Sfile.exists()) return QString();
}

QString CommandLine::MakeMayaCommand(const QString inSourceFile, const QString inExportFile)
{
    return QString();
}

QString CommandLine::MakeBlenderCommand(const QString inSourceFile, const QString inExportFile)
{
    return QString();
}

QString CommandLine::GetMelCommand(const QString inMelScript)
{
    QFile melFile(MELEXPORTSCRIPT);
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
        "\"C:/Program Files/Blender Foundation/Blender 4.2/blender.exe\" ":
        "\"C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe\"";

}

void CommandLine::InitBlenderScript(const QString inExportFile)
{
    //Blender Export Scripts
    QFile blendExp(BLENDEREXPORT);
    if(blendExp.open(QIODevice::WriteOnly))
    {
        QString Cmd = "import bpy\nbpy.ops.export_scene.fbx(filepath='"+ inExportFile + "')";
        blendExp.write(Cmd.toLocal8Bit());
        blendExp.close();
    }

}

void CommandLine::CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,SoftwereType inSType)
{
    //init Programs
    QString mProgram = IsValidProgram(inSourceFile);

    //Blender Export Scripts Init
    InitBlenderScript(inExportFile);

    //Log
    QFile ExportLog(MASSFBXLOG);

    //Create Command
    QString Cmd;
    if(inSourceFile.endsWith(".ma") || inSourceFile.endsWith(".mb")){
        Cmd =  mProgram + " -file \""  + inSourceFile + "\"\" ;\n" ;
        Cmd += GetMelCommand(MELEXPORTSCRIPT) + "\"" + inExportFile;
    }
    else if(inSourceFile.endsWith(".blend")){
        Cmd = mProgram +  " -b " + inSourceFile;
        Cmd += " -P "  BLENDEREXPORT;
    }
    else{
        qDebug() << inSourceFile << " Not Blender or Maya file format software." << Qt::endl;
        OutLog += "fle : " +  inSourceFile + " Not Blender file format software.\n";
        return;
    }

    qDebug() << "command : "<< Cmd << Qt::endl;
    //QProcess


    QStringList params = QStringList() << Cmd;

    //run command..//
    ExportProcess.start(Cmd);
    ExportProcess.waitForStarted();
    //ExportProcess.setProperty("CRFile",QVariant(inExportFile));
    CRFile = inExportFile;
    // ExportProcess->start(mProgram,params);

    // ExportProcess->waitForFinished(-1);
    OutLog += ExportProcess.readAllStandardOutput();
    qDebug() << OutLog << Qt::endl;
    //ui->LeeLog->setPlainText(stdoutStr);
    //Write to log file
    if(ExportLog.open(QIODevice::WriteOnly)){
        ExportLog.write(OutLog.toLocal8Bit());
        ExportLog.close();
    }

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
