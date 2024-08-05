#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTextStream>
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QPlainTextEdit>

#if defined MELEXPORT
#include "Definitions.h"
#endif

enum SoftwereType
{
    Maya,
    Blender,
    MayaAndBlender,
    None
};

class CommandLine : public QObject
{
    Q_OBJECT

public:
    explicit CommandLine(QObject *parent=0);
    ~CommandLine();
    void CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,SoftwereType inSType=Maya);

    void UpdateLogString(QPlainTextEdit* inText,QString &LogStr);
    QString GetCSFile(){return CSFile;}
    bool IsRunning(){return isRunning;}
    QProcess ExportProcess;

    void SetMayaPro(const QString inPath){MayaProgram = "\"" + inPath +"\" "; };
    void SetBlenderPro(const QString inPath){BlenderProgram = "\"" + inPath +"\" "; };
    void SetCommandId(int inId){commandId = inId;}

signals:
    void OnReadLine(QString Line);
    void SendCRFile(QString SendPath,QString SendSFile);
    void SendErrorStr(QString SentStr);
    void SendId(int);


public slots:
    void ReadSTDIN();

    void HandleSTDIN(QString Line);

    void WriteSTDOUT(QString Line);

    void StopThread();

private:
    QThread mThread;

    QString MakeCmd(const QString inSourceFile, const QString inExportFile,bool isOverride=false);

    QString MakeMayaCommand(const QString inSourceFile, const QString inExportFile);

    QString MakeBlenderCommand(const QString inSourceFile, const QString inExportFile);

    QString GetMelCommand(const QString inMelScript);

    QString IsValidProgram(const QString inSourceFile);

    QString InitBlenderScript(const QString inExportFile);

    QString InitMelScript(const QString inExportFile);

    QString LocalScripts ="C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/Scripts/";

    QString CRFile,CSFile;
    QString ErrorStr;
    QString MayaProgram;
    QString BlenderProgram;
    QStringList BlenderScripts;

    int commandId=-1;
    bool isRunning;

private slots:
    void OnSentCRFile();
    void OnError();
    void OnSendId();
};

#endif // COMMANDLINE_H
