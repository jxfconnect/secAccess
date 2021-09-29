#include "authstatus.h"
#include "systrayreader.h"

#include <QDebug>
#include <QFile>
#include <QSettings>

AuthStatus::AuthStatus(QObject *parent) : QObject(parent)
{
    isAuthorized = false;
    lastValidTime = QDateTime::currentDateTime();
    configPath = "auth.ini";
    loadConfiguration();

    wmic = new WMICCommand;
    genDeviceCode();
}

void AuthStatus::setAuthStatus(QJsonObject message)
{
    qDebug() << "setAuthStatus" << message;
    lastValidTime =QDateTime::currentDateTime();
    QString current_date =lastValidTime.toString("yyyy.MM.dd hh:mm:ss.zzz");
    qDebug() << "current date:" << current_date;
    QString responseStatus = message.take("STATUS").toString();
    if(responseStatus == "Y")
    {
        isAuthorized = true;
    } else {
        isAuthorized = false;
    }
}

bool AuthStatus::isClientAuthoried()
{
    qDebug() << "Elapsed time: " <<lastValidTime.secsTo(QDateTime::currentDateTime());
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

void AuthStatus::genDeviceCode()
{
    devCode = wmic->getDevCode();
}

bool AuthStatus::isCryptLogin()
{
    bool status = false;

    SysTrayReader tray;
    // 解决控制台中文 '？'
    setlocale(LC_ALL, "chs");

    // 获取托盘句柄
    HWND h_tray = tray.FindTrayWnd();
    HWND h_tray_fold = tray.FindNotifyIconOverflowWindow();

    QList<WindowInfo *> *infoList = new QList<WindowInfo *>;
    tray.EnumNotifyWindow(h_tray, infoList);
    foreach(WindowInfo *info, *infoList)
    {
        QString procPath = info->getProcPath();
        QString winTitle = info->getWinTitle();
        if(procPath.contains("EsafeNet\\Cobra DocGuard Client\\CDGRegedit.exe") && ((winTitle.contains("联机登录") && winTitle.contains("密文模式"))
                || winTitle.contains("您有未处理消息")))
            return true;
    }
    QList<WindowInfo *> *infoListFold = new QList<WindowInfo *>;
    tray.EnumNotifyWindow(h_tray_fold, infoListFold);
    foreach(WindowInfo *info, *infoListFold)
    {
        QString procPath = info->getProcPath();
        QString winTitle = info->getWinTitle();
        if(procPath.contains("EsafeNet\\Cobra DocGuard Client\\CDGRegedit.exe") && ((winTitle.contains("联机登录") && winTitle.contains("密文模式"))
                || winTitle.contains("您有未处理消息")))
            return true;
    }
    return status;
}

bool AuthStatus::isCryptClientRuning()
{
    QString procStr = "watchctrl.exe,CdgTwin64.exe,CdgTwin32.exe,CDGRegedit.exe";
    QStringList procList = procStr.split(",");
    bool procStatus = wmic->isProcessesRunning(procList);
    bool winStatus = isCryptLogin();
    if(procStatus && winStatus)
        return true;

    return false;
}
