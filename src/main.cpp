#include "mainwindow.h"

#include <QApplication>
#include <QProcess>
#include "Definitions.h"


int main(int argc, char *argv[])
{

    // it is useful to detach from Qt Creator’s <Application output> panel
    // FreeConsole();

    // // create a separate new console window
    //AllocConsole();


    // SetConsoleOutputCP(65001);

    // reopen the std I/O streams to redirect I/O to the new console
    // freopen("CON", "w", stdout);
    // freopen("CON", "w", stderr);
    // freopen("CON", "r", stdin);

    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(MASSVERSION);
    w.show();
    return a.exec();
}
