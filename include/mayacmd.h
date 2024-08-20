#ifndef MAYACMD_H
#define MAYACMD_H

#include <QProcess>
#include <consolecmd.h>

class MayaCmd : public ConsoleCmd
{
    Q_OBJECT
public:
    MayaCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir);
    ~MayaCmd();

    virtual void InItProgram();

    void MayExecuteExp();

    bool IsNotFound;

    QString Message;

    //Setting ID
    int GetExpId() {return ExpId;}
    void SetExpId(int newId){ ExpId = newId;}

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

#endif // MAYACMD_H
