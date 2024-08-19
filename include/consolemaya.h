#ifndef CONSOLEMAYA_H
#define CONSOLEMAYA_H

#include <QFile>
#include <QDir>
#include <Definitions.h>
#include <QThread>
#include <massfbxultilities.h>

#define MAYACONSOLE QString("MayaConsole : ")


using namespace MassFbxUltilities;

class ConsoleMaya : QObject
{
    Q_OBJECT
public:
    ConsoleMaya(QString inMayaBatch,QString inSourceFile,QString inExportDir);
    ~ConsoleMaya();

    void SetProgram(const QString inProgram);

    void SetMSourcePath(const QString inSourcePath);

    void SetMExportDir(const QString inExportDir);

    bool Status(){return IsRunning;}

    void SetStatus(bool inStatus) {IsRunning = inStatus;}

    QString GetLogs() {return LogPath;}

    QString GetScriptPath();

    //Read Log Verifify Export
    bool VerifiedExported(const QString inLogFile);


private:

    //MayaBatch Program
    QString MMayaBatch;

    //Source File Path
    QString MSourcePath;

    //Export Dir
    QString MExportDir;

    //M Threading
    QThread mThread;

    //Console Command
    QString ConsoleCmd;

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
    QString MakeConsoleCmd();

};

#endif // CONSOLEMAYA_H
