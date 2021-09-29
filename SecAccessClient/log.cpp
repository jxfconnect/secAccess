#include "log.h"
#include <QFile>
#include <QDir>
#include <QtDebug>
#include <QSettings>
#include <QDateTime>

Log::Log(QObject *parent) : QObject(parent)
{
    isConfigured = false;
}

void Log::loadConfiguration(QString configPath)
{
    if (!QFile::exists(configPath)) {
        qDebug() << QString("configuration file %1 for this logger is not exist.").arg(configPath);
        return;
    }

    QSettings setting(configPath,QSettings::IniFormat);
    QString dateSuffix = QDateTime::currentDateTime().toString(setting.value("logger.appender.daily.datePattern").toString())+".log";
    logPath = setting.value("logger.appender.daily.file").toString()+dateSuffix;
    isConfigured = true;
}

void Log::write(int type, QString moduleName, QString message)
{
    if(!isConfigured)
        return;

    QString typeName;
    switch (type) {
        case TYPE_INFO:
            typeName = "[INFO]";
            break;
        case TYPE_DEBUG:
            typeName = "[DEBUG]";
            break;
        case TYPE_WARNING:
            typeName = "[WARNING]";
            break;
        case TYPE_ERROR:
            typeName = "[ERROR]";
            break;
        default:
            typeName = "[UNKNOWN]";
    }

    QFile logFile(logPath);
    if(!logFile.open( QIODevice::Append | QIODevice::Text )) {
        qDebug() << QString("fail open log file %1").arg(logPath);
    } else {
        QTextStream ts(&logFile);
        ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") <<" " << typeName <<" ["<<moduleName << "] " << message << "\n";
        logFile.close();
    }
}
