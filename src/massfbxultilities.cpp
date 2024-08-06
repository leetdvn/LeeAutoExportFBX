#include "massfbxultilities.h"


bool MassFbxUltilities::IsValidPath(QString inPaths)
{
    return QDir(inPaths).exists() ? true :
               QFile(inPaths).exists() ? true : false;
}
