#ifndef WMICCOMMAND_H
#define WMICCOMMAND_H

#include "log.h"

#include <QObject>
#include <QList>

class WMICCommand : public QObject
{
    Q_OBJECT
public:
    explicit WMICCommand(QObject *parent = nullptr);

    QString execCmd(QString cmd, QString args);
    QString getCPUId();
    QString getCPUName();
    QString getDevCode();
    QString getBaseBoardSerialNumber();
    QString getDiskDriveSerialNumber();
    QString getUUID();

    bool isProcessRunning(QString processName);
    bool isProcessesRunning(QStringList processList);

    Log *log;

signals:

};

#endif // WMICCOMMAND_H
