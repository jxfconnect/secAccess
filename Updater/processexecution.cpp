#include "processexecution.h"

#include <QProcess>

ProcessExecution::ProcessExecution(QObject *parent) : QObject(parent)
{

}

QString ProcessExecution::execCmd(QString cmd, QString args)
{
    QProcess process;
    process.setProgram(cmd);
    QStringList argument;
    argument <<"/c" <<args;
    process.setArguments(argument);
    process.start();
    process.waitForStarted(); //等待程序启动
    process.waitForFinished(30000);//等待程序关闭
    QString cmdlineResult=QString::fromLocal8Bit(process.readAllStandardOutput());

    return cmdlineResult;
}

QString ProcessExecution::execCmd(QString cmd, QStringList args)
{
    QProcess process;
    process.setProgram(cmd);
    QStringList argument;
    argument = args;
    process.setArguments(argument);
    process.start();
    process.waitForStarted(); //等待程序启动
    process.waitForFinished(30000);//等待程序关闭
    QString cmdlineResult=QString::fromLocal8Bit(process.readAllStandardOutput());

    return cmdlineResult;
}
