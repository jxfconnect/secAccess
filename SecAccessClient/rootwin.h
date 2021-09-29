#ifndef ROOTWIN_H
#define ROOTWIN_H

#include "secclient.h"
#include "authstatus.h"
#include "log.h"

#include <QWidget>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
//#include <QAction>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class RootWin;
}

class RootWin : public QWidget
{
    Q_OBJECT

public:
    explicit RootWin(QWidget *parent = nullptr);
    ~RootWin();

    QSystemTrayIcon *mSysTrayIcon;
    QMenu *mMenu;
    QAction *mExitAppAction;
    QAction *mShowClientAction;

    void createActions();
    void createMenu();

    SocketComm *sc;
    AuthStatus *as;
    QString mLocalIP;
    QString mLocalMAC;
    int appVersion;
    Log *log;

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::RootWin *ui;

    bool isClientOpen;
    QTimer *mCommTimer;

    void commWithServer();
    void validAuthorization(QJsonObject message);
    void upgradeApp();

private slots:
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_exitAppAction();
    void on_showClientAction();
    void setClientClosed();
    void pingToServer();
    void OnReceive(QByteArray bytes);
};

#endif // ROOTWIN_H
