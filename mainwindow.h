#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QApplication>
#include <QSplashScreen>
#include <QDateTime>
#include <QTextEdit>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QStandardItemModel>
#include <QFont>
#include <QScreen>
#include "lib/JlCompress.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_dragArea_textChanged();

    void on_downURL_clicked();

    void on_clipURL_clicked();

    void on_autoDownload_stateChanged(int arg1);

    void on_autoClip_stateChanged(int arg1);

    void on_opencsgocfg_clicked();

    void on_openlocalcfg_clicked();

    void on_manualBtn_clicked();

    void on_backupSetting_clicked();

    void on_openCNlocalcfg_clicked();

    void on_ManualSteamID_clicked();

    void on_getUserDataBtn_clicked();

    void on_getLauncherPathBtn_clicked();

    void on_antiHarmony_clicked();

    void on_reloadHarmony_clicked();

    void on_solveVAC_clicked();

    void onTableBtnClicked();

    void onRestoreBtnClicked();

    void onDeleteBtnClicked();

    void on_openBackupLoc_clicked();

    void on_zipBackupdata_clicked();

    void on_opencsgoDir_clicked();

private:
    Ui::MainWindow *ui;
    //全局变量
    QString steamPath;
    QString launcherPath;
    QString csgoPath;
    QString steamID;
    QString userName;
    bool autoClip;
    bool autoDownload;
    bool backupdataZipped;

    ///核心功能
    //程序启动/关闭时调用
    void setupUI();
    void readSetting();
    void writeSetting();
    void closeEvent(QCloseEvent *e);

    //获取各种路径
    bool getSteamPath();
    bool getLauncherPath();
    bool getCsgoPath();

    //获取SteamID
    void getSteamID();

    ///拓展功能
    //解决VAC验证问题
    void solveVacIssue();
    //分享代码转换直接下载链接
    void sharecodeTransform();
    //备份
    void refreshBackup();

    ///类似SLOT，发生变化时调用
    void onSteamPathChanged();
    void onCsgoPathChanged();
    void onLauncherPathChanged();
    void onSteamIDChanged();

    ///功能封装，供其他函数调用
    void stall(int time);
    bool isDigitStr(QString src);
    QString getProcessPath(QString processName);
    QString cmd(QString command);
    QString cmd_dir(QString command, QString dir);

    //这三个函数封装用于读取 "关键字" "值"中的值 ->ID、启动项
    QString search_and_cut(QString &input, QString key);
    QString get_until(QString input, QString end);
    QString getValue(QString input, QString key);
};
#endif // MAINWINDOW_H
