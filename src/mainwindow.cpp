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

    qDebug() << "EXTPORT File..." << Qt::endl;


    QFile melFile(MELEXPORTSCRIPT);
    QFile BlenderFile("");
    QFile LogFiles(MASSFBXLOG);


    QString Cmd = GeneratedCommand(ui->SourceFolderText->toPlainText() + "textFbx.ma" , ui->ExportFolderText->toPlainText() + "abc.fbx");

    qDebug() << MELEXPORTSCRIPT << Qt::endl;

    // if(!melFile.open(QIODevice::ReadOnly))
    // {
    //     QMessageBox::information(0, "error", melFile.errorString());
    // }

    QString mProgram = "C:/Program Files/Autodesk/Maya2019/bin/mayabatch.exe";
    QString OutLog;

    // QStringList params = QStringList() << Cmd;

    // if(mProcess==nullptr)
    //     mProcess = new QProcess(this);


    // QProcess xProcess;

    // xProcess.start(mProgram,params);

    // xProcess.waitForFinished(-1);


    // QString stdoutStr = xProcess.readAllStandardOutput();

    // ui->LeeLog->setPlainText(stdoutStr);
    // //Write to log file
    // if(LogFiles.open(QIODevice::WriteOnly)){
    //     LogFiles.write(stdoutStr.toLocal8Bit());
    //     LogFiles.close();
    // }

    // QString stderrStr = xProcess.readAllStandardError();

    // qDebug() << "finish";
    // mProcess=nullptr;

    QStringList SFiles;

    GetFilesInDir(ipSourceDir,SFiles);


    QString SFile = ui->SourceFolderText->toPlainText() + "textFbx.ma";
    QString ExFile = ui->ExportFolderText->toPlainText() + "abc.fbx";
    CommandLine* command = new CommandLine();
    command->CreateProcess(SFile,ExFile,OutLog);

    //mProcess->waitForStarted(-1);
    ui->LeeLog->setPlainText(OutLog);


}



QString MainWindow::GeneratedCommand(const QString inSourceFile, const QString inExportFile)
{
    if(inSourceFile.isEmpty() || inExportFile.isNull()) return QString();

    //Maya 2019 Working
    QString cmd = inSourceFile + "\" ;\n" ;

    QFile melFile("C:/Users/thang/Documents/GitHub/LeeAutoExportFBX/Scripts/MayaExportCmd.mel");
    if(!melFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "error", melFile.errorString());
    }

    QTextStream melStream(&melFile);

    cmd += melStream.readAll();//"file -force -options \"v=0\" -type \"FBX export\" -pr -ea \"C:/Users/thang/Documents/Exports/abc";


    return cmd;
}

void MainWindow::GetFilesInDir(const QString inDir,QStringList &OutFiles)
{
    QDir dir(inDir);
    if(!dir.exists()) return;

    QStringList files = dir.entryList(MayaFiles);

    QStringList folders = dir.entryList(QDir::Dirs);


    for(auto f : files){
        OutFiles.push_back(inDir + f);
    }

    for(auto fo : folders){
        if(fo.endsWith(".") || fo.endsWith("..")) continue;
        //qDebug() << inDir + fo;
        QString dirPath = inDir + fo + "/";
        GetFilesInDir(dirPath,OutFiles);
    }
}
