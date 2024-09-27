#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_FbxOptions.h"
#include "./ui_Maintain.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , uiOpt(new Ui::FbxOptions)
    , uiTree(new Ui::Maintain)
{

    ui->setupUi(this);


    undo_stack = new QUndoStack(this);
    // QColor color =Qt::white;
    // QPalette palette = ui->menuFile->palette();
    // palette.setColor(QPalette::WindowText, color);
    // ui->menuView->setPalette(palette);

    //"QMenuBar::item:selected { background: black; } "
    //ui->menuFile->setStyleSheet("QMenu::item{color: white;}");

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

    connect(uiOpt->makedirBox,&QCheckBox::stateChanged,this,&MainWindow::OnMakeDirChanged);
    connect(uiOpt->EBaseSkeleton,&QCheckBox::stateChanged,this,&MainWindow::OnMakeDirChanged);
    connect(uiOpt->LeeGeometry,&QCheckBox::stateChanged,this,&MainWindow::OnMakeDirChanged);

    connect(ui->ShowExploder,&QAction::triggered,this,&MainWindow::OnRevealFolder);
    connect(ui->ShowExport,&QAction::triggered,this,&MainWindow::OnRevealFolder);
    connect(ui->ShowSource,&QAction::triggered,this,&MainWindow::OnRevealFolder);
    connect(ui->ShowScripts,&QAction::triggered,this,&MainWindow::OnRevealFolder);
    connect(ui->ShowLogs,&QAction::triggered,this,&MainWindow::OnRevealFolder);

    //test button
    connect(ui->CustomFilesBtn,&QPushButton::clicked,this,&MainWindow::OnExportClicked);

    connect(ui->Quiter,&QAction::triggered,[this](){QCoreApplication::exit();});
    connect(ui->EditClearConsole, &QAction::triggered,[this](){ui->LeeLog->clear(); });
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


    //Inti Maintain Folder
    ImplementTreeView();


    //Loop Update Functions;

   // QTimer *timer = new QTimer(this);

    TimeUpdate = new QTimer(this);
    connect(TimeUpdate, &QTimer::timeout, this, &MainWindow::LeeUpdateFuntions);
    TimeUpdate->start(2000.f);


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


    //Valid PC
    IsAuthored = StudioIsValid();

    logStr = !IsAuthored ? "<font color=\"red\">Cannot use unauthorized tools</font><br>" :
                         "<font color=\"green\">Welcome To Plus Stuido MassExport Fbx Software</font><br>";

    logStr += MASSINFO.arg(_Pc,_Users,_Host =="" || _Host.isEmpty() || _Host.isNull() ? "empty" : _Host);
    ui->ExpCurrentFile->setText(QString("Exporting File : None"));
    if(!IsAuthored) {
        this->setDisabled(true);
        return;
    }

    // bool ValidNetwork = IsOnline() ?
    //                         IsValidAPI(LEEARTURL,GetMacAdress()) :
    //                         false;

    AddToLog(logStr,"white",true);
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

    //QString localPath = "C:/Users/" + qgetenv("USERNAME") + "/AppData/Local/LeeMassFbx/";

}

#pragma endregion //

MainWindow::~MainWindow()
{
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
    ScrollToNewLog();
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
                if(uiTree->treeView !=nullptr)
                {
                    if(LeeModel !=nullptr){
                        uiTree->treeView->setRootIndex(LeeModel->setRootPath(dir));
                        uiTree->label->setText(QString("Folder Path : %1").arg(dir));
                    }
                }

                // if(TreeSysV !=nullptr)
                //     TreeSysV->setRootPath(dir);
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


    //Valid Soft Ware path name
    if(!IsValidSoft() || !IsAuthored) return;

    // Check Opt Working
    // QString FbxOptions = uiOpt->MayaFbxOptions->currentText();
    // if(FbxOptions.endsWith("Skeleton")){
    //     QString message = "%1 : This Option not ready to run";
    //     AddToLog(message.arg(FbxOptions),"red");
    //     return;
    // }


    QPushButton* clickBtn = qobject_cast<QPushButton*>(sender());


    SoftwereType SType = GetSoftWareType();

    //create Filter List
    QStringList filters= InitFillters();

    //Debug Filters
    QString FilLog = "Export Type : ";
    for(auto ff : filters)
        FilLog+= ff;


    //Get Source Files
    //Reset On Click;
    // completedId = 0;
    // TotalFbx = 0;
    ResetMem();


    //===================================================
    if(clickBtn->objectName() == "CustomFilesBtn")
    {
        bool Confirm = ShowMessageConfirmBox();
        if(!Confirm) return;
        uiOpt->LeeSoft->setCurrentText("Maya & Blender");
        QStringList checkeditems = LeeModel->GetCheckedItem();
        EpSourceFiles = GetFileNameFromDir(ui->SourceFolderText->toPlainText(),checkeditems);

    }
    else{
        GetFilesInDir(ipSourceDir,EpSourceFiles,filters);
    }
    QString ShowType = EpSourceFiles.count() > 0 ?
        "Export Type : " + uiOpt->LeeSoft->currentText() :
                           QString("MassLogs : No Selection Custom files for export...");

    qDebug() << FilLog << Qt::endl;

    AddToLog(Warning,ShowType);


    TotalFiles = EpSourceFiles.count();
    ui->progressBar->setValue(0);
    isRunning = true;
    ui->ExportExecute->setEnabled(false);

    //Log Files Searching
    qDebug() << "Files Count " << EpSourceFiles.count() << Qt::endl;
    qDebug() << "ip Source " << ipSourceDir << Qt::endl;



    //check Multi Threading
    isMultiThread = uiOpt->MultiThreadBox->isChecked();

    if(isMultiThread){
        if(EpSourceFiles.count() > 100) {
            QString Notice = "Critical : Number Files too many > 100 files Your Compuer Will Be Die ";
            for(int i = 0; i < 3 ; ++i)
                AddToLog(Error,Notice);
            return;
        }

        #pragma omp parallel
        {
            #pragma omp parallel for
            for(int i = 0 ; i < EpSourceFiles.count() ; ++i)
                ImplementExport(i);

        }

    }
    else{
        ImplementExport(EpCount);
    }

}

QStringList MainWindow::InitFillters()
{
    if(uiOpt->LeeSoft == nullptr)
        SourceType=1;

    SourceType = uiOpt->LeeSoft->currentIndex();

    if(SourceType < 0) { return MayaFiles ;}

    switch (SourceType) {
        case 1:{
            return MayaFiles;
        }
        case 2:{
            return BlenderFiles; }
        case 0:{
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

void MainWindow::AddToExpLogs(const QString inFileTarget)
{
    QString fInfo = ui->ExportFolderText->toPlainText() + LogsFileName;
    QFile file(fInfo);

    QByteArray dataCurrent;
    //read
    if(file.open(QIODevice::ReadOnly)){
        dataCurrent = file.readAll();
        file.close();
    }

     qDebug() << "JSon 1: " << inFileTarget << Qt::endl;
    QJsonParseError err;
    QJsonDocument jdoc = !dataCurrent.isEmpty() ? QJsonDocument::fromJson(dataCurrent, &err) : QJsonDocument();
    QJsonObject currentObj = !jdoc.isEmpty() ? jdoc.object() : QJsonObject();
    QJsonArray Infos = !currentObj.isEmpty() ? currentObj.value("ExportHistory").toArray() : QJsonArray();

    //qDebug() << "JSon 2 " << QString(jdoc.toJson(QJsonDocument::Compact)) << Qt::endl;
    if(err.error != QJsonParseError::NoError){
        //qDebug() << "JSon 2: " << err.errorString() << Qt::endl;
        return; // Failure
    }


    QJsonObject obj = GetFileInfo(inFileTarget);

    int idx = ExistObject(Infos,obj,"File");

    if(idx >= 0){
        Infos.removeAt(idx);
    }
    Infos.append(obj);
    QJsonObject final;
    final["ExportHistory"] = Infos;
    jdoc.setObject(final);
    //qDebug() << "JSon 3 " << QString(jdoc.toJson(QJsonDocument::Compact)) << Qt::endl;

    if(file.open(QIODevice::ReadWrite | QIODevice::Truncate)){
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
        case LMakeDir:{
            Session="Makedir";
            break;
        }
        case LMesh: {
            Session="Mesh";
            break;
        }
        case LSkeleton:{
            Session ="Skeletal";
            break;
        }
        case LSoft : {
            Session = "SoftWare";
            break;
        }
        case LKit : {
            Session = "FbxKit";
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
    //Loadd All Text Edit Data
    SetTextLine(ui->MayaText,GetDataFromKey("BatchMaya").toString());
    SetTextLine(ui->BlenderText,GetDataFromKey("Blender").toString());
    SetTextLine(ui->SourceFolderText,GetDataFromKey("SourcePath").toString());
    SetTextLine(ui->ExportFolderText,GetDataFromKey("ExportPath").toString());

    //Load All Check Box Data
    SetCheckBox(uiOpt->makedirBox,GetDataFromKey("Makedir").toString()=="2" ? true :false);
    SetCheckBox(uiOpt->LeeGeometry,GetDataFromKey("Mesh").toString()=="2" ? true :false);
    SetCheckBox(uiOpt->EBaseSkeleton,GetDataFromKey("Skeletal").toString()=="2" ? true :false);

    //Load ALl Combo Box Data
    SetComboText(uiOpt->LeeSoft,GetDataFromKey("SoftWare").toString());
    SetComboText(uiOpt->FbxBlenderKit,GetDataFromKey("FbxKit").toString());

}

QJsonValue MainWindow::GetDataFromKey(const QString key)
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
    return jobj.value(key);
}

void MainWindow::SetCheckBox(QCheckBox *box, bool isChecked)
{
    if(box ==nullptr) return;

    return box->setChecked(isChecked);
}

void MainWindow::SetTextLine(QTextEdit *text, const QString intext)
{
    if(text ==nullptr) return;
    return text->setText(intext);
}

void MainWindow::SetComboText(QComboBox *box, const QString intext)
{
    if(box ==nullptr) return;
    return box->setCurrentText(intext);

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

    if(!IsOnline()){
        AddToLog(Error,"Net Message : Network Connection failse please check your internet connection..");

        return false;
    }

    QString HostLower = _Host.toLower();

    QStringList Ids = {GetMacAdress(),HostLower,_Pc};


    if(Ids.length() <=0) return false;

    for(auto Id : Ids)  {
        if(IsValidAPI(LEEARTURL,Id)) return true;
    }

    // if(GetMacAdress() == "10:7C:61:47:26:B1"||
    //     GetMacAdress() == "04:7C:16:E3:94:DB" ||
    //     GetMacAdress() == "AC:15:A2:02:DD:DF" ||
    //     _Pc == "DESKTOP-PPQMP6F"
    //     ) {return true;} //|| GetMacAdress() == "04:7C:16:E3:94:DB"
    // //Studio PC Domain Name
    // if(_Host.toLower() == "giaoduc.edu") return true;

    QString LogAuthor =  "Error : You cannot use the software without the consent of Lee.";

    AddToLog(Error,LogAuthor);

    OnStudioValidFailure();

    return false;
}

bool MainWindow::IsValidSoft()
{
    QString SoftOptions = uiOpt->LeeSoft->currentText();
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

void MainWindow::AddToLog(QString inMessage,QString inColor,bool isClear)
{
    if(inMessage =="") return;
    QString CLog = !isClear ? ui->LeeLog->toHtml() : "";
    CLog += QString("<font color=\"%1\">%2</font>").arg(inColor,inMessage);
    ui->LeeLog->setHtml(CLog);
    ScrollToNewLog();
}

void MainWindow::AddToLog(const LogType inLog, QString inMessage, bool isClear)
{
    if(inMessage =="") return;
    QStringList splitMes =  inMessage.split(" : ");
    QString title = splitMes.count() >=0 ? splitMes[0] : inMessage;

    QString mess = splitMes.count() >=2  ? splitMes[1] : splitMes[0];
    QString result;
    switch (inLog) {
        case Log:{
            result = QString("<font color=\"white\">%1 : </font>").arg(mess);
            break;
        }
        case Warning:{
            result = QString("<font color=\"yellow\">%1 : </font>").arg(title);
            result += mess;
            break;
        }
        case Error:{
            result = QString("<font color=\"red\">%1 : </font>").arg(title);
            result += mess;
            break;
        }
        case Completed:{
            result = QString("<font color=\"green\">%1 : </font>").arg(title);
            result += mess;
            break;
        }
    };

    QString CLog = !isClear ? ui->LeeLog->toHtml() : "";
    CLog += result;
    ui->LeeLog->setHtml(CLog);

    //new Logs
    ScrollToNewLog();
}

void MainWindow::ScrollToNewLog(){

    //set value max down to new Log
    int valueMax = ui->LeeLog->verticalScrollBar()->maximum();
    ui->LeeLog->verticalScrollBar()->setValue(valueMax);
}

void MainWindow::ResetMem()
{
    //Clear Completed Id
    completedId = 0;
    //Clear FBx Total
    TotalFbx = 0;
    //Clear Export Count
    EpCount=0;
    //Clear List console
    ListCmds = QList<ImpCmd*>();
    //Clear Source Files
    EpSourceFiles.clear();
}

void MainWindow::OnDirectoryFilterLoader()
{

}

void MainWindow::OnTestModel()
{
    //Get List Checked Item
    QStringList checkeditems = LeeModel->GetCheckedItem();
    // for(auto item : checkeditems)
    //     qDebug() << "Model 2: " << item << Qt::endl;

    QStringList files = GetFileNameFromDir(ui->SourceFolderText->toPlainText(),checkeditems);

    for(auto f: files){
        qDebug() << "Model result: " << f << Qt::endl;
    }
}

void MainWindow::ImplementFbxOptions()
{
    ///Implement Export OPtions
    ///==============================
    Spoiler = new LeeSpoiler("Settings Maya And Blender",100,this);
    uiOpt->setupUi(Spoiler);
    connect(uiOpt->LeeSoft,&QComboBox::currentTextChanged,this,&MainWindow::OnSoftWereChanged);
    connect(uiOpt->FbxBlenderKit,&QComboBox::currentTextChanged,this,&MainWindow::OnSoftWereChanged);
    //font: 700 9pt "Times New Roman";
    Spoiler->toggleButton->setStyleSheet("font: 700 11pt \"Times New Roman\"; color: rgb(255, 255, 255);");
    Spoiler->toggleButton->setAutoRaise(true);
    QVBoxLayout * vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addLayout(uiOpt->verticalLayout);
    vlayout->addLayout(hlayout);
    Spoiler->setContentLayout(*vlayout);
   // ui->AuthorLayout->insertWidget(ui->AuthorLayout->count(),Spoiler);// ->addWidget(Spoiler);
    //ui->MainVLayout->layout()->addWidget(Spoiler);
    ui->horizontalLayout_3->addWidget(Spoiler,Qt::AlignHCenter);
    //ui->verticalLayout_3->addWidget(Spoiler,Qt::AlignCenter);
    this->setMinimumHeight(this->height() + Spoiler->height());

}

void MainWindow::ImplementTreeView()
{
    MaintainSpoiler = new LeeSpoiler("Custom Files",150,this);
    MaintainSpoiler->SetDirection(Qt::UpArrow,true);
    MaintainSpoiler->SetDirection(Qt::DownArrow);
    uiTree->setupUi(MaintainSpoiler);


    MaintainSpoiler->toggleButton->setStyleSheet("font: 700 11pt \"Times New Roman\"; color: rgb(255, 255, 255);");
    // MaintainSpoiler->toggleButton->setMinimumSize(QSize(30,100));
    MaintainSpoiler->toggleButton->setAutoRaise(true);
    QHBoxLayout * vlayout = new QHBoxLayout();
    QVBoxLayout* hlayout = new QVBoxLayout();
    hlayout->addLayout(vlayout);
    vlayout->addLayout(uiTree->gridLayout);
    MaintainSpoiler->setContentLayout(*hlayout);


    ui->MaintainV->addWidget(MaintainSpoiler);

    /// Init TreeView
    InitFileSysModel();

}

void MainWindow::InitFileSysModel()
{

    ///
    /// Init TreeView
    ///
    QString directory = ui->SourceFolderText->toPlainText();
    QDir dir(directory);

    if(!dir.exists()){
        uiTree->label->setText("Source folder is empty");
        return;
    }

    file3DFilter <<"*.ma" << "*.mb" << "*.blend";

    QString TreeLabel = QString("Folder Path : %1").arg(directory);

    LeeModel = new LeeTreeModel(uiTree->treeView);

    //QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);

    //uiTree->treeView->setModel(proxyModel);
    //proxyModel->setSourceModel(LeeModel);
    //proxyModel->
    //TreeSysV = new QFileSystemModel(uiTree->treeView);

    uiTree->treeView->setModel(LeeModel);
    uiTree->treeView->setRootIndex(LeeModel->setRootPath(directory));
    LeeModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    LeeModel->setNameFilters(file3DFilter);
    LeeModel->setNameFilterDisables(false);
    //proxyModel->setFilterFixedString("\.ma");
    // proxyModel->setFilterRegularExpression(QRegularExpression("*.ma", QRegularExpression::CaseInsensitiveOption));
    // proxyModel->setFilterKeyColumn(1);

    uiTree->label->setText(TreeLabel);
    uiTree->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);


    //test button
    connect(LeeModel,&QFileSystemModel::directoryLoaded,this,&MainWindow::OnDirectoryFilterLoader);

    ///set Avarage With column
    float windowWidth = this->width();
    float avgWidth = float(windowWidth/LeeModel->columnCount());
    for(int i = 0 ; i < LeeModel->columnCount() ; ++i){
        float value = i == 0 ? windowWidth/2 : avgWidth/2;
        uiTree->treeView->setColumnWidth(i,value);
    }
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

    if(!nDir.exists()){
        nDir.mkpath(nPathDir);
    }

    if(uiOpt->EBaseSkeleton->isChecked()){
        QString BDir = nPathDir + "BaseSkeletal/";
        QDir BaesDir(BDir);
        if(!BaesDir.exists(BDir))
            BaesDir.mkpath(BDir);
        qDebug() <<"Debug : " << BDir << Qt::endl;
    }

    //return fbx file exports
    return FbxPath;
}

SoftwereType MainWindow::GetSoftWareType()
{
    int SType = uiOpt->LeeSoft->currentIndex();

    switch (SType) {
        case 0: return Maya;
        case 1: return Blender;
        case 2: return Max3D;
        case 3: return None;
    }
    return None;
}

void MainWindow::OnCmdFinish(int exitCode, QProcess::ExitStatus exitStatus,QStringList inFbxList) {

    //Command Id
    int idx = completedId;
    completedId+=1;

    //check Id
    if(!completedId-1 >= ListCmds.count() || completedId-1 < 0) return;


    //Get Assign Command ID
    ImpCmd* iCmd = ListCmds[completedId-1];

    //Check Blender Addons
    if(iCmd->MissingAddons) {
        if(iCmd->GetSourceFile().endsWith(".blend"))
        {
            QString Message = "AutoRigPro : Blender Addons  is Not Found";
            AddToLog(Error,Message);
        }
    }

    //verifier
    iCmd->VerifiedExported();

    ///Progress bar
    int value = (100.0/TotalFiles) * completedId;
    ui->progressBar->setValue(value);
    if(value >=99){
        ui->progressBar->setValue(100);
    }

    /// File dont have MassExport Layer
    if (iCmd->GetLayerInfo()) {
        QString Message = "MassExport Layer Name not found : %1";
        Message = Message.arg(EpSourceFiles[completedId-1]);
        AddToLog(Error,Message);

        //if(isMultiThread) return;
        //return ExpNext();
    }

    QStringList FbxResult= ListCmds[completedId-1]->ListFbxs;
    for(auto f : FbxResult){
        QString CompleteText = QString("Export Completed : %1").arg(f);
        AddToLog(Completed,CompleteText);
    }


    ui->LeeLog->clear();

    switch (exitStatus) {
        case QProcess::ExitStatus::CrashExit:{
            QString message = QString("Crash Exit Code : %1").arg(exitCode);
            AddToLog(Error,message);
            break;
        }
        case QProcess::ExitStatus::NormalExit:{
            QString message = QString("Export Done Exit Code : %1").arg(exitCode);
            AddToLog(Error,message);
            break;
        }
    }


    if(!ui->LeeDebugContent->isChecked()){
        ImpCmd* iCmd = ListCmds[completedId-1];
        iCmd->ClearOnFinish();
        //ListCmds.removeOne(iCmd);
    }


    if(completedId == TotalFiles)
    {
        //TotalFbx = FbxCompletedCount();
        //enable Export Btn
        ui->ExportExecute->setEnabled(true);
        AddToLog(Warning,QString("Total Export Files : %1").arg(TotalFiles));
        AddToLog(Warning,QString("Total Fbx : %1").arg(TotalFbx));
        //FbxCompletedCount();
        ui->progressBar->setFormat("Done Job %p%");
    }

    AddToExpLogs(iCmd->GetSourceFile());

    if(isMultiThread) return;
    return ExpNext();
}

void MainWindow::OnCmdStarted()
{
    qDebug() << "Started.. "  << Qt::endl;
    // if(!mImpCmd) return;
    // QString currentfile = QString("Exrporting : %1..").arg(mImpCmd->GetSourceFile());
    // AddToLog(currentfile);

}

void MainWindow::ImplementExport(int fileNumber)
{
    {
        if(fileNumber >= EpSourceFiles.count()||
            fileNumber < 0){
            qDebug() << "index Out Range " << Qt::endl;
            return;
        }

        bool isMakeDir = uiOpt->makedirBox->isChecked();
        ui->progressBar->setFormat("Running %p%");

        bool IsMakeBase = uiOpt->EBaseSkeleton->isChecked();

        QString finalExpDir = MakeTreeDirectory(EpSourceFiles[fileNumber],
                            ui->SourceFolderText->toPlainText(),
                            ui->ExportFolderText->toPlainText(),IsMakeBase);

        ui->ExpCurrentFile->setText(QString("Exporting File : %1").arg(EpSourceFiles[fileNumber]));
        //Maya Exp Refactored..
        ImpCmd* mCmd = new ImpCmd(EpSourceFiles[fileNumber],finalExpDir);

        //Base Skeletal
        mCmd->SetExportSkeletal(uiOpt->EBaseSkeleton->isChecked());

        mCmd->SetExpId(fileNumber);
        //Set Source Dir
        mCmd->SetSourceDir(ui->SourceFolderText->toPlainText());
        //Set Program
        mCmd->SetProgram(ui->MayaText->toPlainText(),ui->BlenderText->toPlainText());
        //Set Id
        mCmd->SetExpId(EpCount+1);
        //Set Blender Kit
        if (EpSourceFiles[fileNumber].endsWith(".blend")){
            //Properties Kit Fbx for Blender
            QString Kit = uiOpt->FbxBlenderKit->currentText().startsWith("Auto") ? "AutoRigPro" : "Blender";
            mCmd->setProperty("FbxKit",Kit);
            mCmd->SetScriptPlatForm();

        }

        QString FbxOpt{};

        if(uiOpt->EBaseSkeleton->isChecked())
            FbxOpt="BaseSkeleton";
        else
            FbxOpt="AnimOnly";

        if(uiOpt->LeeGeometry->isChecked())
            mCmd->setProperty("Mesh","On");
        //set Property for Maya
        mCmd->setProperty("FbxOpt",FbxOpt);

        mCmd->SetScriptPlatForm();


        ///Connecttion
        connect(mCmd,&ImpCmd::OnStart,this,&MainWindow::OnCmdStarted);

        qRegisterMetaType<QString>("QString&");

        connect(mCmd,SIGNAL(OnReadLogs(QString&,QString&)),this,SLOT(OnLogs(QString&,QString&)));

        connect(mCmd,&ImpCmd::OnFinish,this,&MainWindow::OnCmdFinish);
        connect(mCmd,&ImpCmd::OnError,this,&MainWindow::OnTakeError);
        connect(mCmd,&ImpCmd::OnPStateChanged,this,&MainWindow::OnStateChanged);


        mCmd->GetProcess()->waitForStarted();
        if(!mCmd->Message.isEmpty())
            AddToLog(mCmd->Message);

        EpCount++;

       // mImpCmd = mCmd;

        //list Threding Add to List
        if(!mCmd->ConsoleExists(ListCmds))
            ListCmds.push_back(mCmd);
    }
}

void MainWindow::ExpNext()
{
    if(isMultiThread) return;

    if(EpCount < EpSourceFiles.count()){
        //Single Threading
        return ImplementExport(EpCount);
    }
    else{
        ui->ExportExecute->setEnabled(true);
        return;
    }
}

int MainWindow::FbxCompletedCount()
{
    if(ListCmds.count() <=0) return 0;
    int total = 0;

    for(auto cmd : ListCmds){
        for(auto fbx : cmd->GetExpResults()){
            total++;
        }
    }
    //qDebug() << "FBX : " << total << Qt::endl;
    return total;
}

void MainWindow::OnRevealFolder()
{
    //Show in Exploders Directories
    QAction* act = qobject_cast<QAction*>(sender());
    if(!act) return;

    QString path{};

    if(act->objectName()==ui->ShowExploder->objectName())
        path = MASSFBXDIR;
    else if(act->objectName()==ui->ShowExport->objectName())
        path = ui->ExportFolderText->toPlainText();
    else if(act->objectName()==ui->ShowSource->objectName())
        path=ui->SourceFolderText->toPlainText();
    else if(act->objectName()==ui->ShowScripts->objectName())
        path=MASSFBXDIR + "Scripts";
    else if(act->objectName()==ui->ShowLogs->objectName())
        path=MASSFBXDIR + "Logs";;

    QDesktopServices::openUrl( QUrl::fromLocalFile(path) );
}

bool MainWindow::IsValidAPI(const QString inUrl,const QString inMacHost)
{

    ///https://raw.githubusercontent.com/leetdvn/GiantyTest/refs/heads/main/LeeArtLisences.json
    if(inUrl.isEmpty()) return false;

    QEventLoop eventloop;
    QNetworkAccessManager networkManager;

    QUrl url(inUrl);

    QNetworkRequest request(url);

    QNetworkReply* reply = networkManager.get(request);

    QString result="NetworkDebug : ";
    connect(reply,&QNetworkReply::finished,&eventloop,&QEventLoop::quit);
    eventloop.exec();
    QString ReplyText= reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(ReplyText.toUtf8());
    QJsonObject obj = doc.object();
    QJsonValue value = obj.value(QString("LeeArt"));

    QJsonArray arr = value.toArray();
    for(auto elm : arr){
        QJsonObject obj = elm.toObject();
        QJsonValue val = obj.value("ID");

        if(val.toString()==inMacHost)
        {
            _AuthorName = val.toString();
            qDebug() << "Network Validated.. : " << val.toString() << Qt::endl;
            return true;
        }
    }
    return false;
}

bool MainWindow::IsOnline()
{
    if ( QNetworkInformation::loadDefaultBackend() && QNetworkInformation::loadBackendByFeatures(QNetworkInformation::Feature::Reachability ) ) {
        QNetworkInformation* net_info = QNetworkInformation::instance();
        if ( nullptr != net_info ) {
            if(net_info->reachability() == QNetworkInformation::Reachability::Online) {
                qDebug() << "Device is Online";
                return true;
            }
            else {
                qDebug() << "Device is offline";
            }
        }
    }
    return false;
}

void MainWindow::LeeUpdateFuntions()
{

    if(IsAuthored){
        TimeUpdate->stop();
        TimeUpdate->deleteLater();
        TimeUpdate= nullptr;
        return;
    }

    IsAuthored = StudioIsValid();
    if(IsAuthored){
        ui->LeeLog->clear();
        this->setEnabled(true);
        InfoEnv();

    }
    else{
        AddToLog(Error,"Net Message : Network Connection failse please check your internet connection..");
        qDebug() << "Count Update : " << NumberCount << Qt::endl;
        NumberCount++;

    }
    // while(true){
    //     QTimer::singleShot(Instance->NumberCount, Instance,LeeUpdateFuntions);
    //     Instance->NumberCount++;
    // }

    // while(true){
    //     LDelay(1);
    //     qDebug() << "Count Update : " << NumberCount << Qt::endl;
    //     NumberCount++;
    // }

}


void MainWindow::OnLogs(QString &inLog,QString &Err)
{
    ImpCmd* iCmd = qobject_cast<ImpCmd*>(sender());

    //On Debug Console
    QString iLog = !iCmd->GetSourceFile().endsWith(".blend") ? inLog.replace("\r\n","") : inLog.replace("\r","");
    //Debug Development
    qDebug() << iLog << Qt::endl;

    if(iLog.startsWith("Exported")){
        AddToLog(LogType::Completed,iLog);
        TotalFbx++;
        ScrollToNewLog();
    }

    //Show Log Console
    if(!ui->LeeDebugConsole->isChecked()) return;

    AddToLog(iLog);
    AddToLog(Error,Err);
    ScrollToNewLog();

}

void MainWindow::OnTakeError(QProcess::ProcessError &Err)
{
    switch (Err)
    {
        case QProcess::ProcessError::Crashed:{
            AddToLog(LogType::Error,"Crash Error");
            break;
        }
        case QProcess::ProcessError::Timedout:{
            qDebug() << "Time Out Error" << Qt::endl;
            AddToLog(LogType::Error,"Time Out Error");

            break;
            }
        case QProcess::ProcessError::UnknownError:{
            qDebug() << "Unknow Error" << Qt::endl;
            AddToLog(LogType::Error,"Unknow Error");
            break;
        }
        case QProcess::FailedToStart:{
            qDebug() << "Failure Error" << Qt::endl;
            AddToLog(LogType::Error,"Failure Error");
            break;
        }
        case QProcess::ReadError: {
            qDebug() << "Reading Error" << Qt::endl;
            AddToLog(LogType::Error,"Reading Error");
            break;
        }
        case QProcess::WriteError:{
            qDebug() << "Write Error" << Qt::endl;
            AddToLog(LogType::Error,"Write Error");
            break;
        }
    }
}

void MainWindow::OnStateChanged(QProcess::ProcessState newState)
{
    QString message="State Changed : %1";

    switch (newState) {
        case QProcess::ProcessState::NotRunning:{
            AddToLog(Warning,message.arg("Not Runing"));
            break;
        }
        case QProcess::ProcessState::Running:{
            AddToLog(Warning,message.arg("Runing"));
            break;
        }
        case QProcess::ProcessState::Starting:{
            AddToLog(Warning,message.arg("Starting"));
            break;
        }
    }
}

void MainWindow::OnSoftWereChanged(const QString & textchanged)
{
    QComboBox* box = qobject_cast<QComboBox*>(sender());

    if(box == nullptr) return;

    if(box->objectName().endsWith("LeeSoft")){
        SaveToLocal(LSoft,QString("%1").arg(textchanged));
        Spoiler->toggleButton->setText(QString("Settings %1").arg(textchanged));
    }
    else if(box->objectName()=="FbxBlenderKit")
        SaveToLocal(LKit,QString("%1").arg(textchanged));
}

void MainWindow::OnMakeDirChanged(int value)
{
    QCheckBox* chBox = qobject_cast<QCheckBox*>(sender());
    qDebug() << "Make Dir " << value <<  Qt::endl;

    if(chBox->objectName()=="makedirBox")
        SaveToLocal(LMakeDir,QString::number(value));
    else if(chBox->objectName() =="LeeGeometry")
        SaveToLocal(LMesh,QString::number(value));
    else if(chBox->objectName()=="EBaseSkeleton")
        SaveToLocal(LSkeleton,QString::number(value));
}

void MainWindow::OnStudioValidFailure()
{
    this->setDisabled(true);
}
