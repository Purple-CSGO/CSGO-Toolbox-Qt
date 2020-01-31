#include "mainwindow.h"
#include <windows.h>

const QString appName = "CSGO工具箱";

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_Use96Dpi);
    QApplication a(argc, argv);

    QPixmap pixmap(":/file/img/Loading.png");
    QSplashScreen screen(pixmap);
    screen.show();
//    screen.showMessage("LOVE", Qt::AlignCenter, Qt::red);
    a.processEvents();

    MainWindow w;
    w.setWindowTitle(appName + " - " + APP_VERSION);
    w.show();

    screen.finish(&w);
    return a.exec();
}
