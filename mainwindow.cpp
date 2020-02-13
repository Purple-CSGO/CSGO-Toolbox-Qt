#include "mainwindow.h"
#include "ui_mainwindow.h"

//引入库文件，位置必须是绝对地址，有改变务必改动
#pragma comment(lib, "E:/QtProject/CSGO Toolbox/CSGO-Toolbox/libShareCodeToURLcs.lib" )
__declspec(dllimport) int api_Urlstring(const char* a);

//全局变量
QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";
QString steamID = "";
QString userName = "";
QString launchOption1 = "";
QString launchOption2 = "";
bool autoClip = false;
bool autoDownload = false;
bool backupdataZipped = false;
bool useTray = true;
bool launchFirst = true;
short cpuType = 0;

//TODO: 加载过程的文字显示在Loading界面上
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUI();
    createTray();
    readSetting();
    getSteamPath();
    getCsgoPath();
    refreshBackup();
    if( SteamIDList.length() == 0 )
        getSteamID();
    else
        setupUserdata();
}

MainWindow::~MainWindow()
{
    if( useTray == true )
        writeSetting();
    delete ui;
}

/**
  * 程序的核心模块，是完成其他实际功能的基础
  * 1. 程序启动/关闭时调用
  * 2. 获取各种路径
  * 3. 获取SteamID
  */

/*  1. 程序启动/关闭时调用   */
//设置UI样式
void MainWindow::setupUI()
{
    QFont font, font1, font2, font6, font60;
    font.setFamily("微软雅黑");
    qApp->setFont(font);
    font1.setPixelSize(14);
    font2.setPixelSize(17);
    font6.setPixelSize(20);
    font60.setPixelSize(60);
    ui->centralwidget->setFont(font1);
    ui->tabWidget->setFont(font2);
    ui->TabMain->setFont(font2);
    ui->backupSetting->setFont(font2);
    ui->backupdata->setFont(font1);
    ui->checkCsgoPath->setFont(font1);
    ui->checkLauncherPath->setFont(font1);
    ui->checkSteamID->setFont(font1);
    ui->checkSteamPath->setFont(font1);
    ui->getLauncherPathBtn->setFont(font2);
    ui->getUserDataBtn->setFont(font2);
    ui->manualBtn->setFont(font2);
    ui->openBackupLoc->setFont(font2);
    ui->openCNlocalcfg->setFont(font2);
    ui->opencsgoDir->setFont(font2);
    ui->opencsgocfg->setFont(font2);
    ui->openlocalcfg->setFont(font2);
    ui->userdata->setFont(font2);
    ui->zipBackupdata->setFont(font2);
    ui->TabSetting->setFont(font2);
    ui->ManualSteamID->setFont(font2);
    ui->manualCsgoBtn->setFont(font2);
    ui->antiHarmony->setFont(font2);
    ui->debug->setFont(font2);
    ui->reloadHarmony->setFont(font2);
    ui->solveVAC->setFont(font2);
    ui->textID->setFont(font6);
    ui->TabDemo->setFont(font2);
    ui->autoClip->setFont(font2);
    ui->autoDownload->setFont(font2);
    ui->clipURL->setFont(font2);
    ui->dispURL->setFont(font2);
    ui->downURL->setFont(font2);
    //ui->dragArea->setFont(font40);
    ui->labelDispURL->setFont(font2);
    ui->labelDragArea->setFont(font60);
    ui->labelSharecode->setFont(font2);
    ui->textEdit->setFont(font6);
    ui->PCconfig->setFont(font2);
    ui->labelDragArea->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkSteamPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkCsgoPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    ui->checkLauncherPath->setAttribute(Qt::WA_TransparentForMouseEvents,true);
}

//读取设置
void MainWindow::readSetting()
{
    bool t;
    QSettings *iniRead = new QSettings("./config.ini", QSettings::IniFormat);
    iniRead->setIniCodec("utf-8");     //解决乱码问题

    iniRead->beginGroup("Paths");
    steamPath = iniRead->value("steamPath").toString();
    launcherPath = iniRead->value("launcherPath").toString();
    csgoPath = iniRead->value("csgoPath").toString();
    iniRead->endGroup();

    iniRead->beginGroup("User");
    userName = iniRead->value("userName").toString();
    steamID = iniRead->value("steamID").toString();
    SteamIDList = iniRead->value("SteamIDList").toStringList();
    PersonaNameList = iniRead->value("PersonaNameList").toStringList();
    iniRead->endGroup();

    iniRead->beginGroup("Auto");
    autoClip = iniRead->value("autoClip").toBool();
    autoDownload = iniRead->value("autoDownload").toBool();
    iniRead->endGroup();

    iniRead->beginGroup("Powerplan");
    cpuType = iniRead->value("cpuType").toUInt();
    iniRead->endGroup();

    iniRead->beginGroup("LaunchOption");
    launchOption1 = iniRead->value("launchOption1").toString();
    launchOption2 = iniRead->value("launchOption2").toString();
    t = iniRead->value("launchFirst").toBool();
    if( iniRead->value("launchFirst").toString().isEmpty() )
        launchFirst = true;
    else
        launchFirst = t;
    iniRead->endGroup();

    iniRead->beginGroup("Tray");
    useTray = iniRead->value("useTray").toBool();
    iniRead->endGroup();

    //iniRead->beginGroup("  ");
    //   = iniRead->value("  ").toString();
    //iniRead->endGroup();

    //读入完成后删除指针
    delete iniRead;

    //检查如果路径不存在则置空
    if( !isSteamExisted() )
        steamPath = "";
    if( !isLauncherExisted() )
        launcherPath = "";
    if( !isCsgoExisted() )
        csgoPath = "";

    //改变路径、iD相关的checkbox状态
    onSteamPathChanged();
    onCsgoPathChanged();
    onLauncherPathChanged();
    onSteamIDChanged();

    //设置ui中的checkbox状态
    if(autoClip == true)
        ui->autoClip->setCheckState(Qt::Checked);
    else
        ui->autoClip->setCheckState(Qt::Unchecked);
    if(autoDownload == true)
        ui->autoDownload->setCheckState(Qt::Checked);
    else
        ui->autoDownload->setCheckState(Qt::Unchecked);

    //设置电源计划radiobutton
    switch(cpuType){
    case 1:ui->Ryzen3000->setChecked(true);on_Ryzen3000_clicked(); break;
    case 2:ui->RyzenEtc->setChecked(true);on_RyzenEtc_clicked();break;
    case 3:ui->allPlatform->setChecked(true);on_allPlatform_clicked();break;
    default:break;
    }

    //设置启动项
    ui->LaunchOpt1->setText( launchOption1 );
    ui->LaunchOpt2->setText( launchOption2 );
    ui->sliderLaunchFirst->setValue( launchFirst );

    //设置托盘
    if( useTray == true )
        ui->checkboxTray->setCheckState(Qt::Checked);
    else
        ui->checkboxTray->setCheckState(Qt::Unchecked);
}

//写入设置
void MainWindow::writeSetting()
{
    //判断当前路径变量是否正确，否则置空
    if( !isSteamExisted() )
        steamPath = "";
    if( !isLauncherExisted() )
        launcherPath = "";
    if( !isCsgoExisted() )
        csgoPath = "";

    //判断powerplan相关变量是否正确
    if( cpuType > 3 || cpuType < 0 )
        cpuType = 0;

    //读取启动项
    launchOption1 = ui->LaunchOpt1->text();
    launchOption2 = ui->LaunchOpt2->text();
    launchFirst = ui->sliderLaunchFirst->value();

    //读取托盘设置
    useTray = ui->checkboxTray->checkState();

    //写设置
    QSettings *IniWrite = new QSettings("./config.ini", QSettings::IniFormat);
    IniWrite->setIniCodec("utf-8");     //解决乱码问题

    IniWrite->beginGroup("AppInfo");
    IniWrite->setValue("Name", qAppName());
    IniWrite->setValue("Version", APP_VERSION);
    IniWrite->endGroup();

    IniWrite->beginGroup("Paths");
    IniWrite->setValue("steamPath", steamPath);
    IniWrite->setValue("launcherPath", launcherPath);
    IniWrite->setValue("csgoPath", csgoPath);
    IniWrite->endGroup();

    IniWrite->beginGroup("User");
    IniWrite->setValue("userName", userName);
    IniWrite->setValue("steamID", steamID);
    IniWrite->setValue("SteamIDList", SteamIDList);
    IniWrite->setValue("PersonaNameList", PersonaNameList);
    IniWrite->endGroup();

    IniWrite->beginGroup("Auto");
    IniWrite->setValue("autoClip", autoClip);
    IniWrite->setValue("autoDownload", autoDownload);
    IniWrite->endGroup();

    IniWrite->beginGroup("Powerplan");
    IniWrite->setValue("cpuType", cpuType);
    IniWrite->endGroup();

    IniWrite->beginGroup("LaunchOption");
    IniWrite->setValue("launchOption1", launchOption1);
    IniWrite->setValue("launchOption2", launchOption2);
    IniWrite->setValue("launchFirst", launchFirst);
    IniWrite->endGroup();

    IniWrite->beginGroup("Tray");
    IniWrite->setValue("useTray", useTray);
    IniWrite->endGroup();

    //IniWrite->beginGroup("  ");
    //IniWrite->setValue("  ", );
    //IniWrite->endGroup();

    //写入完成后删除指针
    delete IniWrite;
}

//关闭主界面时保存设置
void MainWindow::closeEvent(QCloseEvent *e)
{
    if(useTray == true){
        e->ignore();//忽略关闭事件
        hide();//隐藏窗口
    }
    else{
        writeSetting();
        e->accept();
    }
}

//右下角托盘相关
void MainWindow::createTray()
{
    menu = new QMenu( this );//托盘菜单
    menuLaunchOpt = new QMenu( this );
    QAction *changeOpt = new QAction( this );
    QAction *LaunchPwrd = new QAction( this );
    QAction *LaunchWwd = new QAction( this );
    QAction *killcsgoproc = new QAction( this );
    //QAction *reset = new QAction( this );//菜单实现功能：恢复窗口
    QAction *quit = new QAction( this );//菜单实现功能：退出程序
    /****托盘*** */
    tray = new QSystemTrayIcon( this );
    tray->setIcon( QIcon( QPixmap( ":/file/logo.png" ) ) );
    tray->show();
    tray->setToolTip("CSGO工具箱");

    QObject::connect( tray, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ), this, SLOT( TrayIconAction( QSystemTrayIcon::ActivationReason ) ) );
    /****初始化托盘菜单及功能****/

    if(launchFirst == true)
        menuLaunchOpt->setTitle("当前启动项 ①");
    else
        menuLaunchOpt->setTitle("当前启动项 ②");
    changeOpt->setText("切换");
    QObject::connect( changeOpt, SIGNAL( triggered() ), this, SLOT( changeLaunchOpt() ) );

    LaunchWwd->setText("启动国际服");
    LaunchPwrd->setText("启动国服");
    QObject::connect( LaunchWwd, SIGNAL( triggered() ), this, SLOT( launchWwd() ) );
    QObject::connect( LaunchPwrd, SIGNAL( triggered() ), this, SLOT( launchPwrd() ) );

    killcsgoproc->setText("关闭CSGO");
    QObject::connect( killcsgoproc, SIGNAL( triggered() ), this, SLOT( killcsgo() ) );

    //reset->setText( "打开主面板" );
    //QObject::connect( reset, SIGNAL( triggered() ), this, SLOT( dispForm() ) );

    quit->setText( "退出" );
    QObject::connect( quit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

    menu->setStyleSheet("QMenu::item {padding: 2px 22px 2px 12px;}QMenu::item:selected {"
                        "background-color: rgb(145, 201, 247);color: rgb(0, 0, 0);}");
    menuLaunchOpt->setStyleSheet("QMenu::item {padding: 2px 22px 2px 12px;}QMenu::item:selected {"
                                 "background-color: rgb(145, 201, 247);color: rgb(0, 0, 0);}");

    tray->setContextMenu( menu );
    menu->addMenu( menuLaunchOpt );
    menuLaunchOpt->addAction( changeOpt );
    menu->addSeparator();
    menu->addAction( LaunchWwd );
    //menu->addSeparator();
    menu->addAction( LaunchPwrd );
    menu->addSeparator();
    menu->addAction( killcsgoproc );
    menu->addSeparator();
    //menu->addAction( reset );
    menu->addAction( quit );
}

void MainWindow::dispForm()
{
    this->showNormal();
    this->activateWindow();
    this->setWindowState( (this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
}

void MainWindow::TrayIconAction(QSystemTrayIcon::ActivationReason reason)
{   /* 如果对图标进行单击，则显示正常大小的窗口 */
    if ( reason == QSystemTrayIcon::Trigger ){
        QPoint mspos = cursor().pos();//获取鼠标位置
        menu->show();
        menu->move(mspos.rx(), mspos.ry() - 140 );
    }
    else if ( reason == QSystemTrayIcon::DoubleClick )
        dispForm();
}

void MainWindow::on_checkboxTray_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        useTray = true;
    else
        useTray = false;
}

void MainWindow::killcsgo()
{
    cmd("taskkill /F /IM csgo.exe");
}

void MainWindow::launchWwd()
{
    if(launchFirst == true)
        on_LaunchWwd1_clicked();
    else
        on_LaunchWwd2_clicked();
}

void MainWindow::launchPwrd()
{
    if(launchFirst == true)
        on_LaunchPwrd1_clicked();
    else
        on_LaunchPwrd2_clicked();
}

/*  2. 获取各种路径  */
//获取Steam路径
bool MainWindow::getSteamPath()
{
#define STEAM_KEY "HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam" //注册表SteamExe项所在位置
    //第一步 读取设置中的steamPath 已经在main()中调用 readSetting()
    QString tPath = steamPath;

    //第二步 尝试默认路径
    if( !isSteamExisted(tPath) ){
        QStringList defSteamPath;
        defSteamPath
                <<  QStringLiteral("C:/Program Files (x86)/steam/Steam.exe")
                 <<  QStringLiteral("C:/Steam/Steam.exe")
                  <<  QStringLiteral("D:/Steam/Steam.exe")
                   <<  QStringLiteral("E:/Steam/Steam.exe")
                    <<  QStringLiteral("G:/Steam/Steam.exe")
                     <<  QStringLiteral("H:/Steam/Steam.exe")
                         ;

        for(int i = 0; i< defSteamPath.size(); i++)
        {
            if( isSteamExisted( defSteamPath.at(i) ) ){
                tPath = defSteamPath.at(i);
                break;
            }
        }
    }

    //第三步 读取注册表中路径  //HKEY_CURRENT_USER\SOFTWARE\Valve\Steam 查询SteamExe
    if( !isSteamExisted(tPath) ){
        QSettings *pReg = new QSettings(STEAM_KEY, QSettings::NativeFormat);
        tPath = pReg->value("SteamExe").toString(); //读取注册表值
        //pReg->setValue("key", "value");   //设置注册表值
        delete pReg;
    }

    //第四步 读取进程列表 要打开steam
    if( !isSteamExisted(tPath) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("steam.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("steam.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( isSteamExisted(tPath) ){
        steamPath = tPath;
        onSteamPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取Steam路径失败，请手动选择！");
        return false;
    }
}

//获取国服启动器csgoLauncher路径
bool MainWindow::getLauncherPath()
{
#define Launcher_KEY "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam\\NSIS" //注册表CSGO项所在位置 TODO:
    //第一步 读取设置中的launcherPath 已经在main()中调用 readSetting()
    QString tPath = launcherPath;

    //第二步 读取注册表中路径
    if( !isLauncherExisted(tPath) ){
        QSettings *pReg = new QSettings(Launcher_KEY, QSettings::NativeFormat);
        tPath = pReg->value("Path").toString(); //读取注册表值
        delete pReg;
        if( !tPath.isEmpty() )
            tPath = tPath.append("\\csgolauncher.exe");
    }

    //第三步 读取进程列表 要打开国服启动器
    if( !isLauncherExisted(tPath) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgolauncher.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgolauncher.exe");
        }
    }

    //第四步 自动获取路径失败 提示用户手动选择路径
    if( isLauncherExisted(tPath) ){
        launcherPath = tPath;
        onLauncherPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取国服客户端路径失败，请手动选择！");
        tPath = QFileDialog::getOpenFileName(this, "选择国服启动器csgolauncher.exe的位置", tPath, "csgolauncher.exe");
        if( !isLauncherExisted(tPath) ){
            return false;
        }
        launcherPath = tPath;
        return false;
    }
}

//获取国服启动器路径的槽函数
void MainWindow::on_getLauncherPathBtn_clicked()
{
    getLauncherPath();
}

//获取CSGO路径
bool MainWindow::getCsgoPath()
{
    //第一步 读取设置中的csgoPath 已经在main()中调用 readSetting()
    QString tPath = csgoPath;

    //第二步 尝试默认路径
    if( !isCsgoExisted(tPath) ){
        QStringList defCsgoPath;
        defCsgoPath
                <<  QStringLiteral("C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                 <<  QStringLiteral("C:/steam/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                  <<  QStringLiteral("D:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                   <<  QStringLiteral("E:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                    <<  QStringLiteral("F:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                     <<  QStringLiteral("G:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                      <<  QStringLiteral("H:/SteamLibrary/steamapps/common/Counter-Strike Global Offensive/csgo.exe")
                          ;

        for(int i = 0; i< defCsgoPath.size(); i++)
        {
            if( QFile::exists( defCsgoPath.at(i) ) ){
                tPath = defCsgoPath.at(i);
                break;
            }
        }
    }

    //第三步 根据Steam或国服启动器获得路径
    if( !isCsgoExisted(tPath) ){
        if( isSteamExisted() ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
        else if( isCsgoExisted() ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
    }

    //第四步 读取进程列表 要打开steam和csgo 会强制关闭CSGO
    if( !isCsgoExisted(tPath) ){
        QUrl url( "steam://rungameid/730" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgo.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgo.exe");
            cmd("taskkill /F /IM csgo.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( isCsgoExisted(tPath) ){
        csgoPath = tPath;
        onCsgoPathChanged();
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取CSGO路径失败，请手动选择！");
        return false;
    }
}

//手动设置路径
void MainWindow::on_manualBtn_clicked()
{
    QString tPath = "";
    if( !isSteamExisted() ){
        tPath = QFileDialog::getOpenFileName(this, "选择steam.exe的位置", tPath, "steam.exe");
        if( !isSteamExisted(tPath) ){
            return;
        }
        steamPath = tPath;
    }
    if( !isCsgoExisted()){
        tPath = QFileDialog::getOpenFileName(this, "选择csgo.exe的位置", tPath, "csgo.exe");
        if( !isCsgoExisted(tPath) ){
            return;
        }
        csgoPath = tPath;
    }
    //已有路径的处理 TODO: 路径设置好后直接enabled 0
    if( isSteamExisted() && isCsgoExisted() ){
        QMessageBox::warning(this, "提示", "路径已有，无需手动选择！");
    }
}

//手动选择csgo位置
void MainWindow::on_manualCsgoBtn_clicked()
{
    QString tPath = "";
    tPath = QFileDialog::getOpenFileName(this, "选择csgo.exe的位置", tPath, "csgo.exe");
    if( !isCsgoExisted(tPath) ){
        return;
    }
    csgoPath = tPath;
}

/*  3. 获取SteamID  */
//获取SteamID32，32比特，一般9位数字   TODO: 改进算法
void MainWindow::getSteamID()
{
    QString fileName = "";
    if( isSteamExisted() ){
        fileName = steamPath;
        fileName.replace("steam.exe" ,"userdata/", Qt::CaseInsensitive );
    }
    else if( isLauncherExisted() ){
        fileName = launcherPath;
        fileName.replace("csgolauncher.exe" ,"userdata/", Qt::CaseInsensitive );
    }
    else{
        QMessageBox::warning(this, "提示", "没有获得Steam或国服客户端路径！");
        return;
    }

    //读取userdata下所有包含数字的文件名得到SteamID
    QDir *dir=new QDir(fileName);
    QStringList filter;
    QString temp;
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    for(int i = 0; i < fileInfo->count(); i++)
    {
        temp = QString::fromStdString( ( fileInfo->at(i).fileName().toStdString() ) );
        if( isDigitStr( temp ) == true && !SteamIDList.contains( temp ))
            SteamIDList << temp;
    }
    for(int i = 0 ; i < SteamIDList.length() ; i++){
        temp = fileName + SteamIDList.at(i) + "/config/localconfig.vdf";
        QFile f( temp );
        if ( !f.exists() ) //文件不存在
            return;
        if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
            return;
        QTextStream fStream(&f); //用文本流读取文件
        fStream.setCodec("utf-8");  //解决乱码问题
        QString content =  fStream.readAll();
        f.close();
        //去除无用字符 查找昵称
        content = get_until(content, "communitypreferences");
        search_and_cut(content , "PersonaName");
        content.remove("\n");
        content.remove("\"");
        content.remove("\t");
        if( !PersonaNameList.contains( content ) )
            PersonaNameList << content;
    }

    //单用户时直接确定ID
    if(SteamIDList.length() == 1){
        steamID = SteamIDList.at(0);
        userName = PersonaNameList.at(0);
        onSteamIDChanged();
        QMessageBox::warning(this, "提示", "检测到只有一个用户ID，已设置好SteamID！\n SteamID = " + steamID);
    }
    //显示在tabview上 用户按一次按钮完成选择        //TODO: debug
    setupUserdata();
}

void MainWindow::setupUserdata()
{
    QFont font;
    font.setPixelSize(17);
    short n = SteamIDList.length();
    ui->userdata->setColumnCount(4);
    ui->userdata->setRowCount(n);
    ui->userdata->setColumnWidth(0,98);
    ui->userdata->setColumnWidth(1,180);
    ui->userdata->setColumnWidth(2,65);
    ui->userdata->setColumnWidth(3,65);

    QStringList header;
    header <<  "SteamID" << "昵称" << "" << "";
    ui->userdata->setHorizontalHeaderLabels(header);

    for(short i = 0; i < n; i++){
        ui->userdata->setRowHeight(i, 35);
        ui->userdata->setItem(i, 0, new QTableWidgetItem(SteamIDList.at(i)));
        ui->userdata->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->userdata->setItem(i, 1, new QTableWidgetItem(PersonaNameList.at(i)));
        ui->userdata->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        QPushButton *chooseBtn = new QPushButton();
        connect(chooseBtn, SIGNAL(clicked()), this, SLOT(onChooseBtnClicked()));
        QPushButton *DeleteBtn = new QPushButton();
        connect(DeleteBtn, SIGNAL(clicked()), this, SLOT(onUserDeleteBtnClicked()));
        //在QTableWidget中添加控件：删除按钮
        ui->userdata->setCellWidget(i, 2, DeleteBtn);
        DeleteBtn->setProperty("id", i);
        DeleteBtn->setProperty("text", "删除");
        DeleteBtn->setProperty("status", "normal");
        DeleteBtn->setFont(font);
        //选择按钮
        ui->userdata->setCellWidget(i, 3, chooseBtn);
        chooseBtn->setProperty("id", i);
        chooseBtn->setProperty("text", "选择");
        chooseBtn->setProperty("status", "normal");
        chooseBtn->setFont(font);
    }
    ui->userdata->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//选择按钮按下时切换SteamID
void MainWindow::onChooseBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->userdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    steamID = ui->userdata->item(row, 0)->text();
    userName = ui->userdata->item(row, 1)->text();
    onSteamIDChanged();
}

//删除按钮按下时删除对应用户信息
void MainWindow::onUserDeleteBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->userdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    QString temp = ui->userdata->item(row, 0)->text();
    ui->userdata->removeRow(row);

    row = SteamIDList.indexOf( temp );
    if( SteamIDList.contains( temp ) ){
        SteamIDList.removeAt( row );
        PersonaNameList.removeAt( row );
    }

    if( temp == steamID )
        steamID = "";

    onSteamIDChanged();
}

//手动输入SteamID TODO: 进一步加强
void MainWindow::on_ManualSteamID_clicked()
{
    QString tID = ui->textID->toPlainText();
    if( tID.isEmpty() )
        return;
    else if( !isDigitStr(tID) || tID.length() < 8 || tID.length() > 10 ){
        ui->debug->appendPlainText("ID格式有误！");
        return;
    }
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("Steam.exe", "userdata/" +tID, Qt::CaseInsensitive);
        if( QFile::exists( tPath ) ){
            steamID = tID;
            onSteamIDChanged();
            ui->debug->appendPlainText("成功！");
            ui->textID->setText("");
            return;
        }
        else
            ui->debug->appendPlainText("userdata目录下检测不到该ID对应的文件夹，注意检测ID正误");
    }
    else if( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" +tID, Qt::CaseInsensitive);
        if( QFile::exists( tPath ) ){
            steamID = tID;
            onSteamIDChanged();
            ui->debug->appendPlainText("成功！");
            ui->textID->setText("");
            return;
        }
        else
            ui->debug->appendPlainText("userdata目录下检测不到该ID对应的文件夹，注意检测ID正误");
    }
    else{
        ui->debug->appendPlainText("无Steam或国服启动器路径，无法识别，注意检测ID正误");
    }
}

//按钮槽函数
void MainWindow::on_getUserDataBtn_clicked()
{
    getSteamID();
}

/**
 *  拓展功能，基于核心模块和封装好的功能模块，完成具体的功能
 *  包括：
 *  1. 转换DEMO分享代码 获得真实下载链接
 *  2. 修复VAC验证问题
 *  3. 备份和还原设置（个人cfg文件夹）
 *  4. 国服反和谐和反和谐恢复
 *  5. 打开各种cfg文件夹和csgo位置
 */

/*------------------------------------------------------------------------------------*/
// 转换DEMO分享代码 获得真实下载链接  注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::sharecodeTransform()
{
    const QString ShareCodePattern = "^CSGO(-?[\\w]{5}){5}$";
    QRegExp reg( ShareCodePattern );     //正则表达式

    //显示提示
    ui->dispURL->setText("正在转换链接。。。");

    //1. 读入分享代码（参照之前的项目）
    QString ShareCode = ui->dragArea->toPlainText();
    if( !QString(ShareCode).isEmpty() )
        ui->dragArea->setText("");
    else
        return;

    //2. 去掉分享代码的无用信息（得到标准格式） "CSGO-xxx"的长度为34
    ShareCode = ShareCode.remove(0, ShareCode.lastIndexOf("CSGO-") ).left(34);

    //3. 正则表达式匹配
    if( !reg.exactMatch( ShareCode )){
        ui->dispURL->setText("分享链接格式不正确！");
        return;     //TODO: 链接格式不正确的处理
    }

    //4. 标准格式转换成获取3个id需要的字符串格式（20个字符）
    ShareCode.replace("CSGO-","");
    ShareCode.replace("-", "");

    //5. 调用@Yellowfisher编译的DLL库（核心代码源于github.com/akiver/CSGO-Demos-Manager）
    //    获得URL（需要DLL+boiler.exe+steam_api.dll+steam_appid.txt）
    //6. 打开Steam 关闭CSGO
    cmd("explorer.exe \"steam://rungame\"");
    cmd("taskkill /f /t /im csgo.exe");
    do{
        stall(100);
    }while( getProcessPath("csgo.exe").endsWith("exe") );

    //调用DLL库中函数 因为限制只能传进去char数组 故此处转换
    QByteArray ba=ShareCode.toLatin1();
    char *c = ba.data();
    QString retValue = QString::number( api_Urlstring( c ) );
    if( retValue != "0" ){
        ui->dispURL->setText( " 转换失败！错误代码：" + retValue );
        return;
    }

    QFile f("URL.txt");
    if ( !f.exists() ) //文件不存在
        return;
    if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;
    QTextStream fStream(&f); //用文本流读取文件
    fStream.setCodec("utf-8");  //解决乱码问题
    QString URL =  fStream.readAll();
    URL.replace("\n", "");
    URL.replace("\r", "");
    f.close();

    if ( QFile::exists("matches.dat") )
        QFile::remove("matches.dat");
    if ( QFile::exists("URL.txt") )
        QFile::remove("URL.txt");

    if( URL.startsWith("http://") )
        ui->dispURL->setText(URL);
    if( autoDownload == true )
        on_downURL_clicked();
    if( autoClip == true )
        on_clipURL_clicked();
}

//开始下载DEMO
void MainWindow::on_downURL_clicked()
{
    QString URL = ui->dispURL->toPlainText();
    if( URL.startsWith("http://") ){
        QUrl url( URL );
        QDesktopServices::openUrl(url);
    }
    else
        return;
}

//复制DEMO下载链接
void MainWindow::on_clipURL_clicked()
{
    QString URL = ui->dispURL->toPlainText();
    if( URL.startsWith("http://") ){
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(URL);                     //设置剪贴板内容</span>
    }
    else
        return;
}

//自动下载checkbox按下
void MainWindow::on_autoDownload_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoDownload = true;
    else
        autoDownload = false;
}

//自动复制checkbox按下
void MainWindow::on_autoClip_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoClip = true;
    else
        autoClip = false;
}

//拖拽区域文字发生改变时调用 注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::on_dragArea_textChanged()
{
    sharecodeTransform();
}

/*------------------------------------------------------------------------------------*/
//修复VAC验证问题 TODO:
void MainWindow::solveVacIssue()
{
    //检查路径是否正确
    QString tPath;
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "", Qt::CaseInsensitive);
    }
    else if( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "", Qt::CaseInsensitive);
    }
    else{
        QMessageBox::warning(this, "提示", "未获得Steam或国服路径！");
        return;
    }
    //检查用到的批处理脚本
    if( !QFile::exists( QCoreApplication::applicationDirPath() + "/SolveVAC.bat" ) ){
        QMessageBox::warning(this, "提示", "SolveVAC.bat文件丢失！");
        return;
    }

    QString command = "\"" + QCoreApplication::applicationDirPath() + "/SolveVAC.bat\" " + tPath;
    QString output = cmd( command );
    QMessageBox::warning(this, "提示", "已调出批处理脚本，如果不好使请到QQ群反馈，见'关于'");
}

//槽函数
void MainWindow::on_solveVAC_clicked()
{
    solveVacIssue();
}

/*------------------------------------------------------------------------------------*/
//备份设置
void MainWindow::on_backupSetting_clicked()
{
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString zipName = current_date_time.toString("yyyy-MM-dd_hh-mm-ss_%1").arg(userName.left(6)) + ".zip";
    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    if (!dir.exists(tPath))   dir.mkpath(tPath);
    //压缩
    JlCompress::compressDir(zipPath + zipName, tPath);
    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "备份成功！");
    refreshBackup();
}

//刷新备份backupdata域
void MainWindow::refreshBackup()
{   //显示在tablewidget上 用户按一次按钮完成选择
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir(zipPath);
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);

    QStringList nameFilters;
    nameFilters << "*.zip";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    short n = files.length();
    ui->backupdata->setColumnCount(3);
    ui->backupdata->setRowCount(n);
    ui->backupdata->setColumnWidth(0,268);
    ui->backupdata->setColumnWidth(1,70);
    ui->backupdata->setColumnWidth(2,70);

    QFont font;
    font.setPixelSize(17);
    QStringList header;
    header <<  "备份文件" << "" << "" ;
    ui->backupdata->setHorizontalHeaderLabels(header);

    for(short i = 0; i < n; i++){
        ui->backupdata->setRowHeight(i, 35);
        ui->backupdata->setItem(i, 0, new QTableWidgetItem(files.at( n - i - 1 )));

        QPushButton *DeleteButton = new QPushButton();
        connect(DeleteButton, SIGNAL(clicked()), this, SLOT(onDeleteBtnClicked()));
        ui->backupdata->setCellWidget(i, 1, DeleteButton);
        DeleteButton->setProperty("id", i);
        DeleteButton->setProperty("text", "删除");
        DeleteButton->setProperty("status", "normal");
        DeleteButton->setFont(font);

        QPushButton *RestoreButton = new QPushButton();
        connect(RestoreButton, SIGNAL(clicked()), this, SLOT(onRestoreBtnClicked()));
        ui->backupdata->setCellWidget(i, 2, RestoreButton);
        RestoreButton->setProperty("id", i);
        RestoreButton->setProperty("text", "还原");
        RestoreButton->setProperty("status", "normal");
        RestoreButton->setFont(font);
    }
    ui->backupdata->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//恢复按钮槽函数
void MainWindow::onRestoreBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->backupdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    //检查ID和Path
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QString zipName = ui->backupdata->item(row, 0)->text();
    QString bakName = "CFG备份.zip";
    QString bakPath = tPath;
    bakPath.replace("cfg", "");
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    if (!dir.exists(tPath))   dir.mkpath(tPath);
    //如果备份文件不存在，报提示
    if( !QFile(zipPath + zipName).exists() ){
        QMessageBox::warning(this, "提示", "该备份已不存在，还原失败！");
        return;
    }

    //把当前cfg文件夹内容备份在local中，删除cfg文件夹内容
    JlCompress::compressDir(bakPath + bakName, tPath);
    dir.setPath(tPath);
    dir.removeRecursively();
    //dir.mkdir(tPath);

    JlCompress::extractDir(zipPath + zipName, tPath);
    QMessageBox::warning(this, "提示", "还原成功！");
    ui->debug->setPlainText( "row = " + QString::number(row) );
}

//删除按钮槽函数
void MainWindow::onDeleteBtnClicked()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr){
        return;
    }
    QModelIndex idx = ui->backupdata->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = idx.row();
    //检查ID和Path
    if( steamID.isEmpty() )
        return;
    QString tPath = "";
    if( isSteamExisted() ){
        tPath = steamPath;
        tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    }
    else return;

    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QString zipName = ui->backupdata->item(row, 0)->text();
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    //如果备份文件不存在，报提示
    if( !QFile(zipPath + zipName).exists() ){
        QMessageBox::warning(this, "提示", "该备份已不存在，删除失败！");
        return;
    }

    QFile::remove(zipPath + zipName);
    ui->backupdata->removeRow(row);
    QMessageBox::warning(this, "提示", "删除成功！");
    refreshBackup();
}

//备份按钮槽函数
void MainWindow::on_openBackupLoc_clicked()
{
    QString zipPath = QCoreApplication::applicationDirPath() + "/备份/";
    QDir dir;
    //如果路径不存在则创建
    if (!dir.exists(zipPath))   dir.mkpath(zipPath);
    QUrl url( "file:///" + zipPath);
    QDesktopServices::openUrl(url);
}

//备份区域折叠/展开切换
void MainWindow::on_zipBackupdata_clicked()
{
    if( backupdataZipped == true ){
        ui->backupdata->setGeometry(10, 410, 410, 100);
        ui->zipBackupdata->setText("↑");
        ui->getUserDataBtn->setVisible(true);
        backupdataZipped = false;
    }
    else{
        ui->backupdata->setGeometry(10, 130, 410, 380);
        ui->zipBackupdata->setText("↓");
        ui->getUserDataBtn->setVisible(false);
        backupdataZipped = true;
    }
}

/*------------------------------------------------------------------------------------*/
//国服反和谐
void MainWindow::on_antiHarmony_clicked()
{
    QStringList files;
    QString tPath = "";
    if( isCsgoExisted() ){
        tPath = csgoPath;
        tPath.replace("csgo.exe", "csgo/", Qt::CaseInsensitive);
    }
    else return;
    files << "pakxv_audiochinese_000" << "pakxv_audiochinese_001" << "pakxv_audiochinese_002"
          << "pakxv_audiochinese_003"   << "pakxv_audiochinese_004" <<  "pakxv_audiochinese_dir"
          << "pakxv_lowviolence_000"      << "pakxv_lowviolence_dir"        <<  "pakxv_perfectworld_000"
          <<"pakxv_perfectworld_001"     <<  "pakxv_perfectworld_dir";
    //.vpk->.bak
    for (int i = 0; i < files.length() ; i++) {
        if( QFile::exists( tPath + files.at(i) + ".vpk" ) ){
            //先检测并删除.bak，否则rename不会覆盖
            if( QFile::exists( tPath + files.at(i) + ".bak" ) )
                QFile::remove( tPath + files.at(i) + ".bak" );
            QFile::rename(tPath + files.at(i) + ".vpk", tPath + files.at(i) + ".bak");
        }
    }

    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "反和谐成功！");
}

//恢复反和谐
void MainWindow::on_reloadHarmony_clicked()
{
    QStringList files;
    QString tPath = "";
    if( isCsgoExisted() ){
        tPath = csgoPath;
        tPath.replace("csgo.exe", "csgo/", Qt::CaseInsensitive);
    }
    else return;
    files << "pakxv_audiochinese_000" << "pakxv_audiochinese_001" << "pakxv_audiochinese_002"
          << "pakxv_audiochinese_003"   << "pakxv_audiochinese_004" <<  "pakxv_audiochinese_dir"
          << "pakxv_lowviolence_000"      << "pakxv_lowviolence_dir"        <<  "pakxv_perfectworld_000"
          <<"pakxv_perfectworld_001"     <<  "pakxv_perfectworld_dir";

    //.bak->.vpk
    for (int i = 0; i < files.length() ; i++) {
        if( QFile::exists( tPath + files.at(i) + ".bak" ) ){
            //先检测并删除.vpk，否则rename不会覆盖
            if( QFile::exists( tPath + files.at(i) + ".vpk" ) )
                QFile::remove( tPath + files.at(i) + ".vpk" );
            QFile::rename(tPath + files.at(i) + ".bak", tPath + files.at(i) + ".vpk");
        }
    }

    //TODO: 压缩后的操作
    QMessageBox::warning(this, "提示", "反和谐已恢复！");
}

/*------------------------------------------------------------------------------------*/
//打开local/cfg文件夹
void MainWindow::on_openlocalcfg_clicked()
{
    if( !isSteamExisted() || steamID.isEmpty() ){
        return;
    }
    QString tPath = steamPath;
    tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);

    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开国服启动器local/cfg文件夹 TODO:
void MainWindow::on_openCNlocalcfg_clicked()
{
    if( !isLauncherExisted() ){
        return;
    }
    QString tPath = launcherPath;
    tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开csgo/cfg文件夹
void MainWindow::on_opencsgocfg_clicked()
{
    if( !isCsgoExisted() ){
        return;
    }

    QString tPath = csgoPath;
    tPath.replace("csgo.exe", "csgo/cfg", Qt::CaseInsensitive);

    // 检查目录是否存在，若不存在则新建
    QDir dir;
    if (!dir.exists(tPath))
        dir.mkpath(tPath);

    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开csgo位置
void MainWindow::on_opencsgoDir_clicked()
{
    if( isCsgoExisted() ){
        QString tPath = csgoPath;
        tPath.replace("csgo.exe", "", Qt::CaseInsensitive);
        QDir dir;
        QUrl url( "file:///" + tPath);
        QDesktopServices::openUrl(url);
    }
    else return;
}

/*------------------------------------------------------------------------------------*/
//切换电源计划相关
void MainWindow::on_high_clicked()
{
    //三代锐龙
    if( ui->Ryzen3000->isChecked() )
        cmd("powercfg.exe /S 9935e61f-1661-40c5-ae2f-8495027d5d5d");
    //通用
    else if( ui->RyzenEtc->isChecked() || ui->allPlatform->isChecked() )
        cmd("powercfg.exe /S 8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c");
    else
        QMessageBox::warning(this, "提示", "没有选择CPU类型！");
}

void MainWindow::on_balanced_clicked()
{
    //AMD
    if( ui->Ryzen3000->isChecked() || ui->RyzenEtc->isChecked() )
        cmd("powercfg.exe /S 9897998c-92de-4669-853f-b7cd3ecb2790");
    //通用
    else if( ui->allPlatform->isChecked() )
        cmd("powercfg.exe /S 381b4222-f694-41f0-9685-ff5bb260df2e");
    else
        QMessageBox::warning(this, "提示", "没有选择CPU类型！");
}

void MainWindow::on_powersave_clicked()
{
    //通用
    if( ui->Ryzen3000->isChecked() ||ui->RyzenEtc->isChecked() || ui->allPlatform->isChecked() )
        cmd("powercfg.exe /S a1841308-3541-4fab-bc81-f71556f20b4a");
    else
        QMessageBox::warning(this, "提示", "没有选择CPU类型！");
}

void MainWindow::on_Ryzen3000_clicked()
{
    cpuType = 1;
    //ui->high->setChecked(false);
}

void MainWindow::on_RyzenEtc_clicked()
{
    cpuType = 2;
}

void MainWindow::on_allPlatform_clicked()
{
    cpuType = 3;
}

/*------------------------------------------------------------------------------------*/
//启动项相关
void MainWindow::on_LaunchPwrd1_clicked()
{
    launchOption1 = ui->LaunchOpt1->text();
    QUrl url( "steam://rungameid/730//-perfectworld " + launchOption1 );
    QDesktopServices::openUrl(url);
}

void MainWindow::on_LaunchWwd1_clicked()
{
    launchOption1 = ui->LaunchOpt1->text();
    QUrl url( "steam://rungameid/730//-worldwide " + launchOption1 );
    QDesktopServices::openUrl(url);
}

void MainWindow::on_LaunchPwrd2_clicked()
{
    launchOption2 = ui->LaunchOpt2->text();
    QUrl url( "steam://rungameid/730//-perfectworld " + launchOption2 );
    QDesktopServices::openUrl(url);
}

void MainWindow::on_LaunchWwd2_clicked()
{
    launchOption1 = ui->LaunchOpt2->text();
    QUrl url( "steam://rungameid/730//-worldwide " + launchOption2 );
    QDesktopServices::openUrl(url);
}

/*------------------------------------------------------------------------------------*/
//获取电脑配置信息
void MainWindow::getPCconfig()
{
    //定义变量，CPU等可直接输出故不需要
    QString GPU = "", MEM = "", MEMManu = "", MEMClockSpeed = "", Drive = "", tPath;

    ui->PCconfig->setText("");
    //系统
    ui->PCconfig->append( "系统：\t" + wmic("os", "Caption").remove("Microsoft ") + " " + wmic("os", "OSArchitecture") );//32/64位
    //ui->PCconfig->append( wmic("os", "RegisteredUser") );//用户名
    //处理器
    ui->PCconfig->append( "处理器：\t" + get_until( wmic("cpu", "name").left(35), " @" ) );
    //ui->PCconfig->append( "\t" + wmic("cpu", "NumberOfCores")
    //                      + "核心" + wmic("cpu", "NumberOfLogicalProcessors") + "线程" );
    //主板
    ui->PCconfig->append( "主板：\t" + wmic("baseboard", "product") );
    ui->PCconfig->append( "BIOS：\t" + wmic("bios", "SMBIOSBIOSVersion") + " 版本" );//bios版本
    //显卡    TODO:
    GPU = wmic("path win32_VideoController", "name ");
    QStringList GPUlist = GPU.split("\r\n");
    GPU = "显卡：";
    for( int i = 0; i < GPUlist.length() ; i++){
        GPUlist[i] = GPUlist.at(i).trimmed();
        GPU = GPU + "\t" + GPUlist.at(i) + "\n";
    }
    GPU = GPU.trimmed();
    ui->PCconfig->append( GPU );//显卡型号

    //内存    TODO:
    MEM = wmic("memorychip", "PartNumber");
    MEMClockSpeed = wmic("memorychip", "Speed");
    MEMManu = wmic("memorychip", "Manufacturer");
    QStringList MEMlist = MEM.split("\r\n");
    QStringList MEMClockSpeedlist = MEMClockSpeed.split("\r\n");
    QStringList MEMManulist = MEMManu.split("\r\n");
    MEM = "内存：";
    for( int i = 0; i < MEMlist.length() && i < 2 ; i++){
        MEMManulist[i] = MEMManulist.at(i).trimmed().left(14);
        MEMlist[i] = MEMlist.at(i).trimmed().left(20);
        MEM = MEM + "\t" + MEMManulist.at(i) + " " + MEMlist.at(i) + "\n";
    }
    MEM = MEM + "频率：\t";
    for( int i = 0; i < MEMClockSpeedlist.length() && i < 2 ; i++){
        MEMClockSpeedlist[i] = MEMClockSpeedlist.at(i).trimmed();
        MEM = MEM + MEMClockSpeedlist.at(i) + "MHz / ";
    }
    MEM = MEM.trimmed();
    MEM.chop(1);
    if( MEMlist.length() > 2 )  MEM = MEM + "  等 总共" + QString::number(MEMlist.length()) + "条" ;
    ui->PCconfig->append( MEM );//内存型号和频率

    //硬盘    TODO:
    Drive = wmic("diskdrive", "model");
    QStringList Drivelist = Drive.split("\r\n");
    Drive = "硬盘：";
    for( int i = 0; i < Drivelist.length() && i < 2 ; i++){
        Drivelist[i] = Drivelist.at(i).trimmed().left(23);
        Drive = Drive + "\t" + Drivelist.at(i) + "\n";
    }
    Drive = Drive.trimmed();
    if( Drivelist.length() > 2 )  Drive = Drive + " 等 总共" + QString::number(Drivelist.length()) + "块" ;
    ui->PCconfig->append( Drive );//硬盘型号

    //显示器
    QString Monitor = wmic("desktopmonitor", "PNPDeviceID");
    Monitor.replace("DISPLAY\\", "");
    Monitor = get_until(Monitor, "\\");
    Monitor = "显示器：\t"  + Monitor + " " + wmic("path win32_VideoController", "CurrentHorizontalResolution")//长
            + "x" + wmic("path win32_VideoController", "CurrentVerticalResolution")//宽
            + "@" + wmic("path win32_VideoController", "CurrentRefreshRate") + "Hz";//刷新率

    ui->PCconfig->append( Monitor );

    /*-----------------------  读取游戏视频设置  -----------------------*/
    if( isSteamExisted() == true ){
        tPath = steamPath;
        tPath.replace("steam.exe", "", Qt::CaseInsensitive);
    }
    else if ( isLauncherExisted() == true ){
        tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "", Qt::CaseInsensitive);
    }
    else
        return;

    if( !steamID.isEmpty() ){
        ui->PCconfig->append("==============================");
        tPath = tPath + "/userdata/" + steamID + "/730/local/cfg/video.txt";

        QFile f( tPath );
        if ( !f.exists() ) //文件不存在
            return;
        if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
            return;
        QTextStream fStream(&f); //用文本流读取文件
        fStream.setCodec("utf-8");  //解决乱码问题
        QString content =  fStream.readAll();
        f.close();
        //将固定格式的video.txt内容转换为QSettings能识别的格式
        search_and_cut(content, "{\n");
        content.replace("}\n", "");
        content.replace("\t\t", "=");
        content.replace("\t", "");
        content.replace("\"", "");
        content.replace("\"", "");
        content.replace("setting.", "");
        //保存到video.ini下，读取得到

        QDir dir;
        //如果路径不存在则创建
        if (!dir.exists())   dir.mkpath("./video.ini");
        QFile file("./video.ini");

        if (!file.open(QIODevice::WriteOnly|QIODevice::Text))
            return;
        else
        {
            QTextStream stream(&file);
            stream << content;
            stream.flush();
            file.close();
        }

        QSettings *iniRead = new QSettings("./video.ini", QSettings::IniFormat);
        iniRead->setIniCodec("utf-8");     //解决乱码问题

        QStringList videoConfig = {"defaultres" , "defaultresheight", "fullscreen", "csm_quality_level", "gpu_mem_level", "cpu_level", "gpu_level", "mat_queue_mode",
                                   "mat_antialias", "mat_software_aa_strength", "mat_forceaniso", "mat_vsync", "mat_motion_blur_enabled"};
        QString resolution = "分辨率：\t\t", shadow = "全局阴影效果：\t", texture = "模型/贴图细节：\t", effect = "效果细节：\t",
                light = "光影细节：\t", multi_core = "多核渲染：\t", MSAA = "多重采样抗锯齿模式:\t", FXAA = "快速近似抗锯齿：\t",
                texture_filter = "贴图过滤模式：\t", vsync = "垂直同步：\t", Motionblur = "动态模糊：\t";
        //适配Intel缩进
        if( wmic("cpu", "name").contains("Intel", Qt::CaseInsensitive)  == true ){
            effect.append("\t");light.append("\t");
            multi_core.append("\t");vsync.append("\t");Motionblur.append("\t");
        }

        resolution = resolution.append( iniRead->value( videoConfig.at(0) ).toString() + "x" + iniRead->value( videoConfig.at(1) ).toString() );
        switch ( iniRead->value( videoConfig.at(2)).toInt() ) {
        case 0:resolution = resolution.append("  窗口") ;break;
        case 1:resolution = resolution.append("  全屏") ;break;
        }
        ui->PCconfig->append( resolution );
        switch ( iniRead->value( videoConfig.at(3)).toInt() ) {
        case 0:shadow = shadow.append("非常低") ;break;
        case 1:shadow = shadow.append("低") ;break;
        case 2:shadow = shadow.append("中") ;break;
        case 3:shadow = shadow.append("高") ;break;
        }
        ui->PCconfig->append( shadow );
        switch ( iniRead->value( videoConfig.at(4)).toInt() ) {
        case 0:texture = texture.append("低") ;break;
        case 1:texture = texture.append("中") ;break;
        case 2:texture = texture.append("高") ;break;
        }
        ui->PCconfig->append( texture );
        switch ( iniRead->value( videoConfig.at(5)).toInt() ) {
        case 0:effect = effect.append("低") ;break;
        case 1:effect = effect.append("中") ;break;
        case 2:effect = effect.append("高") ;break;
        }
        ui->PCconfig->append( effect );
        switch ( iniRead->value( videoConfig.at(6)).toInt() ) {
        case 0:light = light.append("低") ;break;
        case 1:light = light.append("中") ;break;
        case 2:light = light.append("高") ;break;
        case 3:light = light.append("非常高") ;break;
        }
        ui->PCconfig->append( light );
        switch ( iniRead->value( videoConfig.at(7)).toInt() ) {
        case 0:multi_core = multi_core.append("关闭");break;
        case 1:multi_core = multi_core.append("开启");break;
        case 2:multi_core = multi_core.append("开启");break;
        case -1:multi_core = multi_core.append("开启");break;
        }
        ui->PCconfig->append( multi_core );
        switch ( iniRead->value( videoConfig.at(8)).toInt() ) {
        case 0:MSAA = MSAA.append("关闭") ;break;
        case 2:MSAA = MSAA.append("2X MSAA") ;break;
        case 4:MSAA = MSAA.append("4X MSAA") ;break;
        case 8:MSAA = MSAA.append("8X MSAA") ;break;
        }
        ui->PCconfig->append( MSAA );
        switch ( iniRead->value( videoConfig.at(9)).toInt() ) {
        case 0:FXAA = FXAA.append("关闭") ;break;
        case 1:FXAA = FXAA.append("开启") ;break;
        }
        ui->PCconfig->append( FXAA );
        switch ( iniRead->value( videoConfig.at(10)).toInt() ) {
        case 0:texture_filter = texture_filter.append("双线性") ;break;
        case 1:texture_filter = texture_filter.append("三线性") ;break;
        case 2:texture_filter = texture_filter.append("异向 2X") ;break;
        case 4:texture_filter = texture_filter.append("异向 4X") ;break;
        case 8:texture_filter = texture_filter.append("异向 8X") ;break;
        case 16:texture_filter = texture_filter.append("异向 16X") ;break;
        }
        ui->PCconfig->append( texture_filter );
        switch ( iniRead->value( videoConfig.at(11)).toInt() ) {
        case 0:vsync = vsync.append("关闭") ;break;
        case 1:vsync = vsync.append("开启") ;break;
        }
        ui->PCconfig->append( vsync );
        switch ( iniRead->value( videoConfig.at(12)).toInt() ) {
        case 0:Motionblur = Motionblur.append("关闭") ;break;
        case 1:Motionblur = Motionblur.append("开启") ;break;
        }
        ui->PCconfig->append( Motionblur );
    }
}

//槽函数
void MainWindow::on_save_clicked()
{
    getPCconfig();
}

//截图并复制
void MainWindow::on_screenshotClip_clicked()
{
    ui->PCconfig->setGeometry(10, 10, 410, 555);
    ui->save->setVisible(false);
    ui->screenshotClip->setVisible(false);
    ui->PCconfig->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
    QPixmap p = this->grab( QRect(10, 40, 415, 560) );
    //p.save("123.png", "PNG");
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setPixmap(p);

    ui->save->setVisible(true);
    ui->screenshotClip->setVisible(true);
    ui->PCconfig->setGeometry(10, 10, 410, 490);
    QMessageBox::warning(this, "提示", "截图已经复制到剪贴板");
}

/*------------------------------------------------------------------------------------*/

/**
 *  功能模块封装，给其他功能调用，减少代码冗余
 */

//停顿time(ms)时间
void MainWindow::stall(int time){
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//判断一个字符串是否为纯数字
bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s)
    {
        if(*s>='0' && *s<='9'){}
        else {
            bret = false;
            break;
        }
        s++;
    }
    return bret;
}

/*------------------------------------------------------------------------------------*/
//检查steam路径是否正确，返回布尔值，如果调用时使用参数，则检查该路径
bool MainWindow::isSteamExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = steamPath;
    if( QFile(tPath).exists() && tPath.endsWith("steam.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;

}

bool MainWindow::isLauncherExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = launcherPath;
    if( QFile(tPath).exists() && tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;
}

bool MainWindow::isCsgoExisted(QString tPath)
{
    if( tPath == "default" )
        tPath = csgoPath;
    if( QFile(tPath).exists() && tPath.endsWith("csgo.exe", Qt::CaseInsensitive) )
        return true;
    else
        return false;
}

/*------------------------------------------------------------------------------------*/
//获得进程路径，对cmd函数进行封装并调用wmic指令
QString MainWindow::getProcessPath(QString processName)
{   //processid,name
    processName = "wmic process where name='" + processName + "' get executablepath";
    processName = cmd(processName);
    processName = processName.replace("ExecutablePath", "");
    processName = processName.replace("\r", "");
    processName = processName.replace("\n", "");
    processName = processName.simplified();

    return processName;
}

//调用CMD控制台指令并返回结果，正确执行时返回标准输出通道的内容，执行出错时返回错误通道的内容
QString MainWindow::cmd(QString command)
{
    QProcess p;
    QString temp;
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    temp = QString::fromLocal8Bit(p.readAllStandardOutput());
    p.close();
    return temp;
}

//调用CMD指令 并设定工作路径 TODO: 测试是否成功设置工作路径
QString MainWindow::cmd_dir(QString command, QString dir)
{
    QProcess p;
    QString temp;
    p.setWorkingDirectory(dir);
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    temp = QString::fromLocal8Bit(p.readAll());
    p.close();
    return temp;
}

//调用wmic获取硬件信息并提取 注意：调用cmd函数
QString MainWindow::wmic(QString name, QString info)
{
    QString command = "wmic " + name + " get " + info ;
    command = cmd(command);
    command.replace(info, "", Qt::CaseInsensitive);
    command = command.trimmed();
    return command;
}

/*------------------------------------------------------------------------------------*/
//剪切掉相应的部分 aaakeybbb -> bbb
QString MainWindow::search_and_cut(QString &input, QString key)
{
    qint32 i = input.indexOf(key, Qt::CaseInsensitive);
    if( i >= 0 )
        input.remove(0, i + key.length() );

    return "";
}

//把第一次出现end之前的串提取出来  xxxendyyy -> xxx
QString MainWindow::get_until(QString input, QString end)
{
    int i = input.indexOf(end, Qt::CaseInsensitive);
    return input.left( i );
}

//封装search_and_cut()和get_until()实现从字符串中提取 "key"  "data"格式的data串
QString MainWindow::getValue(QString input, QString key)
{
    key = "\"" + key + "\"";
    search_and_cut(input, key);
    search_and_cut(input, "\"");
    return get_until(input, "\"");
}

/*------------------------------------------------------------------------------------*/

/**
 *  其他SLOT槽函数
 */

void MainWindow::onSteamPathChanged()
{
    if( isSteamExisted() ){
        ui->checkSteamPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkSteamPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onCsgoPathChanged()
{
    if( isCsgoExisted() ){
        ui->checkCsgoPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkCsgoPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onLauncherPathChanged()
{
    if( isLauncherExisted() ){
        ui->checkLauncherPath->setCheckState(Qt::Checked);
    }
    else
        ui->checkLauncherPath->setCheckState(Qt::Unchecked);
}

void MainWindow::onSteamIDChanged()
{
    if( steamID.isEmpty() ){
        ui->checkSteamID->setText("ID: ×");
    }
    else
        ui->checkSteamID->setText("ID: " + steamID);
}

void MainWindow::on_sliderLaunchFirst_valueChanged(int value)
{
    if( value > 0 ){
        launchFirst = true;
        ui->sliderLaunchFirst->setValue(1);
        menuLaunchOpt->setTitle("当前启动项 ①");
    }
    else{
        launchFirst = false;
        ui->sliderLaunchFirst->setValue(0);
        menuLaunchOpt->setTitle("当前启动项 ②");
    }
}

void MainWindow::changeLaunchOpt()
{
    if( launchFirst == true )
        on_sliderLaunchFirst_valueChanged( 0 );
    else
        on_sliderLaunchFirst_valueChanged( 1 );
}

/*
    //自定义启动项指令 "F:\CSGO国服\csgolauncher.exe" -silent
    // 计算机\HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Valve\Steam\NSIS  Path
    //国服 计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
    //计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Compatibility Assistant\Store

    //获得计算机名
    //QString machineName = QHostInfo::localHostName();
    //ui->debug->setPlainText( machineName );
    //ui->debug->setPlainText( QCoreApplication::applicationDirPath() );    //exe所在路径
    //ui->debug->setPlainText( QDir::currentPath() ); //项目路径 当前路径

    if(QFile::exists(steamPath)) ;   //拖拽可能用到的代码
      if( tPath.endsWith("windeployqt.exe",Qt::CaseSensitive) ){
        steamPath.replace("file:///","");        //去除拖拽产生的前缀

        if( !QString(tPath).isEmpty() )

#include <QFile>

void ReadLine()
{

    QFile file("要读的文件路径");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            QString str(line);
            qDebug() << str;
            displayString << str;
        }
        file.close();

    }
}

void WriteLine()
{

    QFile file("要写的文件路径");
    if (file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.seek(file.size());
        for (auto& i : displayString)
        {
            QString qs;
            qs.append("Content:");
            qs.append(i);
            qs.remove("\n");
            stream << qs << "\n";
        }
        file.close();
    }
}

*/
