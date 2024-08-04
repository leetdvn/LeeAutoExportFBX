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

    void GetFilesInDir(const QString inDir,QStringList &OutFiles);
private:
    Ui::MainWindow *ui;
    Ui::FbxOptions *uiOpt;

    QUndoStack* undo_stack=nullptr;

    QString logStr;
    QString ipSourceDir;
    QString ExportDir;

    CommandLine* command=nullptr;

    QProcess* mProcess=nullptr;

    QString GeneratedCommand(const QString inSourceFile, const QString inExportFile);

    QStringList MayaFiles,BlenderFiles;

    QList<QStringList> leeFilters;

protected:

    void ImplementFbxOptions();

    LeeSpoiler* Spoiler;

    QPushButton* ExpotBtn;

};
#endif // MAINWINDOW_H
