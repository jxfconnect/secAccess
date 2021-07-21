#include "rootwin.h"

#include <QApplication>
#include <QSharedMemory>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QString strPath(QDir::toNativeSeparators(QApplication::applicationDirPath()));
//    qDebug() << strPath;

    a.setQuitOnLastWindowClosed(false);
    QSharedMemory singleton(a.applicationName());
    if(!singleton.create(1))  {    //程序已经在运行
        return -1;
    }


    RootWin w;
//    w.show();
    return a.exec();
}
