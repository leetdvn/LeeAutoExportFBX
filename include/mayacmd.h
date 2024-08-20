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

signals:
    void OnStartExc();

    void OnFinish(QStringList Logs);

    void OnError();

private:
    QProcess MExProcess;

    bool IsRuning=false;

private slots:
    void OnExpStart();

    void OnExpFinish();

    void OnExpError();
};

#endif // MAYACMD_H
