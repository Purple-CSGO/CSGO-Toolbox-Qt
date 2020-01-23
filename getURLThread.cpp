#include "getURLThread.h"
#include <QDebug>

getURLThread::getURLThread()
{
    //构造函数
    isStop = false;
}

void getURLThread::closeThread()
{
    isStop = true;
}

void getURLThread::run()
{
    while(true)
    {
        if(isStop)
            return;
        QString ShareCode;
        QByteArray ba=ShareCode.toLatin1();
        char *c = ba.data();
        QString::number( api_Urlstring( c ) );
        qDebug()<<tr("URL线程正在运行！")<<QThread::currentThreadId();
        sleep(1);
    }
}
