#ifndef MAYACMD_H
#define MAYACMD_H

#include <consolecmd.h>

class MayaCmd : public ConsoleCmd
{

public:
    MayaCmd(QString inMayaBatch,QString inSourceFile,QString inExportDir);
    ~MayaCmd();

    virtual void InItProgram(const QString inProgram);
};

#endif // MAYACMD_H
