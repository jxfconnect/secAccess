#include "authstatus.h"

#include <QDebug>
#include <QFile>
#include <QSettings>

AuthStatus::AuthStatus(QObject *parent) : QObject(parent)
{
    isAuthorized = false;
    lastValidTime = QDateTime::currentDateTime();
    configPath = "auth.ini";
    loadConfiguration();
}

void AuthStatus::setAuthStatus(QByteArray bytes)
{
    qDebug() << bytes;
    lastValidTime =QDateTime::currentDateTime();
    QString current_date =lastValidTime.toString("yyyy.MM.dd hh:mm:ss.zzz");
    qDebug() << current_date;
    isAuthorized = true;
}

bool AuthStatus::isClientAuthoried()
{
    qDebug() << "Time: " <<lastValidTime.secsTo(QDateTime::currentDateTime());
    if(!isAuthorized)
        return false;
    if(lastValidTime.secsTo(QDateTime::currentDateTime())>30)
        return false;
    return true;
}

bool AuthStatus::loadConfiguration()
{
    if (!QFile::exists(configPath)) {
        qDebug() << QString("Auth configuration file %1 does not exist.").arg(configPath);
        return false;
    }
    QSettings setting(configPath,QSettings::IniFormat);
    authKey = setting.value("auth.key").toString();
    serverName = setting.value("auth.server").toString();
    port = setting.value("auth.port").toInt();

    return true;
}

void AuthStatus::setAuthKey(QString authKey)
{
    if (!QFile::exists(configPath)) {
        qDebug() << QString("Auth configuration file %1 does not exist.").arg(configPath);
        return;
    }
    QSettings setting(configPath,QSettings::IniFormat);
    setting.setValue("auth.key", authKey);

    this->authKey = authKey;
}
