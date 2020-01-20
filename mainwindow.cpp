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
    getPaths();
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

    //显示到ui中
    //ui->steamPath->setText(steamPath);
    //ui->launcherPath->setText(launcherPath);
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

    QString command = "explorer.exe \"steam://rungameid/730\"";
    cmd(command);

    do{
        steamPath = getProcessPath("steam.exe");
        csgoPath = getProcessPath("csgo.exe");
        stall(100);
    }while( !QFile::exists( steamPath ) | !QFile::exists( csgoPath ) );

    command = "taskkill /f /t /im csgo.exe";
    cmd(command);

    ui->debug->setPlainText( csgoPath + " + " + steamPath );
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
