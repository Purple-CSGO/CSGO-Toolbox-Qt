#include "mainwindow.h"
#include "ui_mainwindow.h"
//引入库文件，位置必须是绝对地址，有改变务必改动
#pragma comment(lib, "E:/QtProject/CSGO Toolbox/CSGO-Toolbox/libShareCodeToURLcs.lib" )
__declspec(dllimport) int api_Urlstring(const char* a);

QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";
QString steamID = "";
QString userName = "";
bool autoClip = false;
bool autoDownload = false;

#define STEAM_KEY "HKEY_CURRENT_USER\\SOFTWARE\\Valve\\Steam" //注册表SteamExe项所在位置
#define Launcher_KEY "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam\\NSIS" //注册表CSGO项所在位置 TODO:
//TODO: 加载过程的文字显示在Loading界面上
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->labelDragArea->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    readSetting();
    getSteamPath();
    getLauncherPath();
    getCsgoPath();
    //solveVacIssue(steamPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//停顿time(ms)时间
void MainWindow::stall(int time){
    QTime dieTime = QTime::currentTime().addMSecs(time);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//关闭主界面时保存设置
void MainWindow::closeEvent(QCloseEvent *e)
{
    writeSetting();
    e->accept();
}

//读取设置  QSettings在.ini文件不存在时自动生成
void MainWindow::readSetting()
{
    QSettings *iniRead = new QSettings("./config.ini", QSettings::IniFormat);
    iniRead->setIniCodec("utf-8");     //解决乱码问题

    iniRead->beginGroup("Paths");
    steamPath = iniRead->value("steamPath").toString();
    launcherPath = iniRead->value("launcherPath").toString();
    csgoPath = iniRead->value("csgoPath").toString();
    iniRead->endGroup();

    iniRead->beginGroup("user");
    userName = iniRead->value("userName").toString();
    steamID = iniRead->value("steamID").toString();
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
    //判断当前路径变量是否正确，否则置空
    if( !QFile::exists( steamPath ) || !steamPath.endsWith("steam.exe", Qt::CaseInsensitive ) )
        steamPath = "";
    if( !QFile::exists( launcherPath ) || !launcherPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive ) )
        launcherPath = "";
    if( !QFile::exists( csgoPath ) || !csgoPath.endsWith("csgo.exe", Qt::CaseInsensitive ) )
        csgoPath = "";

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

    IniWrite->beginGroup("user");
    IniWrite->setValue("userName", userName);
    IniWrite->setValue("steamID", steamID);
    IniWrite->endGroup();

    IniWrite->beginGroup("auto");
    IniWrite->setValue("autoClip", autoClip);
    IniWrite->setValue("autoDownload", autoDownload);
    IniWrite->endGroup();
    //写入完成后删除指针
    delete IniWrite;
}

//获取Steam路径
bool MainWindow::getSteamPath()
{
    //第一步 读取设置中的steamPath 已经在main()中调用 readSetting()
    QString tPath = steamPath;

    //第二步 尝试默认路径
    if( !QFile(tPath).exists() || !tPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
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
            if( QFile::exists( defSteamPath.at(i) ) ){
                tPath = defSteamPath.at(i);
                break;
            }
        }
    }

    //第三步 读取注册表中路径  //HKEY_CURRENT_USER\SOFTWARE\Valve\Steam 查询SteamExe
    if( !QFile(tPath).exists() || !tPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
        QSettings *pReg = new QSettings(STEAM_KEY, QSettings::NativeFormat);
        tPath = pReg->value("SteamExe").toString(); //读取注册表值
        //pReg->setValue("key", "value");   //设置注册表值
        delete pReg;
    }

    //第四步 读取进程列表 要打开steam
    if( !QFile(tPath).exists() || !tPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("steam.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("steam.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( QFile(tPath).exists() && tPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
        steamPath = tPath;
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取Steam路径失败，请手动选择！");
        return false;
    }
}

//获取国服启动器LauncherPath路径
bool MainWindow::getLauncherPath()
{
    //第一步 读取设置中的launcherPath 已经在main()中调用 readSetting()
    QString tPath = launcherPath;

    //第二步 读取注册表中路径
    if( !QFile(tPath).exists() || !tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) ){
        QSettings *pReg = new QSettings(Launcher_KEY, QSettings::NativeFormat);
        tPath = pReg->value("Path").toString(); //读取注册表值
        delete pReg;
        if( !tPath.isEmpty() )
            tPath = tPath.append("\\csgolauncher.exe");
    }

    //第三步 读取进程列表 要打开国服启动器
    if( !QFile(tPath).exists() || !tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) ){
        QUrl url( "steam://rungame" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgolauncher.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgolauncher.exe");
        }
    }

    //第步 自动获取路径失败 提示用户手动选择路径
    if( QFile(tPath).exists() && tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) ){
        launcherPath = tPath;
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取国服客户端路径失败，请手动选择！");
        return false;
    }
}

//获取CSGO路径  TODO: 添加默认路径
bool MainWindow::getCsgoPath()
{
    //第一步 读取设置中的csgoPath 已经在main()中调用 readSetting()
    QString tPath = csgoPath;

    //第二步 尝试默认路径
    if( !QFile(tPath).exists() || !tPath.endsWith("csgo.exe", Qt::CaseInsensitive) ){
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
    if( !QFile(tPath).exists() || !tPath.endsWith("csgo.exe", Qt::CaseInsensitive) ){
        if( !QFile(steamPath).exists() || !tPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
        else if( !QFile(launcherPath).exists() || !tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) ){
            tPath = steamPath;
            tPath = tPath.replace("steam.exe", "steamapps/common/Counter-Strike Global Offensive/csgo.exe", Qt::CaseInsensitive);
        }
    }

    //第四步 读取进程列表 要打开steam和csgo 会强制关闭CSGO
    if( !QFile(tPath).exists() || !tPath.endsWith("csgo.exe", Qt::CaseInsensitive) ){
        QUrl url( "steam://rungameid/730" );
        QDesktopServices::openUrl(url);

        tPath = getProcessPath("csgo.exe");
        for(int count = 10; count >0 && !QFile::exists( tPath ) ; count-- ){
            stall(100);
            tPath = getProcessPath("csgo.exe");
            cmd("taskkill /f /t /im csgo.exe");
        }
    }

    //第五步 自动获取路径失败 提示用户手动选择路径
    if( QFile(tPath).exists() && tPath.endsWith("csgo.exe", Qt::CaseInsensitive) ){
        csgoPath = tPath;
        return true;
    }
    else{
        QMessageBox::warning(this, "提示", "自动获取CSGO路径失败，请手动选择！");
        return false;
    }
}

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
    temp = temp.replace("\r", "");
    temp = temp.replace("\n", "");
    if( QString(temp).isEmpty() )
        temp = QString::fromLocal8Bit(p.readAllStandardError());
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
    temp = QString::fromLocal8Bit(p.readAllStandardOutput());
    temp = temp.replace("\r", "");
    temp = temp.replace("\n", "");
    if( QString(temp).isEmpty() )
        temp = QString::fromLocal8Bit(p.readAllStandardError());
    p.close();
    return temp;
}

//修复VAC验证问题 TODO:
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

//获取SteamID，一般9位数字 TODO:
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

    //启动项修改: "Software" -> "730" -> "LaunchOptions" 利用String相关操作依次查找匹配的第一个串位置&删掉该串之前的内容
    //读取C:\Program Files (x86)\Steam\userdata\354813078\config\localconfig.vdf
    //CSGO没有安装好时没有"LaunchOptions" 这一项 手动在第一个"}"之前添加
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

}
//ui->tabWidget->setCurrentIndex(0); 设置当前tab第0页

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

//打开csgo/cfg文件夹
void MainWindow::on_opencsgocfg_clicked()
{
    if( !QFile::exists( csgoPath ) || !csgoPath.endsWith("csgo.exe", Qt::CaseInsensitive ) ){
        return;
    }

    QString tPath = csgoPath;
    tPath.replace("csgo.exe", "csgo/cfg", Qt::CaseInsensitive);
    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开local/cfg文件夹 TODO:
void MainWindow::on_openlocalcfg_clicked()
{
    if( !QFile::exists( steamPath ) || !steamPath.endsWith("steam.exe", Qt::CaseInsensitive ) ){
        return;
    }
    QString tPath = steamPath;
    tPath.replace("steam.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    if( !QFile(tPath).exists() )    //TODO: 路径不存在的处理 直接创建一个文件夹？
        tPath.replace(steamID + "/730/local/cfg", "");
    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//打开国服启动器local/cfg文件夹 TODO:
void MainWindow::on_openCNlocalcfg_clicked()
{
    if( !QFile::exists( launcherPath ) || !launcherPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive ) ){
        return;
    }
    QString tPath = launcherPath;
    tPath.replace("csgolauncher.exe", "userdata/" + steamID + "/730/local/cfg", Qt::CaseInsensitive);
    if( !QFile(tPath).exists() )    //TODO: 路径不存在的处理 直接创建一个文件夹？
        tPath.replace(steamID + "/730/local/cfg", "");
    QUrl url( "file:///" + tPath );
    QDesktopServices::openUrl(url);
}

//手动设置路径
void MainWindow::on_manual_clicked()
{
    QString tPath = "";
    if( QString(steamPath).isEmpty() ){
        tPath = QFileDialog::getOpenFileName(this, "选择steam.exe的位置", tPath, "steam.exe");
        if( !QFile::exists( tPath ) || !tPath.endsWith("steam.exe", Qt::CaseInsensitive ) ){
            return;
        }
        steamPath = tPath;
    }
    if( QString(csgoPath).isEmpty() ){
        tPath = QFileDialog::getOpenFileName(this, "选择csgo.exe的位置", tPath, "csgo.exe");
        if( !QFile::exists( tPath ) || !tPath.endsWith("csgo.exe", Qt::CaseInsensitive ) ){
            return;
        }
        csgoPath = tPath;
    }
    if( QString(launcherPath).isEmpty() ){
        tPath = QFileDialog::getOpenFileName(this, "选择国服启动器csgolauncher.exe的位置", tPath, "csgolauncher.exe");
        if( !QFile::exists( tPath ) || !tPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive ) ){
            return;
        }
        launcherPath = tPath;
    }
    //已有路径的处理 TODO: 路径设置好后直接enabled 0
    if( !QString(csgoPath).isEmpty() && !QString(steamPath).isEmpty() && !QString(launcherPath).isEmpty() ){
        QMessageBox::warning(this, "提示", "路径已有，无需手动选择！");
    }
}

//备份设置 TODO:
void MainWindow::on_backupSetting_clicked()
{

}

//恢复设置 TODO:
void MainWindow::on_restoreSetting_clicked()
{

}

//转移设置 TODO:
void MainWindow::on_transferSetting_clicked()
{

}

//手动输入SteamID TODO: 进一步加强
void MainWindow::on_ManualSteamID_clicked()
{
    if( ui->debug->toPlainText().isEmpty() )
        return;
    QString tID = ui->debug->toPlainText();
    QString tPath = "";
    ui->debug->appendPlainText("1");
    if( QFile(steamPath).exists() && steamPath.endsWith("steam.exe", Qt::CaseInsensitive) ){
        tPath = steamPath;
        tPath.replace("Steam.exe", "userdata/" +tID, Qt::CaseInsensitive);
        ui->debug->appendPlainText("2");
        if( QFile::exists( tPath ) ){
            steamID = tID;
            return;
        }
    }
    else if( QFile(launcherPath).exists() && launcherPath.endsWith("csgolauncher.exe", Qt::CaseInsensitive) ){
        QString tPath = launcherPath;
        tPath.replace("csgolauncher.exe", "userdata/" +tID, Qt::CaseInsensitive);
        ui->debug->appendPlainText("3");
        if( QFile::exists( tPath ) ){
            steamID = tID;
            return;
        }
    }
    else
        ui->debug->appendPlainText("ID错误！");
}
