#include "mainwindow.h"
#include "ui_mainwindow.h"
//引入库文件，位置必须是绝对地址，有改变务必改动
#pragma comment(lib, "E:/QtProject/CSGO Toolbox/CSGO-Toolbox/libShareCodeToURLcs.lib" )
__declspec(dllimport) int api_Urlstring(const char* a);

QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";
QString steamID = "";
bool autoClip = false;
bool autoDownload = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->labelDragArea->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    readSetting();
    getPaths();
    //solveVacIssue(steamPath);
    //ui->debug->appendPlainText( cmd("cmd.exe /c cd D:/") );

    //cmd("explorer \"" + steamPath.replace("steam.exe", "config\\") + "\"");
    //getSteamID();

    //ui->debug->appendPlainText( t );
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
    iniRead->setIniCodec("utf-8");     //解决乱码问题
    iniRead->beginGroup("Paths");
    steamPath = iniRead->value("steamPath").toString();
    launcherPath = iniRead->value("launcherPath").toString();
    csgoPath = iniRead->value("csgoPath").toString();
    iniRead->endGroup();

    iniRead->beginGroup("auto");
    autoClip = iniRead->value("autoClip").toBool();
    autoDownload = iniRead->value("autoDownload").toBool();
    iniRead->endGroup();
    //读入完成后删除指针
    delete iniRead;

    //如果路径不存在则置空
    if( !QFile::exists( steamPath ) || !steamPath.endsWith("steam.exe", Qt::CaseInsensitive ) )
        steamPath = "";
    if( !QFile::exists( launcherPath ) || !launcherPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive ) )
        launcherPath = "";
    if( !QFile::exists( csgoPath ) || !csgoPath.endsWith("csgo.exe", Qt::CaseInsensitive ) )
        csgoPath = "";

    //设置ui中的checkbox状态
    if(autoClip == true)
        ui->autoClip->setCheckState(Qt::Checked);
    else
        ui->autoClip->setCheckState(Qt::Unchecked);
    if(autoDownload == true)
        ui->autoDownload->setCheckState(Qt::Checked);
    else
        ui->autoDownload->setCheckState(Qt::Unchecked);
}

//写入设置
void MainWindow::writeSetting()
{
    //从ui中读取信息
    //steamPath = ui->steamPath->text();
    //launcherPath = ui->launcherPath->text();

    if( !QFile::exists( steamPath ) || !steamPath.endsWith("steam.exe", Qt::CaseInsensitive ) )
        steamPath = "";
    if( !QFile::exists( launcherPath ) || !launcherPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive ) )
        launcherPath = "";
    if( !QFile::exists( csgoPath ) || !csgoPath.endsWith("csgo.exe", Qt::CaseInsensitive ) )
        csgoPath = "";

    //ui->debug->setPlainText( steamPath );
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

    IniWrite->beginGroup("auto");
    IniWrite->setValue("autoClip", autoClip);
    IniWrite->setValue("autoDownload", autoDownload);
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

    steamPath = pReg->value("SteamExe").toString(); //读取注册表值
    //v.toString();
    delete pReg;

        //ui->debug->setPlainText(steamPath);

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
*/
}

//获得进程路径，对cmd函数进行封装并调用wmic指令
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
    //ui->debug->setPlainText( processName );
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
    //ui->debug->appendPlainText(temp);
    temp = temp.replace("\r", "");
    temp = temp.replace("\n", "");
    if( QString(temp).isEmpty() )
        temp = QString::fromLocal8Bit(p.readAllStandardError());
    //ui->debug->setPlainText(temp);
    p.close();
    return temp;
}

QString MainWindow::cmd_dir(QString command, QString dir)
{
    QProcess p;
    QString temp;
    p.setWorkingDirectory(dir);
    p.start(command);
    p.waitForStarted();
    p.closeWriteChannel();  //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    temp = QString::fromLocal8Bit(p.readAllStandardOutput());
    //ui->debug->appendPlainText(temp);
    temp = temp.replace("\r", "");
    temp = temp.replace("\n", "");
    if( QString(temp).isEmpty() )
        temp = QString::fromLocal8Bit(p.readAllStandardError());
    //ui->debug->setPlainText(temp);
    p.close();
    return temp;
}

//修复VAC验证问题
void MainWindow::solveVacIssue(QString Path)
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

    //1.
    cmd("taskkill /F /IM Steam.exe");
    cmd("taskkill /F /IM csgolauncher.exe");
    //2.
    cmd("sc config Netman start= AUTO");
    cmd("sc start Netman");
    cmd("sc config RasMan start= AUTO");
    cmd("sc start RasMan");
    cmd("sc config TapiSrv start= AUTO");
    cmd("sc start TapiSrv");
    cmd("sc config MpsSvc start= AUTO");
    cmd("sc start MpsSvc");
    cmd("netsh advfirewall set allprofiles state on");
    //3.
    cmd("bcdedit /deletevalue nointegritychecks");
    cmd("bcdedit /deletevalue loadoptions");
    cmd("bcdedit /debug off");
    cmd("bcdedit /deletevalue nx");
    //4.
    if( !QFile::exists(Path) )
        return;
    //5.
    steamservice  /uninstall
    ping -n 3 127.0.0.1>nul
    steamservice  /install
*/
    //debug测试
    ui->debug->appendPlainText( cmd("cd") );
    ui->debug->appendPlainText( cmd("cd " + steamPath) );
    ui->debug->appendPlainText( cmd("dir") );


}

QString MainWindow::search_and_cut(QString input, QString key)
{   //剪切不掉相应的部分
    QString t = input;
    qint32 i = t.indexOf(key);
    //ui->debug->appendPlainText();
    if( i >= 0 )
        input.remove(0, i + key.length() );
    else
            ui->debug->appendPlainText("GG");
    ui->debug->appendPlainText(t);
    return t;
}

QString MainWindow::get_until(QString input, QString end)
{
    return "";
}

QString MainWindow::getValue(QString input, QString key)
{
    return "";
}

//启动项修改: "Software" -> "730" -> "LaunchOptions" 利用String相关操作依次查找匹配的第一个串位置&删掉该串之前的内容
//读取C:\Program Files (x86)\Steam\userdata\354813078\config\localconfig.vdf
//CSGO没有安装好时没有"LaunchOptions" 这一项 手动在第一个"}"之前添加

void MainWindow::getSteamID()
{
    QString fileName = steamPath;
    fileName.replace("steam.exe" ,"config\\loginusers.vdf", Qt::CaseInsensitive );

    QFile f( fileName );
    if ( !f.exists() ) //文件不存在
        return;
    if ( !f.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;
    QTextStream fStream(&f); //用文本流读取文件
    fStream.setCodec("utf-8");  //解决乱码问题
    QString content =  fStream.readAll();
    f.close();

    content = search_and_cut(content ,"Name");
    //ui->debug->appendPlainText(content);
}
//ui->tabWidget->setCurrentIndex(0); 设置当前tab第0页
/*
//国服除了csgolauncher.exe名称不一样 CSGO应该是安装在steam位置之下 其他没有不同
//注册表似乎找不到对应表项

//获取用户ID: "users" -> " -> 读17位&检测是否纯数字 -> "AccountName"tabtab" ..."得到账号名 同理昵称
//                        "}..."" -> 继续读17位id 同上 ...
//利用String相关操作依次查找匹配的第一个串位置&删掉该串之前的内容
//读取C:\Program Files (x86)\Steam\config\loginusers.vdf
*/
/*"users"
{
    "76561198315078806"
    {
        "AccountName"		"_jerry_dota2"
        "PersonaName"		"Purp1e"
        ...若干行
    }
    "第二个id"
    {
        ...
    }
//或者在config.vdf中查找"SteamID" 或"Accounts"
                "Accounts"
                {
                    "_jerry_dota2"
                    {
                        "SteamID"		"76561198315078806"
                    }
                    "_im_ai_"
                    {
                        "SteamID"		"76561198107125441"
                    }
                }
*/

// 转换DEMO分享代码 获得真实下载链接  注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::sharecodeTransform()
{   //字典去掉了大写I 小写G 小写L 数字1和0
    const QString DICTIONARY = "ABCDEFGHJKLMNOPQRSTUVWXYZabcdefhijkmnopqrstuvwxyz23456789";
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
//两个示例分享链接
//steam://rungame/730/76561202255233023/+csgo_download_match%20CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ"
//steam://rungame/730/76561202255233023/+csgo_download_match%20CSGO-UoNbQ-zr5JZ-uW25m-qPBz8-PzBND"
//http://replay142.valve.net/730/003392017082055917700_0716589588.dem.bz2

//判断一个字符串是否为纯数字
bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)     //不是纯数字
        return false;
    else       //纯数字
        return true;
}

//拖拽区域文字发生改变时调用 注意这里不可对dragArea赋非空值否则会死循环
void MainWindow::on_dragArea_textChanged()
{
    sharecodeTransform();
}


/*

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

void MainWindow::on_clipURL_clicked()
{
    //QString originalText = clipboard->text();         //获取剪贴板上文本信息
    QString URL = ui->dispURL->toPlainText();
    if( URL.startsWith("http://") ){
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        clipboard->setText(URL);                     //设置剪贴板内容</span>
    }
    else
        return;
}

void MainWindow::on_autoDownload_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoDownload = true;
    else
        autoDownload = false;
}

void MainWindow::on_autoClip_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked )
        autoClip = true;
    else
        autoClip = false;
}
