#ifndef FIREWALLRULE_H
#define FIREWALLRULE_H

#include "log.h"

#include <QProcess>

#include <QObject>

class FirewallRule : public QObject
{
    Q_OBJECT
public:
    explicit FirewallRule(QObject *parent = nullptr);

    void createRule(QString clientIP);
    void enableRule(QString ruleName);
    void disableRule(QString ruleName);
    QString execCmd(QString cmd, QString args);
    bool isRuleExist(QString ruleName);

signals:

private:
    QProcess *process;
    Log *log;

};

#endif // FIREWALLRULE_H
