#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QUndoStack>
#include <QProcess>
#include "leespoiler.h"
#include <QColorDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <omp.h>
#include <QHostInfo>
#include <QNetworkInterface>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <massfbxultilities.h>
#include <impcmd.h>
#include <Definitions.h>
#include <QDesktopServices>
#include <QFileDevice>
#include <QTreeView>
#include <QFileSystemModel>
#include <leetreemodel.h>
#include <QTextEdit>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
    class FbxOptions;
    class Maintain;
}
QT_END_NAMESPACE

enum DataPath
{
    LBatchMaya,
    LBlender,
    LSource,
    LExport,
    LMakeDir,
    LMesh,
    LSkeleton,
    LSoft,
    LKit
};

const QString LogsFileName = "ExportsLogs.plus";


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

    void OnSoftWereChanged(const QString& textchanged);

    void OnMakeDirChanged(int value);

    void OnDirectoryFilterLoader();

    void OnTestModel();
private:
    Ui::MainWindow *ui;
    Ui::FbxOptions *uiOpt;
    Ui::Maintain   *uiTree;

    QFileSystemModel* TreeSysV;
    LeeTreeModel* LeeModel=nullptr;
    QUndoStack* undo_stack=nullptr;

    QString logStr;
    QString ipSourceDir;
    QString ExportDir;
    QProcess* mProcess=nullptr;

    QStringList InitFillters();

    QStringList file3DFilter;
    //init Default
    void InitLocal();

    //Maya Files Blender Files
    QStringList MayaFiles,BlenderFiles;

    //List Source Files
    QStringList EpSourceFiles;
    int EpCount = 0;

    QList<QStringList> leeFilters;

    bool ValidPaths();

    void SaveRecentData(const QString inFilePath);

    void AddToExpLogs(const QString inFileTargetsqaeu2rdxsz);

    void SaveToLocal(DataPath inType,const QString inContent);

    void LoadRecentData();

    QJsonValue GetDataFromKey(const QString key);

    void SetCheckBox(QCheckBox* box , bool isChecked);

    void SetTextLine(QTextEdit* text ,const QString intext);

    void SetComboText(QComboBox* box ,const QString intext);


    QString localfilePath="C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/LeeMassFbx.json";

    QString GetMacAdress();

    bool StudioIsValid();

    bool IsValidMaya(const QString inProgramPath) {return inProgramPath.endsWith("mayabatch.exe");}

    bool IsValidBlender(const QString inProgramPath) {return inProgramPath.endsWith("blender.exe");}

    bool IsValidSoft();

    void AddToLog(QString inMessage,QString inColor="white",bool isClear=0);

    void AddToLog(const LogType inLog,QString inMessage,bool isClear=0);

    void ScrollToNewLog();

    void ResetMem();

    void OnStudioValidFailure();
protected:


    //ImpCmd* mImpCmd=nullptr;

    QList<ImpCmd*> ListCmds;

    void ImplementFbxOptions();

    void ImplementTreeView();

    void InitFileSysModel();

    QString GetExportPath(const QString inSourceFile,const QString inExportDir);

    SoftwereType GetSoftWareType();

    //info Env
    void InfoEnv();

    LeeSpoiler* Spoiler;

    LeeSpoiler* MaintainSpoiler;


    QPushButton* ExpotBtn;

    int SourceType=-1;

    QString LastCompletedFile;

    QString _Pc,_Users,_Host;
    bool isRunning=false;

    int TotalFiles=0;
    int TotalFbx;
    int completedId=0;
    bool isMultiThread=false;

    bool isError=false;

    void OnCmdFinish(int exitCode, QProcess::ExitStatus exitStatus,QStringList inFbxList);

    void OnCmdStarted();

    void ImplementExport(int fileNumber);

    void ExpNext();

    int FbxCompletedCount();

    void OnRevealFolder();

public slots:
    void OnLogs(QString &inLog,QString &Err);

    void OnTakeError(QProcess::ProcessError &Err);

    void OnStateChanged(QProcess::ProcessState newState);
private:
    bool IsAuthored;

};
#endif // MAINWINDOW_H
