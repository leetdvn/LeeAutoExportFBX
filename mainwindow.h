#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QUndoStack>
#include "commandline.h"
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // void show_console() {
    //     //AllocConsole();

    //     freopen("conin$", "r", stdin);
    //     freopen("conout$", "w", stdout);
    //     freopen("conout$", "w", stderr);
    // }

public slots:
    void OnTextChanged();

    void OnBrowserClicked();

    void OnExportClicked();

    void GetFilesInDir(const QString inDir,QStringList &OutFiles);
private:
    Ui::MainWindow *ui;

    QUndoStack* undo_stack=nullptr;

    QString logStr;
    QString ipSourceDir;
    QString ExportDir;

    CommandLine* command=nullptr;

    QProcess* mProcess=nullptr;

    QString GeneratedCommand(const QString inSourceFile, const QString inExportFile);

    QStringList MayaFiles,BlenderFiles;

    QList<QStringList> leeFilters;

};
#endif // MAINWINDOW_H
