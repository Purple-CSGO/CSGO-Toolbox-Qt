#include "mainwindow.h"
#include <windows.h>

const QString appName = "CSGO工具箱";

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);

    //Loading界面
    QPixmap pixmap(":/file/img/Loading.png");
    QSplashScreen screen(pixmap);
    screen.show();
    //screen.showMessage("LOVE", Qt::AlignCenter, Qt::red);     //Loading界面显示文字
    a.processEvents();

    //主界面
    MainWindow w;
    w.setWindowTitle(appName + " - " + APP_VERSION);    //设置标题
    w.dispForm();

    //Loading结束
    screen.finish(&w);
    return a.exec();
}
