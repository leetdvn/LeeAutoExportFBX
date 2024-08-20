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
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <omp.h>
#include <QHostInfo>
#include <QNetworkInterface>
#include <qmessagebox.h>
#include <massfbxultilities.h>
#include <mayacmd.h>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class FbxOptions;
}
QT_END_NAMESPACE

enum DataPath
{
    LBatchMaya,
    LBlender,
    LSource,
    LExport
};

using namespace MassFbxUltilities;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void OnTextChanged();

    void OnSoftWareChanged();

    void OnBrowserFolder();

    void OnBrowserFile();

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

    //init Default
    void InitLocal();

    //Maya Files Blender Files
    QStringList MayaFiles,BlenderFiles;

    //List Source Files
    QStringList EpSourceFiles;
    int EpCount = 0;

    QList<QStringList> leeFilters;

    QList<CommandLine*> MassCmds;

    bool ValidPaths();

    void SaveRecentData(const QString inFilePath);

    void SaveToLocal(DataPath inType,const QString inContent);

    void LoadRecentData();

    QJsonObject LoadObjectFromFile(const QString infile);

    QString localfilePath="C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/LeeMassFbx.json";

    QString GetMacAdress();

    bool StudioIsValid();

    bool IsValidMaya(const QString inProgramPath) {return inProgramPath.endsWith("mayabatch.exe");}

    bool IsValidBlender(const QString inProgramPath) {return inProgramPath.endsWith("blender.exe");}

    bool IsValidSoft();

    void AddToLog(QString inMessage,QString inColor="black");

private slots:

    void Display(QString inReceiveFile,QString CSFile);

    void DisplayErr(QString ErStr);

    void OnCompletedId(int Id);

protected:

    MayaCmd* mMaya=nullptr;
    CommandLine* command = nullptr;
    void ImplementFbxOptions();

    QString GetExportPath(const QString inSourceFile,const QString inExportDir);

    SoftwereType GetSoftWareType();

    void ClearScripts();
    //info Env
    void InfoEnv();

    //execute Fbx Export
    void ExecuteExportFbx(const int inId);

    LeeSpoiler* Spoiler;

    QPushButton* ExpotBtn;

    int SourceType=-1;

    QString LastCompletedFile;

    QString _Pc,_Users,_Host;
    bool isRunning=false;

    int TotalFiles;
    int completedId=0;

    bool isError=false;

    void OnMayaFinish(QStringList inFbxList);

};
#endif // MAINWINDOW_H
