#include "mainwindow.h"
#include "ui_mainwindow.h"

QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";
QString steamID = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSetting();
    //getPaths();
    sharecodeTransform();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//关闭主界面时保存设置
void MainWindow::closeEvent(QCloseEvent *e)
{
        writeSetting();
        e->accept();
}

//读取设置
void MainWindow::readSetting()
{
    //QSettings在.ini文件不存在时自动生成
    QSettings *iniRead = new QSettings("./config.ini", QSettings::IniFormat);
    iniRead->beginGroup("Paths");
    steamPath = iniRead->value("steamPath").toString();
    launcherPath = iniRead->value("launcherPath").toString();
    csgoPath = iniRead->value("csgoPath").toString();
    iniRead->endGroup();

    //读入完成后删除指针
    delete iniRead;
}

//写入设置
void MainWindow::writeSetting()
{
    //从ui中读取信息
    //steamPath = ui->steamPath->text();
    //launcherPath = ui->launcherPath->text();

    //写设置
    QSettings *IniWrite = new QSettings("./config.ini", QSettings::IniFormat);

    IniWrite->beginGroup("AppInfo");
    IniWrite->setValue("Name", qAppName());
    IniWrite->setValue("Version", APP_VERSION);
    IniWrite->endGroup();

    IniWrite->beginGroup("Paths");
    IniWrite->setValue("steamPath", steamPath);
    IniWrite->setValue("launcherPath", launcherPath);
    IniWrite->setValue("csgoPath", csgoPath);
    IniWrite->endGroup();
    //写入完成后删除指针
    delete IniWrite;
}

void MainWindow::stall(int time){
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime )
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::getPaths()
{
    //第一步 读取设置之后检查各种Path变量 如果成功则return
    //第二布 尝试默认路径 如果成功则return
    //第三步 读取注册表中路径 如果成功则return      √调试成功
//    for /f "tokens=1,2,* " %%i in ('REG QUERY "HKEY_CURRENT_USER\SOFTWARE\Valve\Steam" ^| find /i "SteamPath"') do set "SteamPath=%%k"
//HKEY_CURRENT_USER\SOFTWARE\Valve\Steam 位置查询SteamExe  SteamPath是文件夹的路径

    #define RUN_KEY "HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam"
    QSettings *pReg = new QSettings(RUN_KEY, QSettings::NativeFormat);

        //pReg->setValue("key", "value"); //设置注册表值

        QString path = pReg->value("SteamExe").toString(); //读取注册表值
        //v.toString();
        delete pReg;

        ui->debug->setPlainText(path);
    //第四步 调用自动获取路径的算法（从进程列表中获得） 如果成功则return
    //第五步 初始化失败 用户手动选择路径
/*
    //设置默认steam路径，如果匹配直接设定steamPath
    QStringList defSteamPath;
    defSteamPath
            <<  QStringLiteral("C:\\Program Files (x86)\\Steam")
            ;

    for(int i = 0; i< defSteamPath.size(); i++)
    {
        if( QFile::exists( defSteamPath.at(i) ) ){
            steamPath = defSteamPath.at(i);
            break;
        }
    }
*/

    //getProcessPath("qq.exe");
/*
    QString command = "explorer.exe \"steam://rungameid/730\"";
    cmd(command);

    do{
        steamPath = getProcessPath("steam.exe");
        csgoPath = getProcessPath("csgo.exe");
        stall(100);
    }while( !QFile::exists( steamPath ) | !QFile::exists( csgoPath ) );

    command = "taskkill /f /t /im csgo.exe";
    cmd(command);

    ui->debug->setPlainText( csgoPath + " + " + steamPath );*/

    //获得计算机名
    //QString machineName = QHostInfo::localHostName();
    //ui->debug->setPlainText( machineName );
    //ui->debug->setPlainText( QCoreApplication::applicationDirPath() );    //exe所在路径
    //ui->debug->setPlainText( QDir::currentPath() ); //项目路径 当前路径
/*     if(QFile::exists(steamPath)) ;

      if( tPath.endsWith("windeployqt.exe",Qt::CaseSensitive) ){
        steamPath.replace("file:///","");        //去除拖拽产生的前缀

        if( !QString(tPath).isEmpty() )

    QProcess p;
    QString command = toolPath + " " + deployPath + " " + "-no-angle -no-opengl-sw";
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    QString OutMsg = QString::fromLocal8Bit(p.readAllStandardOutput());
    QString ErrMsg = QString::fromLocal8Bit(p.readAllStandardError());
*/
}

QString MainWindow::getProcessPath(QString processName)
{
    //processid,name
    processName = "wmic process where name='" + processName + "' get executablepath";
    processName = cmd(processName);
    processName = processName.replace("ExecutablePath", "");
    processName = processName.replace("\r", "");
    processName = processName.replace("\n", "");
    processName = processName.simplified();
    //Debug
    ui->debug->setPlainText( processName );
    return processName;
}

//调用CMD控制台指令并返回结果，正确执行时返回标准输出通道的内容，执行出错时返回错误通道的内容
QString MainWindow::cmd(QString command)
{
    QProcess p;
    QString temp;
    p.start(command);
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();

    temp = QString::fromLocal8Bit(p.readAllStandardOutput());
    temp = temp.replace("\r", "");
    temp = temp.replace("\n", "");
    if( QString(temp).isEmpty() )
        temp = QString::fromLocal8Bit(p.readAllStandardError());
    ui->debug->setPlainText( command );
    return temp;
}

void MainWindow::solveVacIssue()
{
/*
思路：
    1. 关闭steam或者国服启动器
    2. 开启 Network Connections
        开启 Remote Access Connection Manager
        开启 Telephony
        开启 Windows Firewall
   3. 恢复 Data Execution Prevention 启动设置为默认值
   4. 获取steam或国服启动器目录（Done）
   5. 重装Steam Services
            steamservice  /uninstall
            steamservice  /install
        # 等待出现"Add firewall exception failed for steamservice.exe"
   6. 启动Steam Services服务
            start "Steam Client Service"
   7. 其他诸如设置快捷方式 打开网吧模式
            ? start /high steam -console -cafeapplaunch -forceservice
*/
}

// 转换DEMO分享代码 获得真实下载链接
void MainWindow::sharecodeTransform()
{
    const QString DICTIONARY = "ABCDEFGHJKLMNOPQRSTUVWXYZabcdefhijkmnopqrstuvwxyz23456789";
    //0. 读入分享代码
    QString ShareCode = "steam://rungame/730/76561202255233023/+csgo_download_match%20CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ";

    //1. 去掉分享代码的无用信息
    ShareCode = "CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ";
    QString("s");
    //2. 正则表达式匹配

    const QString ShareCodePattern = "^CSGO(-?[\\w]{5}){5}$";
    QRegExp reg( ShareCodePattern );     //正则表达式

    if( ShareCode.count("CSGO-") == 1 ){
        ShareCode.remove(0, ShareCode.lastIndexOf("CSGO-") );
        ShareCode.replace("CSGO-","");
        ShareCode.replace("-", "");
        ui->debug->setPlainText( ShareCode );
    }
    else    //>1
        ui->debug->setPlainText("有多个链接，暂不支持");
    //else  =0 链接不正确

    //big = big * 字典长度 + 当前字符索引位置
    BigUnsigned big = 0;
    for(int i = ShareCode.length() - 1; i >= 0; i++)
        big = big * DICTIONARY.length() + DICTIONARY.indexOf( ShareCode.at(i) );

    //QByteArray all = QArray(big);

    //if(reg.exactMatch( ShareCode )){
    //    ui->debug->setPlainText("1");
    //}
    //else



    //steam://rungame/730/76561202255233023/+csgo_download_match%20CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ


    //通过有用信息提取三个参数
    //CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ
    //注：满足正则表达式之后trim掉CSGO-之前和dJ后面的信息






    //调用boiler.exe 给cmd函数传参
    //boiler.exe path_to_output_file [matchid outcomeid tokenid]
    //boiler.exe match.dat 3392010210108244359 3392017082055917700 18964

    //在match.dat中读取某行的参数***

    //无符号64位整型
    quint64 MatchId =0, OutcomeId = 0, TokenId = 0;

    MatchId =0, OutcomeId =
            0, TokenId = 0;


}
