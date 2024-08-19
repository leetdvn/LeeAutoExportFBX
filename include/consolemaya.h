#ifndef CONSOLEMAYA_H
#define CONSOLEMAYA_H

#include <QFile>
#include <QDir>
#include <Definitions.h>

class ConsoleMaya : QObject
{
    Q_OBJECT
public:
    ConsoleMaya(QString inMayaBatch,QString inSourceFile,QString inExportDir);
    ~ConsoleMaya();

    void SetProgram(const QString inProgram);

private:
    QString MMayaBatch;
    QString MSourcePath;
    QString MExportDir;
};

#endif // CONSOLEMAYA_H
