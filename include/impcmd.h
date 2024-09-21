#ifndef IMPCMD_H
#define IMPCMD_H

#include <QProcess>
#include <consolecmd.h>

class ImpCmd : public ConsoleCmd
{
    Q_OBJECT
public:
    ImpCmd(QString inSourceFile,QString inExportDir);
    ~ImpCmd();

    virtual void InItProgram();

    void MayExecuteExp();

    bool IsNotFound;
    bool MissingAddons;

    QString Message;
    QString Logs;

    //Setting ID
    int GetExpId() {return ExpId;}
    void SetExpId(int newId){ ExpId = newId;}

    void ClearOnFinish();

    void SetProgram(const QString inMayaPath,const QString inBlenderPath);

    bool ConsoleExists(QList<ImpCmd*> inList);

    QStringList ListFbxs;


    void SetExportSkeletal(bool isSkeletal) {isExportSkeletal=isSkeletal;}

    bool GetExportSkeletal() {return isExportSkeletal;}

signals:
    void OnStart();

    void OnFinish(int exitCode, QProcess::ExitStatus exitStatus,QStringList Logs);

    void OnError(QProcess::ProcessError &Error);

    void OnReadLogs(QString&,QString&);

    void OnPStateChanged(QProcess::ProcessState newState);

private:

    bool IsRuning=false;

    int ExpId=0;

    bool isExportSkeletal;

    void InitSkeletalExp(const SoftwereType Blender);

    QString SkeletalDir;

private slots:
    void OnExpStart();

    void OnExpFinish(int exitCode, QProcess::ExitStatus exitStatus);

    void OnExpError();

    void ReadLogs();

    void OnStateChanged();
};

#endif // IMPCMD_H
