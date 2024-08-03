#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <qmessagebox.h>
#include <QColorDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    undo_stack = new QUndoStack(this);
    QColor color =Qt::white;
    QPalette palette = ui->menuFile->palette();
    palette.setColor(QPalette::WindowText, color);
    ui->menuView->setPalette(palette);

    connect(ui->SourceFolderText,&QTextEdit::textChanged, this, &MainWindow::OnTextChanged);
    connect(ui->ExportFolderText,&QTextEdit::textChanged, this, &MainWindow::OnTextChanged);


    connect(ui->SourceBrowserBtn,&QPushButton::clicked, this, &MainWindow::OnBrowserClicked);

    connect(ui->ExportBrowserBTn,&QPushButton::clicked,this,&MainWindow::OnBrowserClicked);
    connect(ui->ExportExecute,&QPushButton::clicked,this,&MainWindow::OnExportClicked);

    mProcess = new QProcess();

    //set Default testing folder;
    ui->SourceFolderText->setText("C:/Users/thang/Documents/");
    ui->ExportFolderText->setText("C:/Users/thang/Documents/Exports/");

    MayaFiles <<"*.ma" << "*.mb";

}

MainWindow::~MainWindow()
{
    command->StopThread();
    delete ui;
}

void MainWindow::OnTextChanged()
{

    //get Object
    QTextEdit* senderText = qobject_cast<QTextEdit*>(sender());
    //get Text
    QString dir = senderText->toPlainText();
    if(dir.isNull()|| dir.isEmpty()) return;

    //check folder Exists message log
    if(!QDir(dir).exists()) {
        QString log = "Source Folder : " + dir + " does'nt exists.\n";
        ui->LeeLog->setPlainText(log);
        qDebug() << log << Qt::endl;
        return;
    }

    //assign varible
    if(senderText->objectName()==ui->SourceFolderText->objectName())
        ipSourceDir = dir;
    else
        ExportDir = dir;
}

void MainWindow::OnBrowserClicked()
{
     qDebug() << "browser clicked.." << Qt::endl;
    QFileDialog* fdialog = new QFileDialog();
    fdialog->setFileMode(QFileDialog::Directory);
    fdialog->setOption(QFileDialog::ShowDirsOnly);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "C:/",
                                                    QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

    qDebug() << dir << Qt::endl;

    if(!dir.isNull() || dir.isEmpty()){
        QPushButton* const senderButton= qobject_cast<QPushButton*>(sender());
        if(senderButton){
            //qDebug() << senderButton->objectName() << Qt::endl;
            if(senderButton->objectName()== ui->SourceBrowserBtn->objectName())
            {
                ui->SourceFolderText->setText(dir);
            }
            else
                ui->ExportFolderText->setText(dir);
        }
        // ui->SourceFolderText->setText(dir);
        // ui->SourceFolderText->setEnabled(false);
        // ImplanteTreeView(dir);
        // lCurrentDirName = dir;
        // lDir.setPath(dir);
    }

}

void MainWindow::OnExportClicked()
{

    // QStringList filters = state > 0 ? l3DFiles : lallFiles;
    // fileSystemModel->setNameFilters(filters);
    // lDir.setNameFilters(filters);

    qDebug() << "EXTPORT File..." << Qt::endl;

    //QDir dir = QDir(ExportDir);

    //if(command==nullptr)
    //command = new CommandLine(this);

    //QString batchMaya = "C:\\Program Files\\Autodesk\\Maya2019\\bin\\mayabatch.exe -file \"C:/Users/thang/Documents/textFbx.ma\" -command \"file -force -options \"v=0;\" -type \"FBX export\" -pr -ea \"C:/Users/thang/Documents/Exports/abc.fbx\"";
    // QString ExportCmd = "\"C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe\" -file \"C:/Users/thang/Documents/textFbx.ma\" -command \"file -force -options \\\"v=0\\\" -type \\\"FBX export\\\" -pr -ea \\\"C:/Users/thang/Documents/Exports/abc.fbx\\\"";

    QFile melFile("C:/Users/thang/Documents/GitHub/LeeAutoExportFBX/MayaExportCmd.mel");
    QFile LogFiles("C:/Users/thang/Documents/GitHub/LeeAutoExportFBX/LeeLog.txt");


    QString Cmd = GeneratedCommand(ui->SourceFolderText->toPlainText() + "textFbx.ma" , ui->ExportFolderText->toPlainText() + "abc.fbx");

    qDebug() << Cmd << Qt::endl;

    if(!melFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", melFile.errorString());
    }

    QString mProgram = "C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe";
    QString ShellCmd = " -file \"C:/Users/thang/Documents/textFbx.ma\"";//@"C:/EpicSources/UE_SOURCE_5.3/UnrealEngine/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe"  -projectfiles -project="E:/UEProjects/LeeAnonymous/LeeRikARPG_UShow.uproject" -game -LeeRikARPG_UShow -progress" ;



    QStringList params = QStringList() << Cmd;

    if(mProcess==nullptr)
        mProcess = new QProcess(this);


    QProcess xProcess;
    //qDebug() << melStream.readAll() << Qt::endl;

    //mProcess->start(ExportCmd);

    // const char* cmdStr = batchMaya.toLocal8Bit();
    //qDebug() << cmd2 << Qt::endl;

    const char* cmdStr = Cmd.toLocal8Bit();

    // mProcess->setProgram("cmd.exe");
   // mProcess->startDetached(mProgram,params);
    //mProcess->execute(ShellCmd);

    //system(ShellCmd.toLocal8Bit());
    xProcess.start(mProgram,params);

    xProcess.waitForFinished(-1);
    // QString error = mProcess->readAllStandardError();
    // qDebug() << error << Qt::endl;

    QString stdoutStr = xProcess.readAllStandardOutput();

    ui->LeeLog->setPlainText(stdoutStr);
    //Write to log file
    if(LogFiles.open(QIODevice::WriteOnly)){
        LogFiles.write(stdoutStr.toLocal8Bit());
        LogFiles.close();
    }

    QString stderrStr = xProcess.readAllStandardError();

    // qDebug() << stdoutStr << Qt::endl;
    // qDebug() << stderrStr << Qt::endl;

    // if(xProcess.waitForStarted()){
    //     qDebug() << "Starting";

    // }
    // if(xProcess.isOpen()){
    //     QString result = xProcess.readAllStandardOutput();
    //     qDebug() << result << Qt::endl;
    // }



    qDebug() << "finish";
    mProcess=nullptr;


    // attach the new console to this application’s process
    //AttachConsole(process->processId());
    //show_console();
}



QString MainWindow::GeneratedCommand(const QString inSourceFile, const QString inExportFile)
{
    if(inSourceFile.isEmpty() || inExportFile.isNull()) return QString();

    //Maya 2019 Working
    QString cmd = inSourceFile + "\" ;\n" ;

    QFile melFile("C:/Users/thang/Documents/GitHub/LeeAutoExportFBX/MayaExportCmd.mel");
    if(!melFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", melFile.errorString());
    }

    QTextStream melStream(&melFile);

    cmd += melStream.readAll();//"file -force -options \"v=0\" -type \"FBX export\" -pr -ea \"C:/Users/thang/Documents/Exports/abc";

    QStringList oFiles;

    GetFilesInDir(ipSourceDir,oFiles);

    for(auto mf : oFiles)
        qDebug() << mf;

    return cmd;
}

void MainWindow::GetFilesInDir(const QString inDir,QStringList &OutFiles)
{
    QDir dir(inDir);
    if(!dir.exists()) return;

    QStringList files = dir.entryList(MayaFiles);

    QStringList folders = dir.entryList(QDir::Dirs);


    for(auto f : files){
        OutFiles.push_back(f);
    }

    for(auto fo : folders){
        if(fo.endsWith(".") || fo.endsWith("..")) continue;
        //qDebug() << inDir + fo;
        QString dirPath = inDir + fo + "/";
        GetFilesInDir(dirPath,OutFiles);
    }
}
