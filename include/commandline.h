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
#include <Definitions.h>
#include <massfbxultilities.h>

enum SoftwereType
{
    Maya,
    Blender,
    MayaAndBlender,
    None
};

#define LOCALSCRIPTS  QString("C:/Users/%1/AppData/Local/LeeMassFbx/Scripts/").arg(qgetenv("USERNAME"))


using namespace MassFbxUltilities;

class CommandLine : public QObject
{
    Q_OBJECT

public:
    explicit CommandLine(QObject *parent=0);
    ~CommandLine();
    void CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,SoftwereType inSType=Maya);

    void UpdateLogString(QPlainTextEdit* inText,QString &LogStr);
    QString GetCSFile(){return CSFile;}
    void SetClearOnComplete(bool isDelete);
    bool IsRunning(){return isRunning;}
    QProcess ExportProcess;

    void SetMayaPro(const QString inPath){MayaProgram = "\"" + inPath +"\" "; };
    void SetBlenderPro(const QString inPath){BlenderProgram = "\"" + inPath +"\" "; };
    void SetCommandId(int inId){commandId = inId;}

    static QString GetMelCommand(const QString inMelScript);

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

    QString MakeCmdAsScript(const QString inProgram,const QString inSourceFile, const QString inExportFile, bool &isSuccess);

    QString MakeMayaCommand(const QString inSourceFile, const QString inExportFile);

    QString MakeBlenderCommand(const QString inSourceFile, const QString inExportFile);

    QString IsValidProgram(const QString inSourceFile);

    QString InitBlenderScript(const QString inExportFile);

    QString InitMelScript(const QString inExportFile);

    void AddToLog(QString inContentLine);

    QString CRFile,CSFile;
    QString ErrorStr;
    QString MayaProgram;
    QString BlenderProgram;

    QString ScriptPath;
    QStringList BlenderScripts;

    int commandId=-1;
    bool isRunning;
    bool ClearOnCompleted;

    void QRegExp(const char *);
private slots:
    void OnSentCRFile();
    void OnError();
    void OnSendId();
};

#endif // COMMANDLINE_H
