#include "firewallrule.h"

FirewallRule::FirewallRule(QObject *parent) : QObject(parent)
{
    process = new QProcess;
    log = new Log;
    log->loadConfiguration("logger.ini");
}

QString FirewallRule::execCmd(QString cmd, QString args)
{
    process->setProgram(cmd);
    QStringList argument;
    argument <<"/c" <<args;
    process->setArguments(argument);
    process->start();
    log->write(Log::TYPE_DEBUG, "Firewall", args);
    process->waitForStarted(); //等待程序启动
    process->waitForFinished();//等待程序关闭
    QString cmdlineResult=QString::fromLocal8Bit(process->readAllStandardOutput()); //程序输出信息
    log->write(Log::TYPE_DEBUG, "Firewall", cmdlineResult);

    return cmdlineResult;
}

void FirewallRule::createRule(QString clientIP)
{
    QString ruleName = QString("100sec_ip%1").arg(clientIP);
    QString ruleArgs = QString("netsh advfirewall firewall add rule name=100sec_ip%1 protocol=any dir=in action=allow remoteip=%2").arg(clientIP).arg(clientIP);
    execCmd("cmd", ruleArgs);
}

void FirewallRule::enableRule(QString ruleName)
{
    QString ruleArgs = QString("netsh advfirewall firewall set rule name=%1 new enable=yes").arg(ruleName);
    execCmd("cmd", ruleArgs);
}

void FirewallRule::disableRule(QString ruleName)
{
    QString ruleArgs = QString("netsh advfirewall firewall set rule name=%1 new enable=no").arg(ruleName);
    execCmd("cmd", ruleArgs);
}


