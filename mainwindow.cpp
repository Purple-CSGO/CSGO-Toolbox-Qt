#include "mainwindow.h"
#include "ui_mainwindow.h"

QString steamPath = "";
QString launcherPath = "";
QString csgoPath = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    readSetting();
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
