#include <consolemaya.h>

ConsoleMaya::ConsoleMaya(QString inMayaBatch,QString inSourceFile,QString inExportDir)
    :MMayaBatch(inMayaBatch),
    MSourcePath(inSourceFile),
    MExportDir(inExportDir)

{

}

void ConsoleMaya::SetProgram(const QString inProgram)
{
    QFile fileProgram(inProgram);
    if(!fileProgram.exists()) return;

    if(!inProgram.endsWith("mayabatch.exe"))
        qDebug() << inProgram << "not mayabatch file.." << Qt::endl;

    MMayaBatch = inProgram;
}
