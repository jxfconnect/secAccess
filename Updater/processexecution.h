#ifndef PROCESSEXECUTION_H
#define PROCESSEXECUTION_H

#include <QObject>

class ProcessExecution : public QObject
{
    Q_OBJECT
public:
    explicit ProcessExecution(QObject *parent = nullptr);

    QString execCmd(QString cmd, QString args);
    QString execCmd(QString cmd, QStringList args);

signals:

};

#endif // PROCESSEXECUTION_H
