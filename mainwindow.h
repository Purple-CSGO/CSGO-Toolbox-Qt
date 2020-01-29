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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void stall(int time);

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

    void on_restoreSetting_clicked();

    void on_transferSetting_clicked();

    void on_openCNlocalcfg_clicked();

    void on_ManualSteamID_clicked();

    void onTableBtnClicked();

    void on_getUserDataBtn_clicked();

    void on_getLauncherPathBtn_clicked();

    void on_antiHarmony_clicked();

    void on_reloadHarmony_clicked();

    void on_solveVAC_clicked();

private:
    Ui::MainWindow *ui;

    void readSetting();
    void writeSetting();
    void closeEvent(QCloseEvent *e);
    bool getSteamPath();
    bool getLauncherPath();
    bool getCsgoPath();
    void solveVacIssue(QString Path);
    QString getProcessPath(QString processName);
    QString cmd(QString command);
    QString cmd_dir(QString command, QString dir);
    void sharecodeTransform();
    bool isDigitStr(QString src);
    void getSteamID();
    //这三个函数封装用于读取vdf
    QString search_and_cut(QString &input, QString key);
    QString get_until(QString input, QString end);
    QString getValue(QString input, QString key);
};
#endif // MAINWINDOW_H
