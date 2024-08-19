#include "mainwindow.h"

#include <QApplication>
#include <QProcess>
#include "Definitions.h"


int main(int argc, char *argv[])
{
    // create a separate new console window
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(MASSVERSION);
    w.show();
    return a.exec();
}
