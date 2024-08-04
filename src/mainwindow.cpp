#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_FbxOptions.h"
#include <qmessagebox.h>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , uiOpt(new Ui::FbxOptions)
{

    ui->setupUi(this);

    undo_stack = new QUndoStack(this);
    // QColor color =Qt::white;
    // QPalette palette = ui->menuFile->palette();
    // palette.setColor(QPalette::WindowText, color);
    // ui->menuView->setPalette(palette);

    ImplementFbxOptions();
    ui->progressBar->setValue(10);

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

    BlenderFiles << "*.blend";

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

    qDebug() << "EXTPORT File Running.." << Qt::endl;

    if(command){
        if(command->IsRunning()){
            QString OutLog =ui->LeeLog->toHtml();
            OutLog += "Running File : " + command->GetCSFile();
            ui->LeeLog->setHtml(OutLog);
            return;
        }
    }

    QFile melFile(MELEXPORTSCRIPT);
    QFile BlenderFile("");
    QFile LogFiles(MASSFBXLOG);
    //clear log
    ui->LeeLog->setPlainText(QString());

    //qDebug() << MELEXPORTSCRIPT << Qt::endl;
    //Define Log Str
    QString OutLog;


    SoftwereType SType = GetSoftWareType();

    //type None Error type
    if(SType==None) {

        /*Debug Something here*/
        return;
    }

    //create Filter List
    QStringList filters= InitFillters();

    //Debug Filters
    QString FilLog = "Export Type : ";
    for(auto ff : filters)
        FilLog+= ff;
    qDebug() << FilLog << Qt::endl;
    OutLog += FilLog + "<br>";
    //Get Source Files
    QStringList SFiles;
    GetFilesInDir(ipSourceDir,SFiles,filters);

    //Log Files Searching

    isRunning=true;
    //execute command current test 1 file
    for(int i=0;i<SFiles.count();++i)
    {
        if(i > 5) return;
        QString SFile = SFiles[i];
        QString ExFile = GetExportPath(SFile,ExportDir);

        //Execute Command
        command = new CommandLine();
        command->CreateProcess(SFile,ExFile,OutLog,SType);

        connect(&command->ExportProcess,&QProcess::finished,this,&MainWindow::OnFinish);
        connect(command,&CommandLine::SendCRFile,this,&MainWindow::Display);
        connect(command,&CommandLine::SendErrorStr,this,&MainWindow::DisplayErr);
        command->ExportProcess.waitForStarted();
        OutLog += "Exporting from : " + SFile + " to : " + ExFile;
        if(i <5) OutLog +="<br>";
        qDebug() << "file " << SFile << "number : " << SFiles.count() <<  Qt::endl;

        //ui->MayaText->setHtml("<font color=\"red\">Red text</font>");
        ui->LeeLog->setHtml(OutLog);
        ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());

        // command->ExportProcess.waitForFinished(-1);
        // OutLog += command->ExportProcess.readAllStandardOutput() + "\n";
        // OutLog += "Export Completed : " + SFile + "\n";
        // ui->LeeLog->setPlainText(OutLog);

    }

    command=nullptr;

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

QStringList MainWindow::InitFillters()
{
    if(uiOpt->comboBox == nullptr)
        SourceType=1;

    SourceType = uiOpt->comboBox->currentIndex();

    if(SourceType < 0) { return MayaFiles ;}

    switch (SourceType) {
        case 0:{
            return MayaFiles;
        }
        case 1:{
            return BlenderFiles; }
        case 2:{
            QStringList nFilter= MayaFiles;
            for(auto f : BlenderFiles)
            {
                nFilter.push_back(f);
            }
            return nFilter;
        }
    }

    return QStringList();
}

void MainWindow::Display(QString inReceiveFile,QString CSFile)
{
    QString OutLog =ui->LeeLog->toHtml();
    QFile fileExp(inReceiveFile);
    //<color:#ff0000=\"DeepPink\">
    QString Message = fileExp.exists() ? "<font color=\"green\">Export Completed : </font>" : "<font color=\"red\">Export Failure : </font>";
    QString FMessage = fileExp.exists() ? inReceiveFile : CSFile;
    OutLog += Message + FMessage;
    ui->LeeLog->setHtml(OutLog);
    ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());
    LastCompletedFile = CSFile;

}

void MainWindow::DisplayErr(QString ErStr)
{
    // QString OutLog =ui->LeeLog->toPlainText();
    // OutLog += "Err : " + ErStr + "\n";
    // ui->LeeLog->setPlainText(OutLog);
    qDebug() << "err : " << ErStr;

}

void MainWindow::GetFilesInDir(const QString inDir,QStringList &OutFiles,QStringList inFilters)
{
    QDir dir(inDir);
    if(!dir.exists()) return;

    QStringList files = dir.entryList(inFilters);

    QStringList folders = dir.entryList(QDir::Dirs);


    for(auto f : files){
        OutFiles.push_back(inDir + f);
    }

    for(auto fo : folders){
        if(fo.endsWith(".") || fo.endsWith("..")) continue;
        //qDebug() << inDir + fo;
        QString dirPath = inDir + fo + "/";
        GetFilesInDir(dirPath,OutFiles,inFilters);
    }
}

void MainWindow::OnFinish()
{
    QProcess process = qobject_cast<QProcess>(sender());

    if(process.isOpen())
        process.deleteLater();
}


void MainWindow::ImplementFbxOptions()
{

    Spoiler = new LeeSpoiler("LeeMassFbx Maya",100,this);
    uiOpt->setupUi(Spoiler);
    connect(uiOpt->comboBox,&QComboBox::currentIndexChanged,this,&MainWindow::OnComboBoxChanged);

    Spoiler->toggleButton->setStyleSheet("font: 700 11pt \"Sitka\"; color: rgb(255, 255, 255);");
    Spoiler->toggleButton->setAutoRaise(true);
    QVBoxLayout * vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();

    hlayout->addLayout(uiOpt->gridLayout);
    vlayout->addLayout(hlayout);
    Spoiler->setContentLayout(*vlayout);
   // ui->AuthorLayout->insertWidget(ui->AuthorLayout->count(),Spoiler);// ->addWidget(Spoiler);
    //ui->MainVLayout->layout()->addWidget(Spoiler);
    ui->verticalLayout_3->insertWidget(ui->verticalLayout_3->count()-2,Spoiler,Qt::AlignHCenter);
    //ui->verticalLayout_3->addWidget(Spoiler,Qt::AlignCenter);
}

QString MainWindow::GetExportPath(const QString inSourceFile, const QString inExportDir)
{
    if(inSourceFile.isEmpty() || inSourceFile.isNull()) return QString();

    QFile file(inSourceFile);
    if(!file.exists()) return QString();

    QString fpath = inSourceFile;

    fpath.replace("\\","/");

    QStringList splitStr= fpath.split("/");

    QString fname =splitStr[splitStr.count()-1];

    QString nPathDir = inExportDir + fname.left(fname.lastIndexOf(".")) + "/";

    QString FbxPath = nPathDir + fname.left(fname.lastIndexOf(".")) + ".fbx";

    //qDebug() << "fbxPaht : " << FbxPath << Qt::endl;

    //make dir export fbx
    QDir nDir(nPathDir);

    if(!nDir.exists())
        nDir.mkpath(nPathDir);

    //return fbx file exports
    return FbxPath;
}

SoftwereType MainWindow::GetSoftWareType()
{
    int SType = uiOpt->comboBox->currentIndex();

    switch (SType) {
        case 0: return Maya;
        case 1: return Blender;
        case 2: return MayaAndBlender;
    }
    return None;
}

void MainWindow::ExecuteExportFbx(QString inSourceFile, QString inExportDir)
{
    QString ExFile = GetExportPath(inSourceFile,inExportDir);

}

void MainWindow::OnComboBoxChanged(int valuechanged)
{
    qDebug() << "Value Changed " << valuechanged << Qt::endl;
    QString textChanged;
    switch (valuechanged) {
        case 0:{textChanged="LeeMassFbx Option Maya";break;}
        case 1:{textChanged="LeeMassFbx Option Blender";break;}
        case 2:{textChanged="LeeMassFbx Option Maya And Blender";break;}
    }

    Spoiler->toggleButton->setText(textChanged);
}

