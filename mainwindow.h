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

private:
    Ui::MainWindow *ui;

    void readSetting();
    void writeSetting();
    void closeEvent(QCloseEvent *e);
    void getPaths();
    QString getProcessPath(QString processName);
    QString cmd(QString command);

};
#endif // MAINWINDOW_H
