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

    //create Filter List
    QStringList filters= InitFillters();

    //Debug Filters
    QString FilLog = "Export Type : ";
    for(auto ff : filters)
        FilLog+= ff;
    qDebug() << FilLog << Qt::endl;
    OutLog += FilLog + "\n";
    //Get Source Files
    QStringList SFiles;
    GetFilesInDir(ipSourceDir,SFiles,filters);

    //Log Files Searching

    //execute command current test 1 file
    for(int i=0;i<SFiles.count();++i)
    {
        if(i > 2) return;
        QString SFile = SFiles[i];
        QString ExFile = GetExportPath(SFile,ExportDir);

        //Execute Command
        CommandLine* command = new CommandLine();
        command->CreateProcess(SFile,ExFile,OutLog,SType);

        connect(&command->ExportProcess,&QProcess::finished,this,&MainWindow::OnFinish);
        command->ExportProcess.waitForStarted();
        OutLog += "Exporting from : " + SFile + " to : " + ExFile + "\n";
        qDebug() << "file " << SFile << "number : " << SFiles.count() <<  Qt::endl;
        ui->LeeLog->setPlainText(OutLog);
        emit OnCompleted(command->CRFile);
    }

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
    QString Out = ui->LeeLog->toPlainText();

    QProcess process = qobject_cast<QProcess>(sender());

    QString CRFile = process.property("CRFile").toString();
    if(process.isOpen())
        process.deleteLater();

    // Out +="Export Completed : " + CRFile + "\n";
    // ui->LeeLog->setPlainText(Out);
    // qDebug() << "ReadAble " << Qt::endl;
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

void MainWindow::OnCompleted(QString comletedfile)
{
    QString Out = ui->LeeLog->toPlainText();
    Out+="Export Completed : " + comletedfile + "\n";
    ui->LeeLog->setPlainText(Out);

}
