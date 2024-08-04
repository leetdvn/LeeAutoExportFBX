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
    QProcess ExportProcess;
    QString CRFile;

signals:
    void OnReadLine(QString Line);

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

    void InitBlenderScript(const QString inExportFile);
};

#endif // COMMANDLINE_H
