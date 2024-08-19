#ifndef CONSOLECMD_H
#define CONSOLECMD_H

#include <QFile>
#include <QDir>
#include <Definitions.h>
#include <QThread>
#include <massfbxultilities.h>

#define MAYACONSOLE QString("MayaConsole : ")


using namespace MassFbxUltilities;

class ConsoleCmd : QObject
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

    QString GetLogs() {return LogPath;}

    QString GetScriptPath();

    //Read Log Verifify Export
    bool VerifiedExported(const QString inLogFile);

    virtual void InItProgram(const QString inProgram) = 0;

protected:

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
    bool IsNotFound;
    //Total FBX in Layer
    int NumFbx;
    //List Result FBX
    QStringList ExportResult;

    //Filter Exported.
    int FilterCompleted(const QString inLogFile, QStringList &LogResult);

    //Init Script Path
    QString InitExportScript();

    // Create Cmd
    QString MakeConsoleCmd(SoftwereType inType);

};

typedef ConsoleCmd Super;
#endif // CONSOLECMD_H
