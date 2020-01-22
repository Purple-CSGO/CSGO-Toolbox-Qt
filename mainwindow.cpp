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
    //ui->debug->setPlainText( command );
    return temp;
}

//修复VAC验证问题
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

//启动项修改: "Software" -> "730" -> "LaunchOptions" 利用String相关操作依次查找匹配的第一个串位置&删掉该串之前的内容
//读取C:\Program Files (x86)\Steam\userdata\354813078\config\localconfig.vdf
//CSGO没有安装好时没有"LaunchOptions" 这一项 手动在第一个"}"之前添加

//国服除了csgolauncher.exe名称不一样 CSGO应该是安装在steam位置之下 其他没有不同
//注册表似乎找不到对应表项

//获取用户ID: "users" -> " -> 读17位&检测是否纯数字 -> "AccountName"tabtab" ..."得到账号名 同理昵称
//                        "}..."" -> 继续读17位id 同上 ...
//利用String相关操作依次查找匹配的第一个串位置&删掉该串之前的内容
//读取C:\Program Files (x86)\Steam\config\loginusers.vdf
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
// 转换DEMO分享代码 获得真实下载链接
void MainWindow::sharecodeTransform()
{   //字典去掉了大写I 小写G 小写L 数字1和0
/*    const QString DICTIONARY = "ABCDEFGHJKLMNOPQRSTUVWXYZabcdefhijkmnopqrstuvwxyz23456789";
    const QString ShareCodePattern = "^CSGO(-?[\\w]{5}){5}$";
    QRegExp reg( ShareCodePattern );     //正则表达式

    //0. 读入分享代码（参照之前的项目）
    QString ShareCode = ui->dragArea->toPlainText();
    ShareCode = "steam://rungame/730/76561202255233023/+csgo_download_match%20CSGO-52um8-FaZvF-Ajutm-hCoKH-2JedJ";
    if( !QString(ShareCode).isEmpty() )
        ui->dragArea->setText("");
    //1. 去掉分享代码的无用信息（得到标准格式） "CSGO-xxx"的长度为34
    ShareCode = ShareCode.remove(0, ShareCode.lastIndexOf("CSGO-") ).left(34);
    //2. 正则表达式匹配
    if( !reg.exactMatch( ShareCode )){
        ui->debug->setPlainText("分享链接格式不正确！");
        return;     //TODO: 链接格式不正确的处理
    }
    //3. 标准格式转换成获取3个id需要的字符串格式（20个字符）
    ShareCode.replace("CSGO-","");
    ShareCode.replace("-", "");

    //ui->debug->setPlainText( ShareCode );

    //4. 20个字符需要查字典（见↑DICTIONARY）得到一个<58的位置
    //      用到base57的加密算法 得到一个144bit的大整数，

    //big = big * 字典长度 + 当前字符索引位置
    //得到144bits大整数 分割成三个id
    BigUnsigned big = 0;
    for(int i = ShareCode.length() - 1 ; i >= 0; i--){
        big.multiply(big, DICTIONARY.length());
        big += DICTIONARY.indexOf( ShareCode.at(i) );
    }

    QByteArray all = QString::fromStdString( bigIntegerToString( big ) ).toLatin1();
    if( all.length() == sizeof(qint64) + sizeof(qint64) + sizeof(qint16) ){
        all.append( "\0" );
    }
    //Reverse
    QByteArray t("");
    for(int i = 0  ;i < all.length()/2 ;i++){
        t[0] = all[ i ];
        all[ i ] = all[ all.length() - i - 1 ];
        all[ all.length() - i - 1 ] = t[0];
    }

    QString qstr = QString::fromStdString( bigIntegerToString( big ) );

    qstr = QString::fromStdString( all.toStdString() );

    ui->debug->appendPlainText( qstr );
*/
    //QByteArray all = QArray(big);


    //    20*8 = 160 bits  = 64bits MatchId + 64bits OutcomeId + 32bits TokenId
    //                                       8Byte                  8Byte                           4Byte


    //4.1 字符转换 查字典
    /*int* bytes = new int[20];
    for(int i = 0; i < 2; i++){
        bytes[i] = DICTIONARY.indexOf( ShareCode.at(1) );
    }
    //4.2 调用自己编写的ACC函数计算三个ID 和数值转换&累加器的工作原理类似
    quint64 MatchId_int = ACC(bytes, 0, 4);
    quint64 OutcomeId_int = ACC(bytes, 4, 8);
    quint64 TokenId_int = ACC(bytes, 12, 8);

    QString MatchId = tr("%1").arg(MatchId_int);
    QString OutcomeId = tr("%1").arg(OutcomeId_int);
    QString TokenId = tr("%1").arg(TokenId_int);

    ui->debug->appendPlainText( "TokenId: " + MatchId);
    ui->debug->appendPlainText( "OutcomeId: " + OutcomeId);
    ui->debug->appendPlainText( "TokenId: " + TokenId );
    */
    //调用boiler.exe 给cmd函数传参
    //boiler.exe path_to_output_file [matchid outcomeid tokenid]
    //boiler.exe match.dat 3392010210108244359 3392017082055917700 18964
    // 10111100010010110101110101000010000000000000000000000110000111
    // 10111100010010110111011001000010000000000000000000000010000100
    // 100101000010100

    // 100001110000000100000000100000000101000011010111000100100010111110000100000000000000000010000000100100001101110100010010001011110001010001001010
    // 11760499549408184579621369443876940960175178
    // Reverse之后
    // 87157106904967834496312697548180494599406711
    // 11111010001000001110110111100011010111000110010110110010101001111100100101110101101110000000100010001000101010011011110111101010010100010001110111
    //在match.dat中读取某行的参数***



}

//字节累加器 计算Byte序列的数值，bytes为整型数组 Index是数组中的低地址 length是计算的长度(Byte)
/*
 * quint64 MainWindow::ACC(int *bytes, int Index, int length){
    quint64 sum = 0;
    quint64 r = 1;  //8位->256
    for(int i = 1; i <= length; i++){
        sum += bytes[ Index + length - i] *r;
        r *= (2^8);
        //ui->debug->appendPlainText( tr("sum = %1").arg(sum);
    }
    return sum;
}*/
//判断一个字符串是否为纯数字
bool MainWindow::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

void MainWindow::on_dragArea_textChanged()
{
    //sharecodeTransform();
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
