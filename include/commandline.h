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

class CommandLine : public QObject
{
    Q_OBJECT

public:
    explicit CommandLine(QObject *parent=0);
    ~CommandLine();
    void CreateProcess(const QString inSourceFile, const QString inExportFile,QString &OutLog,bool isMaya=true);

    void UpdateLogString(QPlainTextEdit* inText,QString &LogStr);

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

    QString GetMelCommand(const QString inMelScript);

    QProcess ExportProcess;
};


#endif // COMMANDLINE_H
