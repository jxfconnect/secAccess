#include "wmiccommand.h"

#include <QProcess>
#include <QDebug>
#include <QCryptographicHash>

WMICCommand::WMICCommand(QObject *parent) : QObject(parent)
{
    log = new Log;
    log->loadConfiguration("logger.ini");
}

QString WMICCommand::execCmd(QString cmd, QString args)
{
    QProcess process;
    process.setProgram(cmd);
    QStringList argument;
    argument <<"/c" <<args;
    process.setArguments(argument);
    process.start();
    process.waitForStarted(); //等待程序启动
    process.waitForFinished();//等待程序关闭
    QString cmdlineResult=QString::fromLocal8Bit(process.readAllStandardOutput());

//    qDebug() << cmdlineResult;

    return cmdlineResult;
}

QString WMICCommand::getCPUId()
{
    QString cmdResult = execCmd("cmd","wmic cpu get processorid");
    QString cpuid = cmdResult.split("\r\r\n").at(1).trimmed();
    qDebug() << "CPUID:" << cpuid;

    return cpuid;
}

QString WMICCommand::getCPUName()
{
    QString cmdResult = execCmd("cmd","wmic cpu get Name");
    QString cpuName = cmdResult.split("\r\r\n").at(1).trimmed();
    qDebug() << "CPU Name:" << cpuName;
//    log->write(Log::TYPE_DEBUG, "WMICCommand::getCPUName", "CPU Name:" + cpuName);

    return cpuName;
}

QString WMICCommand::getBaseBoardSerialNumber()
{
    QString cmdResult = execCmd("cmd","wmic BaseBoard get SerialNumber");
    QString code = cmdResult.split("\r\r\n").at(1).trimmed();
    qDebug() << "BaseBoardSerialNumber:" << code;
//    log->write(Log::TYPE_DEBUG, "WMICCommand::getBaseBoardSerialNumber", "BaseBoardSerialNumber:" + code);

    return code;
}

QString WMICCommand::getDiskDriveSerialNumber()
{
    QString cmdResult = execCmd("cmd","wmic diskdrive get serialnumber");
    QString code = cmdResult.split("\r\r\n").at(1).trimmed();
    qDebug() << "DiskDriveSerialNumber:" << code;
//    log->write(Log::TYPE_DEBUG, "WMICCommand::getDiskDriveSerialNumber", "DiskDriveSerialNumber:" + code);

    return code;
}

QString WMICCommand::getUUID()
{
    QString cmdResult = execCmd("cmd","wmic csproduct get uuid");
    QString code = cmdResult.split("\r\r\n").at(1).trimmed();
    qDebug() << "UUID:" << code;
//    log->write(Log::TYPE_DEBUG, "WMICCommand::getUUID", "UUID:" + code);

    return code;
}

QString WMICCommand::getDevCode()
{
    QString deviceCode=QString("{%1}{%2}{%3}{%4}").arg(getCPUId()).arg(getBaseBoardSerialNumber()).arg(getDiskDriveSerialNumber())
            .arg(getUUID());
    QString md5;
    QByteArray ba,bb;
    QCryptographicHash md(QCryptographicHash::Md5);
//    ba.append(deviceCode);
    ba = deviceCode.toLocal8Bit();
    md.addData(ba);
    bb = md.result();
    md5.append(bb.toHex());

    qDebug() << "device code:" << md5;
//    log->write(Log::TYPE_DEBUG, "WMICCommand::getDevCode", "crypted code:" + md5);

    return md5;
}

bool WMICCommand::isProcessRunning(QString processName)
{
    bool status = false;

    QString cmdResult = execCmd("cmd","tasklist");
    status = cmdResult.contains(processName);

    return status;
}

bool WMICCommand::isProcessesRunning(QStringList processList)
{
    QString cmdResult = execCmd("cmd","tasklist");
    for(int i=0; i<processList.size(); i++)
    {
         if(!cmdResult.contains(processList.at(i)))
             return false;
    }

    return true;
}
