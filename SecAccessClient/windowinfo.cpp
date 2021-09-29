#include "windowinfo.h"

WindowInfo::WindowInfo(QObject *parent) : QObject(parent)
{

}

QString WindowInfo::getMainWndID() const
{
    return mainWndID;
}

void WindowInfo::setMainWndID(const QString &value)
{
    mainWndID = value;
}

QString WindowInfo::getProcPath() const
{
    return procPath;
}

void WindowInfo::setProcPath(const QString &value)
{
    procPath = value;
}

QString WindowInfo::getWinTitle() const
{
    return winTitle;
}

void WindowInfo::setWinTitle(const QString &value)
{
    winTitle = value;
}
