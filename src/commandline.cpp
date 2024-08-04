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

QString CommandLine::MakeCommand(const QString inSourceFile, const QString inExportFile, SoftwereType inSType)
{
    QString mProgram = inSType ==0  ? "\"C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe\"" : "\"C:/Program Files/Blender Foundation/Blender 4.2/blender.exe\" ";
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

void CommandLine::CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,SoftwereType inSType)
{
    //init Programs
    QString mProgram = inSType ==0  ? "\"C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe\"" : "\"C:/Program Files/Blender Foundation/Blender 4.2/blender.exe\" ";

    QString CmdProgram= "C:/Program Files/Blender Foundation/Blender 4.2/blender.exe";
    //make bat
    QString BlendBat("/home/Documents/GitHub/LeeAutoExportFBX/Scripts/ExpotText.txt"); //"C:\Users\thang\Documents\GitHub\LeeAutoExportFBX\Scripts\ExpotText.txt"
    QFile blendExp(BLENDEREXPORT);

    if(blendExp.open(QIODevice::WriteOnly))
    {
        QString Cmd = "import bpy\nbpy.ops.export_scene.fbx(filepath='"+ inExportFile + "')";
        blendExp.write(Cmd.toLocal8Bit());
        blendExp.close();
    }

    //Log
    QFile ExportLog(MASSFBXLOG);

    //Create Command
    QString Cmd;
    if(inSType ==0){
        if(inSourceFile.endsWith(".ma") || inSourceFile.endsWith(".mb")){
            Cmd =  mProgram + " -file \""  + inSourceFile + "\"\" ;\n" ;
            Cmd += GetMelCommand(MELEXPORTSCRIPT) + "\"" + inExportFile;
        }
        else{
            OutLog += "fle : " +  inSourceFile + " Not Maya file format software.\n";
            qDebug() << inSourceFile << " Not Maya file format software." << Qt::endl;
            return;
        }

    }
    qDebug() << "Source : " << inSourceFile << "Type : " << inSType << Qt::endl;

    if(inSType==1){
        qDebug() << "Source : " << Cmd << Qt::endl;

        if(!inSourceFile.endsWith(".blend")){
            qDebug() << inSourceFile << " Not Blender file format software." << Qt::endl;
            OutLog += "fle : " +  inSourceFile + " Not Blender file format software.\n";
            return;
        }
        Cmd = mProgram +  " -b " + inSourceFile;
        Cmd += " -P "  BLENDEREXPORT;

    }
    qDebug() << Cmd << Qt::endl;
    //QProcess


    QStringList params = QStringList() << Cmd;

    //run command..//
    //process.start("\"C:/Program Files/Blender Foundation/Blender 4.2/blender.exe\" -b C:/Users/thang/Documents/abc.blend -P " + QString(BLENDEREXPORT));
    ExportProcess.start(Cmd);
    ExportProcess.waitForStarted();
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
