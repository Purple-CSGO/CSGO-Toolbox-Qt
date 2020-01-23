#ifndef PTHREAD_H
#define PTHREAD_H
#include <QThread>
#include <QByteArray>

//引入库文件，位置必须是绝对地址，有改变务必改动
#pragma comment(lib, "E:/QtProject/CSGO Toolbox/CSGO-Toolbox/libShareCodeToURLcs.lib" )
__declspec(dllimport) int api_Urlstring(const char* a);

class getURLThread : public QThread
{
public:
    getURLThread();
    void closeThread();

protected:
    void run();

private:
    volatile bool isStop;       //isStop是易失性变量，需要用volatile进行申明
};


#endif // PTHREAD_H
