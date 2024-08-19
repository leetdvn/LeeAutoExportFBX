#include <mayacmd.h>

MayaCmd::MayaCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir)
    :Super(inMayaBatch,inSourceFile,inExportDir)
{
}

MayaCmd::~MayaCmd()
{

}

void MayaCmd::InItProgram(const QString inProgram)
{
    MProgram = inProgram;

    qDebug() << "Program Make"  << Qt::endl;
}
