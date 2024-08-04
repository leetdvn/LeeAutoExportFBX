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

    qDebug() << "Type Export " << SType <<Qt::endl;
    OutLog += "Type Export " + QString::number(SType) + "\n";
    //create Filter List
    QStringList filters;
    InitFillters(filters);

    //Debug Filters
    QString FilLog = "Filter type : ";
    for(auto ff : filters)
        FilLog+= ff;
    qDebug() << FilLog << Qt::endl;
    OutLog += FilLog + "\n";
    //Get Source Files
    QStringList SFiles;
    GetFilesInDir(ipSourceDir,SFiles,filters);

    //Log Files Searching
    for(auto f : SFiles){
        OutLog += f + "\n";
        qDebug() << f <<Qt::endl;
    }

    //Test Source Files
    QString SFile = SFiles[0];
    QString ExFile = GetExportPath(SFile,ExportDir);
    OutLog = "Loading File : " + SFile + "\n";

    //fbx exist file
    if(!QFile(SFile).exists()){
        //Source file does not exists
        return;
    }

    QFile FbxFile(ExFile);
    if(FbxFile.exists()){
        //file exist need ovverride or next file;

    }


    //Execute Command
    // CommandLine* command = new CommandLine();
    // command->CreateProcess(SFile,ExFile,OutLog,SType);

    // connect(&command->ExportProcess,&QProcess::finished,this,&MainWindow::OnReadAble);
    // command->ExportProcess.waitForStarted();
    // ui->LeeLog->setPlainText(OutLog);


    // command->ExportProcess.waitForFinished(-1);
    // OutLog += command->ExportProcess.readAllStandardOutput();
    // ui->LeeLog->setPlainText(OutLog);





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

void MainWindow::InitFillters(QStringList &OutFilters)
{
    if(uiOpt->comboBox == nullptr)
        SourceType=1;

    SourceType = uiOpt->comboBox->currentIndex();

    if(SourceType < 0) {
        OutFilters = MayaFiles;
        return;
    }

    switch (SourceType) {
        case 0:{
            OutFilters = MayaFiles;
            return ;
        }
        case 1:{
            OutFilters = BlenderFiles;
            return;
        }
        case 2:{
            for(auto f : BlenderFiles)
            {
                MayaFiles.push_back(f);
            }
            OutFilters = MayaFiles;
            return ;
        }
    }

    return ;
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

void MainWindow::OnReadAble()
{
    QString Out = "Finish...";

    QProcess process = qobject_cast<QProcess>(sender());

    if(process.isOpen())
        process.deleteLater();
    ui->LeeLog->setPlainText(Out);
    //qDebug() << "ReadAble " << Qt::endl;
}


void MainWindow::ImplementFbxOptions()
{

    Spoiler = new LeeSpoiler("MassFbx Options",100,this);
    uiOpt->setupUi(Spoiler);
    uiOpt->comboBox->setCurrentIndex(1);
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

    qDebug() << "fbxPaht : " << FbxPath << Qt::endl;

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

void MainWindow::OnComboBoxChanged(int valuechanged)
{
    qDebug() << "Value Changed " << valuechanged << Qt::endl;
}
