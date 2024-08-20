#ifndef CONSOLECMD_H
#define CONSOLECMD_H

#include <QFile>
#include <QDir>
#include <Definitions.h>
#include <QThread>
#include <massfbxultilities.h>
#include <Definitions.h>
#include <QProcess>

using namespace MassFbxUltilities;

class ConsoleCmd : public QObject
{
    Q_OBJECT
public:
    ConsoleCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir);

    ConsoleCmd(const ConsoleCmd & cCmd);

    ~ConsoleCmd();

    void SetProgram(const QString inProgram);

    void SetMSourcePath(const QString inSourcePath);

    void SetMExportDir(const QString inExportDir);

    bool Status(){return IsRunning;}

    void SetStatus(bool inStatus) {IsRunning = inStatus;}

    void SetConsoleCmd(const QString inCmd);

    void SetLogPath(const QString inPath){LogPath=inPath;}

    QString GetLogs() {return LogPath;}

    QProcess* GetProcess(){return MExProcess;}
    //get Script Path
    QString GetScriptPath();
    //Get inSourceFile
    QString GetSourceFile(){return MSourcePath;}
    //Get Source File Name
    QString GetSourceName();
    //Get ExpResult;
    QStringList GetExpResults(){return ExportResult;}

    bool GetLayerInfo(){return IsNotFound;}
    //Read Log Verifify Export
    bool VerifiedExported();

    virtual void InItProgram() = 0;


signals:
    void OnLogResult(QStringList Logs);

protected:

    //
    QProcess* MExProcess=nullptr;

    //Maya or Blender Program
    QString MProgram;

    //Source File Path
    QString MSourcePath;

    //Export Dir
    QString MExportDir;

    //M Threading
    QThread mThread;

    //Console Command
    QString mCmd;

    //Log File
    QString LogPath;
    //Status
    bool IsRunning=false;

    //bool Layer Not Found
    bool IsNotFound = false;
    //Total FBX in Layer
    int NumFbx;
    //List Result FBX
    QStringList ExportResult;

    //Filter Exported.
    int FilterCompleted(const QString inLogFile, QStringList &LogResult);

    //Init Script Path
    QString InitExportScript(const QString inBaseScript);

    // Create Cmd
    QString MakeConsoleCmd(SoftwereType inType);

private slots:
    void ExportLogResult();

};

typedef ConsoleCmd Super;
#endif // CONSOLECMD_H
