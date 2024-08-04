#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QUndoStack>
#include "commandline.h"
#include <QProcess>
#include "leespoiler.h"
#include <QColorDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollBar>

#if defined MELEXPORT
#include "Definitions.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class FbxOptions;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OnTextChanged();

    void OnBrowserClicked();

    void OnExportClicked();

    void OnComboBoxChanged(int valuechanged);

    void GetFilesInDir(const QString inDir,QStringList &OutFiles,QStringList inFilters);

    void OnFinish();
private:
    Ui::MainWindow *ui;
    Ui::FbxOptions *uiOpt;

    QUndoStack* undo_stack=nullptr;

    QString logStr;
    QString ipSourceDir;
    QString ExportDir;

    QProcess* mProcess=nullptr;

    QString GeneratedCommand(const QString inSourceFile, const QString inExportFile);

    QStringList InitFillters();

    QStringList MayaFiles,BlenderFiles;

    QList<QStringList> leeFilters;

private slots:
    void Display(QString inReceiveFile,QString CSFile);

    void DisplayErr(QString ErStr);
protected:
    CommandLine* command = nullptr;
    void ImplementFbxOptions();

    QString GetExportPath(const QString inSourceFile,const QString inExportDir);

    SoftwereType GetSoftWareType();

    //execute Fbx Export
    void ExecuteExportFbx(QString inSourceFile,QString inExportDir);

    LeeSpoiler* Spoiler;

    QPushButton* ExpotBtn;

    int SourceType=-1;
    QString LastCompletedFile;

    bool isRunning;

};
#endif // MAINWINDOW_H
