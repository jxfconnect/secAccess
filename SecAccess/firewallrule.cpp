#include "firewallrule.h"

FirewallRule::FirewallRule(QObject *parent) : QObject(parent)
{
    process = new QProcess;
    log = new Log;
    log->loadConfiguration("logger.ini");
}

QString FirewallRule::getArgs() const
{
    return args;
}

void FirewallRule::setArgs(const QString &value)
{
    args = value;
}

QString FirewallRule::getTypeName() const
{
    return typeName;
}

void FirewallRule::setTypeName(const QString &value)
{
    typeName = value;
}

QString FirewallRule::getRuleName() const
{
    return ruleName;
}

void FirewallRule::setRuleName(const QString &value)
{
    ruleName = value;
}

QString FirewallRule::getClientIP() const
{
    return clientIP;
}

void FirewallRule::setClientIP(const QString &value)
{
    clientIP = value;
}

void FirewallRule::initRule()
{
    ruleName = QString("100sec_ip%1%2").arg(clientIP).arg(typeName);
    disableRule();
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

void FirewallRule::createRule()
{
    ruleName = QString("100sec_ip%1%2").arg(clientIP).arg(typeName);
    QString ruleArgs = QString("netsh advfirewall firewall add rule name=%1 protocol=any dir=in action=allow remoteip=%2 %3")
            .arg(ruleName).arg(clientIP).arg(args);
    execCmd("cmd", ruleArgs);
}

void FirewallRule::enableRule()
{
    QString ruleArgs = QString("netsh advfirewall firewall set rule name=%1 new enable=yes").arg(ruleName);
    execCmd("cmd", ruleArgs);
}

void FirewallRule::disableRule()
{
    QString ruleArgs = QString("netsh advfirewall firewall set rule name=%1 new enable=no").arg(ruleName);
    execCmd("cmd", ruleArgs);
}

void FirewallRule::deleteRule()
{
    QString ruleArgs = QString("netsh advfirewall firewall delete rule name=%1 dir=in").arg(ruleName);
    execCmd("cmd", ruleArgs);
}
