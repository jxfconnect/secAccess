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

    QString ruleName;
    QString clientIP;
    QString typeName;
    QString args;

    void createRule();
    void enableRule();
    void disableRule();
    void deleteRule();
    QString execCmd(QString cmd, QString args);
    bool isRuleExist(QString ruleName);
    void initRule();

    QString getClientIP() const;
    void setClientIP(const QString &value);

    QString getRuleName() const;
    void setRuleName(const QString &value);

    QString getTypeName() const;
    void setTypeName(const QString &value);

    QString getArgs() const;
    void setArgs(const QString &value);

signals:

private:
    QProcess *process;
    Log *log;

};

#endif // FIREWALLRULE_H
