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

    QString Message;

    //Setting ID
    int GetExpId() {return ExpId;}
    void SetExpId(int newId){ ExpId = newId;}

    void ClearOnFinish();

signals:
    void OnStart();

    void OnFinish(QStringList Logs);

    void OnError();

private:

    bool IsRuning=false;

    int ExpId=0;

private slots:
    void OnExpStart();

    void OnExpFinish();

    void OnExpError();

    void ReadLogs();
};

#endif // IMPCMD_H
