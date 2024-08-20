#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_FbxOptions.h"


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
    ui->progressBar->setValue(0);

    connect(ui->SourceFolderText,&QTextEdit::textChanged, this, &MainWindow::OnTextChanged);
    connect(ui->ExportFolderText,&QTextEdit::textChanged, this, &MainWindow::OnTextChanged);

    connect(ui->MayaText,&QTextEdit::textChanged, this, &MainWindow::OnSoftWareChanged);
    connect(ui->BlenderText,&QTextEdit::textChanged, this, &MainWindow::OnSoftWareChanged);
    //connect(ui->ExportFolderText,&QTextEdit::textChanged, this, &MainWindow::OnTextChanged);


    connect(ui->SourceBrowserBtn,&QPushButton::clicked, this, &MainWindow::OnBrowserFolder);
    connect(ui->ExportBrowserBTn,&QPushButton::clicked,this,&MainWindow::OnBrowserFolder);

    connect(ui->MayaFileBtn,&QPushButton::clicked, this, &MainWindow::OnBrowserFile);
    connect(ui->BlenderFileBtn,&QPushButton::clicked,this,&MainWindow::OnBrowserFile);

    connect(ui->ExportExecute,&QPushButton::clicked,this,&MainWindow::OnExportClicked);

    mProcess = new QProcess();

    //Info Env
    InfoEnv();


    //init default
    InitLocal();

    //set Default testing folder;
    // ui->SourceFolderText->setText("C:/Users/leepl/Documents/");
    // ui->ExportFolderText->setText("C:/Users/leepl/Documents/Exports/");
    LoadRecentData();

    //filter Maya
    MayaFiles <<"*.ma" << "*.mb";

    //filter Blender
    BlenderFiles << "*.blend";

    if(ui->LeeLog->toHtml().isEmpty()){
        ui->LeeLog->setHtml("");
    }
}

#pragma region Init
void MainWindow::InfoEnv()
{
    //init Default
    _Users = qgetenv("USERNAME");
    _Host = QHostInfo::localDomainName();
    _Pc = qgetenv("COMPUTERNAME");

    qDebug() << "Pc : " << _Pc ;
    qDebug() << "Users : " << _Users;

    if(_Host !="")
        qDebug() << "Host : " << _Host << Qt::endl;
}

void MainWindow::InitLocal()
{
    //init List Dir
    QStringList MassDirs = {"LeeMassFbx","LeeMassFbx/Scripts","LeeMassFbx/Logs"};
    //init Defaults

    QString baseDir = "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/";
    for(auto d : MassDirs){
        QString nDir = baseDir + d;
        QDir dir(nDir);
        if(!dir.exists())
            dir.mkdir(nDir);
    }

    QString localPath = "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/";

    // QDir lDir(localPath);
    // QDir LogDir(localPath + "Logs/");
    // if(!lDir.exists()) lDir.mkdir(localPath);
    // if(!LogDir.exists()) LogDir.mkdir(localPath + "Logs/");

    QFile lfile(localfilePath);
    if(!lfile.exists()){
        if(lfile.open(QIODevice::ReadWrite))
            lfile.close();
    }

    //Base Path
    QString MelPath = QDir::currentPath() + QString("/Scripts/MayaExportCmd.mel"); // CommandLine::GetMelCommand(QString("Scripts/MayaExportCmd.mel"));
    QString Mel = CommandLine::GetMelCommand(MelPath);


    QFile melfile(MELEXPORTSCRIPT);
    if(melfile.open(QIODevice::NewOnly)){
        qDebug() << "Blender File : " << Mel << Qt::endl;
        melfile.write(Mel.toLocal8Bit());
        melfile.close();
    }

    QString bPythonPath = QDir::currentPath() +  QString("/Scripts/BlenderExport.py");//CommandLine::GetMelCommand(QString("Scripts/BlenderExport.py"));
    QString bPython = CommandLine::GetMelCommand(bPythonPath);


    //Copy Scripts
    QFile Blender(BLENDERSMARTEXPORT);
    if(Blender.open(QIODevice::NewOnly)){
        qDebug() << "Blender File : " << bPython << Qt::endl;
        Blender.write(bPython.toLocal8Bit());
        Blender.close();
    }

    QFile fbxlog(MASSFBXLOG);
    if(!fbxlog.exists()){
        if(fbxlog.open(QIODevice::ReadWrite| QIODevice::NewOnly)){
            QString info = QString("PC : %1\nUSER : %2\nDOMAIN : %3\n").arg(_Pc,_Users,_Host);
            fbxlog.write(info.toLocal8Bit());
            fbxlog.close();
        }
    }
}

#pragma endregion //

MainWindow::~MainWindow()
{
    if(MassCmds.length() > 0)
    {
        for(auto* cmd : MassCmds){
            cmd->deleteLater();
        }
    }
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

        //"<font color=\"red\">Cannot use unauthorized tools</font>";
        //ui->LeeLog->setPlainText(log);
        AddToLog(log,"red");
        qDebug() << log << Qt::endl;
        return;
    }

    //assign varible
    if(senderText->objectName()==ui->SourceFolderText->objectName())
        ipSourceDir = dir;
    else
        ExportDir = dir;
}

void MainWindow::OnSoftWareChanged()
{
    if(isRunning) return;

    QTextEdit* softw = qobject_cast<QTextEdit*>(sender());

    QString softStr = softw->toPlainText();
    QString Wrongsoftlog ;
    //maya Button or blender Soft
    if(softw->objectName() == ui->MayaText->objectName())
    {
        if(!IsValidMaya(softStr))
            Wrongsoftlog  = "this softwave is not valid choise mayabatch.exe file";
    }
    else if(softw->objectName() == ui->BlenderText->objectName()){
        if(!IsValidBlender(softStr))
            Wrongsoftlog  = "this softwave is not valid choise blender.exe file";
    }

    AddToLog(Wrongsoftlog,"yellow");
    //Set Vertical Scroll to end value
    ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());

}

void MainWindow::OnBrowserFolder()
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


    if(!dir.isNull() || !dir.isEmpty()){
        QPushButton* const senderButton= qobject_cast<QPushButton*>(sender());
        if(senderButton){
            dir+="/";
            //qDebug() << senderButton->objectName() << Qt::endl;
            if(senderButton->objectName()== ui->SourceBrowserBtn->objectName())
            {

                ui->SourceFolderText->setText(dir);
                SaveToLocal(LSource,dir);
            }
            else{
                ui->ExportFolderText->setText(dir);
                SaveToLocal(LExport,dir);
            }
        }
    }
}

void MainWindow::OnBrowserFile()
{
    QPushButton* const senderButton= qobject_cast<QPushButton*>(sender());

    QString wTitle = senderButton->objectName().startsWith("Maya") ?
                        "Open Maya File" :
                        "Open Blender File";

    QString Soft = senderButton->objectName().startsWith("Maya") ?
                        tr("MayaBatch File (*.exe)"):
                        tr("Blender File (*.exe)");

    QString baseDir = "C:/Program Files/";
    QString Dir  = senderButton->objectName().startsWith("Maya") ?
                        baseDir + "Autodesk/" :
                        baseDir + "Blender Foundation/";
    QFileDialog* fdialog = new QFileDialog();
    fdialog->setFileMode(QFileDialog::Directory);
    fdialog->setOption(QFileDialog::ShowDirsOnly);

    if(QDir(Dir).exists()) baseDir= Dir;

    QString file = QFileDialog::getOpenFileName(this, wTitle,baseDir,Soft);

    DataPath type = senderButton->objectName().startsWith("Maya") ? LBatchMaya : LBlender;
    SaveToLocal(type,file);

    qDebug() << file << Qt::endl;

    QTextEdit* textE = nullptr;

    if(senderButton){
        textE = senderButton->objectName().startsWith("Maya") ? ui->MayaText : ui->BlenderText;
    }

    if(!file.isNull() || file.isEmpty()){
        qDebug() << senderButton->objectName() << Qt::endl;
    }

    if(textE !=nullptr)
        textE->setPlainText(file);


}

void MainWindow::OnExportClicked()
{

    qDebug() << "EXTPORT File Running.." << Qt::endl;
    QString OutLog;

    if(!StudioIsValid()){
        OutLog = "<font color=\"red\">Cannot use unauthorized tools</font>";
        ui->LeeLog->setHtml(OutLog);
        return;
    }
    else{
        OutLog += "<font color=\"green\">Welcome To Plus Stuido MassExport Fbx Software</font>";
        ui->LeeLog->setHtml(OutLog);
    }

    //Valid Soft Ware path name
    if(!IsValidSoft()) return;


    //check Running status
    if(command){
        if(command->IsRunning() || !isRunning){
            QString OutLog =ui->LeeLog->toHtml();
            OutLog += "Running File : " + command->GetCSFile();
            ui->LeeLog->setHtml(OutLog);
            return;
        }
    }

    if(!uiOpt->DebugBox->isChecked())
        ClearScripts();

    QFile melFile(MELEXPORTSCRIPT);
    QFile BlenderFile("");
    QFile LogFiles(MASSFBXLOG);
    //clear log
    ui->LeeLog->setPlainText(QString());

    //qDebug() << MELEXPORTSCRIPT << Qt::endl;
    //Define Log Str


    SoftwereType SType = GetSoftWareType();

    //create Filter List
    QStringList filters= InitFillters();

    //Debug Filters
    QString FilLog =QString("PC : %1 <br>USERS : %2 <br>DOMAIN : %3").arg(_Pc,_Users,_Host);
    FilLog+= "<br>Export Type : ";
    for(auto ff : filters)
        FilLog+= ff;
    qDebug() << FilLog << Qt::endl;
    logStr += FilLog + "<br>";
    //Get Source Files
    GetFilesInDir(ipSourceDir,EpSourceFiles,filters);

    //Log Files Searching
    qDebug() << "Files Count " << EpSourceFiles.count() << Qt::endl;
    qDebug() << "ip Source " << ipSourceDir << Qt::endl;
    ui->progressBar->setValue(0);
    TotalFiles = EpSourceFiles.count();

    isRunning = true;
    completedId = 0;
    ui->ExportExecute->setEnabled(false);

    mMaya = new MayaCmd(ui->MayaText->toPlainText(),EpSourceFiles[EpCount],ui->ExportFolderText->toPlainText());
    connect(mMaya,&MayaCmd::OnFinish,this,&MainWindow::OnMayaFinish);

    //ExecuteExportFbx(EpCount);

    //execute command current test 1 file
    // #pragma omp parallel for
    // {
    //     for(int i=0;i<EpSourceFiles.count();++i)
    //     {
    //         QString SFile = EpSourceFiles[i];

    //         //make Path file
    //         QString ExFile = GetExportPath(SFile,ExportDir);

    //         //Execute Command
    //         command = new CommandLine();
    //         MassCmds.push_back(command);
    //         command->SetMayaPro(ui->MayaText->toPlainText());
    //         command->SetBlenderPro(ui->BlenderText->toPlainText());
    //         command->SetCommandId(i +1);
    //         EpCount++;
    //         //debug mode
    //         if(!uiOpt->DebugBox->isChecked()) command->SetClearOnComplete(true);

    //         command->CreateProcess(SFile,ExFile,OutLog,SType);

    //         connect(command,&CommandLine::SendErrorStr,this,&MainWindow::DisplayErr);
    //         connect(&command->ExportProcess,&QProcess::finished,this,&MainWindow::OnFinish);
    //         connect(command,&CommandLine::SendCRFile,this,&MainWindow::Display);
    //         connect(command,&CommandLine::SendId,this,&MainWindow::OnCompletedId);
    //         command->ExportProcess.waitForStarted();
    //         OutLog += "Exporting from : " + SFile + " to : " + ExFile;

    //         if(i !=EpSourceFiles.count()) OutLog +="<br>";
    //         qDebug() << "file " << SFile << "number : " << EpSourceFiles.count() <<  Qt::endl;

    //         //ui->MayaText->setHtml("<font color=\"red\">Red text</font>");
    //         ui->LeeLog->setHtml(OutLog);
    //         ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());

    //         // command->ExportProcess.waitForFinished(-1);
    //         // OutLog += command->ExportProcess.readAllStandardOutput() + "\n";
    //         // OutLog += "Export Completed : " + SFile + "\n";
    //         // ui->LeeLog->setPlainText(OutLog);

    //     }
    // }

    command=nullptr;

}

QString MainWindow::GeneratedCommand(const QString inSourceFile, const QString inExportFile)
{
    if(inSourceFile.isEmpty() || inExportFile.isNull()) return QString();

    //Maya 2019 Working
    QString cmd = inSourceFile + "\" ;\n" ;

    //File
    QString fpath = "";
    //Base Script  Source
    QFile melFile("Scripts/MayaExportCmd.mel");
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

bool MainWindow::ValidPaths()
{
    QStringList paths ={
        ui->MayaText->toPlainText(),
        ui->BlenderText->toPlainText(),
        ui->SourceFolderText->toPlainText(),
        ui->ExportFolderText->toPlainText()
    };

    for(auto f: paths){
        if(f.endsWith(".exe")){
            qDebug() << "Valid Path : " << f << Qt::endl;
            if(!QFile(f).exists()) return false;
        }
        else{
            qDebug() << "Valid Path : " << f << Qt::endl;
            if(!QDir(f).exists()) return false;
        }
    }
    return true;
}

void MainWindow::SaveRecentData(const QString inFilePath)
{
    QJsonObject obj;
    obj["BatchMaya"]= ui->MayaText->toPlainText();
    obj["Blender"]= ui->BlenderText->toPlainText();
    obj["SourcePath"]= ui->SourceFolderText->toPlainText();
    obj["ExportPath"]= ui->ExportFolderText->toPlainText();

    QJsonDocument jdoc;
    jdoc.setObject(obj);

    QFile file(inFilePath);

    if(file.open(QIODevice::ReadWrite)){
        file.write(jdoc.toJson());
        file.close();
    }
}

void MainWindow::SaveToLocal(DataPath inType,const QString inContent)
{

    QJsonObject jObj = LoadObjectFromFile(localfilePath);

    QString Session;

    switch (inType) {
        case LBatchMaya:{
            Session="BatchMaya";
            break;
        }
        case LBlender:{
            Session="Blender";
            break;
        }
        case LSource:{
            Session="SourcePath";
            break;
        }
        case LExport:{
            Session="ExportPath";
            break;
        }
    }

    qDebug() << "ip : " << inType << "Sess : " << Session << "path : " << inContent <<  Qt::endl;
    jObj[Session] = inContent;

    QJsonDocument jdoc;
    jdoc.setObject(jObj);

    QFile file(localfilePath);

    if(file.open(QIODevice::WriteOnly)){
        file.write(jdoc.toJson());
        file.close();
    }
}

void MainWindow::LoadRecentData()
{
    QJsonDocument jdoc;
    QFile jfile(localfilePath);
    if(jfile.exists()){
        if(jfile.open(QIODevice::ReadOnly))
        {
            QString content = jfile.readAll();
            jdoc=QJsonDocument::fromJson(content.toUtf8());
        }
        jfile.close();
    }

    QJsonObject jobj = jdoc.object();
    QJsonValue maya = jobj.value("BatchMaya");
    QJsonValue blender = jobj.value("Blender");
    QJsonValue source = jobj.value("SourcePath");
    QJsonValue exp = jobj.value("ExportPath");
    ui->MayaText->setText(maya.toString());
    ui->BlenderText->setText(blender.toString());
    ui->SourceFolderText->setText(source.toString());
    ui->ExportFolderText->setText(exp.toString());
}

QJsonObject MainWindow::LoadObjectFromFile(const QString infile)
{
    QJsonDocument jdoc;
    QFile jfile(infile);
    if(jfile.exists()){
        if(jfile.open(QIODevice::ReadOnly))
        {
            QString content = jfile.readAll();
            jdoc=QJsonDocument::fromJson(content.toUtf8());
        }
        jfile.close();
    }

    return jdoc.object();
}

QString MainWindow::GetMacAdress()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;
    for (int i = 0; i < ifaces.count(); i++){
        QNetworkInterface iface = ifaces.at(i);
        if (iface.hardwareAddress() != "") { return iface.hardwareAddress(); }
    }
    return QString();
}

bool MainWindow::StudioIsValid()
{
    qDebug() << GetMacAdress() << Qt::endl;
    //My Mac Supervisor
    if(GetMacAdress() == "10:7C:61:47:26:B1" || GetMacAdress() == "04:7C:16:E3:94:DB") {return true;}
    //Studio PC Domain Name
    if(_Host == "giaoduc.edu") return true;

    QString LogAuthor = "<font color=\"red\"> Error : You cannot use the software without the consent of Plus Studio. </font>";

    AddToLog(LogAuthor);

    return false;
}

bool MainWindow::IsValidSoft()
{
    QString SoftOptions = uiOpt->comboBox->currentText();
    bool result = false;
    QString CLog;//= ui->LeeLog->toHtml();

    if(SoftOptions == "Maya") {
        QString MayaPath = ui->MayaText->toPlainText();
        if(!IsValidPath(MayaPath)){
            CLog += MayaPath +  " file does'nt exists.";
        }

        result = IsValidMaya(ui->MayaText->toPlainText());
        if(!result){
            CLog += "Maya Path is Not SoftWave";

        }
    }

    else if(SoftOptions == "Blender") {
        result = IsValidBlender(ui->BlenderText->toPlainText());
        if(!result){
            CLog += "Blender Path is Not SoftWave";
            AddToLog(CLog,"red");
            return result;
        }
    }
    else{
        result= IsValidMaya(ui->MayaText->toPlainText()) && IsValidBlender(ui->BlenderText->toPlainText());

        if(!result){
            if(!IsValidMaya(ui->MayaText->toPlainText())){
                CLog += "Choise mayabatch.exe ";
            }
            if(!IsValidBlender(ui->BlenderText->toPlainText())){
                CLog+=" Choise blender.exe location ";
            }

        }
    }

    //if not Validation
    if(!result) AddToLog(CLog,"red");

    return result;
}

void MainWindow::AddToLog(QString inMessage,QString inColor)
{
    if(inMessage =="") return;

    QString CLog = ui->LeeLog->toHtml();

    CLog += "<font color=\"" + inColor + "\"> " +  inMessage +  "</font>";
    ui->LeeLog->setHtml(CLog);
    ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());

}

void MainWindow::Display(QString inReceiveFile,QString CSFile)
{
    if(isError) return;
    logStr =ui->LeeLog->toHtml();
    QFile fileExp(inReceiveFile);
    //<color:#ff0000=\"DeepPink\">
    logStr+= fileExp.exists() ? "<font color=\"green\">Export Completed : </font>" : "<font color=\"red\">Export Failure : </font>";
    logStr+= fileExp.exists() ? inReceiveFile : CSFile;

    //qDebug() << "id : " << completedId << "total : " << TotalFiles;
    if(completedId == TotalFiles-1)
        logStr += "<br><font color=\"yellow\">MassExport Total : " +  QString::number(TotalFiles) +  " files.</font>";

    ui->LeeLog->setHtml(logStr);

    //sctorll bar update to end vertical
    ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());
    LastCompletedFile = CSFile;
}

void MainWindow::DisplayErr(QString ErStr)
{

    //Report Error message
    QString LogErr = "<font color=\"red\"> Error : " + ErStr +  " </font>";
    QString LogNotFound;

    CommandLine* cmdLine = qobject_cast<CommandLine*>(sender());
    if(!cmdLine) return;

    if(ErStr !=""){
        if(ErStr.endsWith("\"ExportCollection\" not found'\r\n")){
            LogNotFound= "<font color=\"purple\"> Blender File : " + cmdLine->GetCSFile() +  " Export Collections is not found. </font>";
        }
        else if(ErStr.startsWith("Warning"))
        {
            LogNotFound = "<font color=\"yellow\"> Warning </font>";
        }

        isError = ErStr.endsWith("\"ExportCollection\" not found'\r\n") ? true : false;
        AddToLog(LogNotFound);
    }
    qDebug() << "Err : " << ErStr << Qt::endl;
}

void MainWindow::OnCompletedId(int Id)
{
    completedId+=1;
    int value = (100.0/TotalFiles) * completedId;
    ui->progressBar->setValue(value);
    if(value >=99){
        ui->progressBar->setValue(100);
    }

    isRunning = completedId == TotalFiles ? false : true;

    //qDebug() << "id  : " << completedId << "total : " << TotalFiles;

    ui->ExportExecute->setEnabled(!isRunning);
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
        QString dirPath = inDir + fo + "/";
        GetFilesInDir(dirPath,OutFiles,inFilters);
    }
}

void MainWindow::OnFinish()
{
    QProcess process = qobject_cast<QProcess>(sender());

    qDebug() << "Count : " << EpCount << Qt::endl;
    //mMaya.InItProgram(ui->MayaText->toPlainText());

    //new Designs
    if(EpCount < EpSourceFiles.count())
    {
        //return ExecuteExportFbx(EpCount);

    }
    else{
        process.deleteLater();
    }
    // if(process.isOpen())
    //     process.deleteLater();

}

void MainWindow::ImplementFbxOptions()
{

    Spoiler = new LeeSpoiler("Settings Maya",100,this);
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

    bool makedir = uiOpt->makedirBox->isChecked();
    QString nPathDir = makedir ? inExportDir + fname.left(fname.lastIndexOf(".")) + "/" : inExportDir ;

    QString FbxPath = nPathDir + fname.left(fname.lastIndexOf(".")) + ".fbx";

    //qDebug() << "fbxPaht : " << FbxPath << Qt::endl;

    //make dir export fbx
    QDir nDir(nPathDir);

    if(!makedir) return FbxPath;

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

void MainWindow::ClearScripts()
{
    QStringList scripts = QStringList() << "*.mel" << "*.py";

    QDir SDir(LOCALSCRIPTS);

    QStringList files = SDir.entryList(scripts);

    if(files.count() <=0) return;

    for(auto s : files) {
        SDir.remove(s);
    }

}

void MainWindow::ExecuteExportFbx(const int inId)
{
    //check Path Source
    if(!IsValidPath(EpSourceFiles[inId])){
        AddToLog("Source Path does'nt Exists.","red");
        return;
    }


    QString SFile = EpSourceFiles[inId];

    //make Path file
    QString ExFile = GetExportPath(SFile,ExportDir);

    SoftwereType SType = GetSoftWareType();

    //Execute Command
    command = new CommandLine();
    MassCmds.push_back(command);
    command->SetMayaPro(ui->MayaText->toPlainText());
    command->SetBlenderPro(ui->BlenderText->toPlainText());
    command->SetCommandId(inId);
    command->SetExportFolder(ExportDir);
    EpCount++;
    //debug mode
    if(!uiOpt->DebugBox->isChecked()) command->SetClearOnComplete(true);

    command->CreateProcess(SFile,ExFile,logStr,SType);

    connect(command,&CommandLine::SendErrorStr,this,&MainWindow::DisplayErr);
    connect(command,&CommandLine::SendCRFile,this,&MainWindow::Display);
    connect(command,&CommandLine::SendId,this,&MainWindow::OnCompletedId);
    connect(&command->ExportProcess,&QProcess::finished,this,&MainWindow::OnFinish);
    command->ExportProcess.waitForStarted();
    if(!logStr.endsWith("<br>"))
        logStr+="<br>";
    logStr += "Exporting from : " + SFile + " to : " + ExFile;

    qDebug() << "file " << SFile << "number : " << EpSourceFiles.count() <<  Qt::endl;

    ui->LeeLog->setHtml(logStr);
    ui->LeeLog->verticalScrollBar()->setValue(ui->LeeLog->verticalScrollBar()->maximum());


}

void MainWindow::OnMayaFinish(QStringList inFbxList)
{
    //LogFile OnFinish Command
    mMaya->VerifiedExported();
    if(mMaya->IsNotFound){
        AddToLog("Error : MassExport Layer Name not found","red");
        return;
    }

    if(!mMaya || inFbxList.count() <=0) return;

    for(auto line : inFbxList){
        qDebug() << "Maya Finish : " << line << Qt::endl;
    }
}

void MainWindow::OnComboBoxChanged(int valuechanged)
{
    qDebug() << "Value Changed " << valuechanged << Qt::endl;
    QString textChanged;
    switch (valuechanged) {
        case 0:{textChanged="Settings Maya";break;}
        case 1:{textChanged="Settings Blender";break;}
        case 2:{textChanged="Settings Maya And Blender";break;}
    }

    Spoiler->toggleButton->setText(textChanged);
}

