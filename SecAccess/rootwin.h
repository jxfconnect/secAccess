#ifndef ROOTWIN_H
#define ROOTWIN_H

#include "secserver.h"
#include "client.h"
#include "databasecomm.h"
#include "log.h"
#include "firewallrule.h"

#include <QWidget>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
//#include <QAction>
#include <QTimer>
#include <QDateTime>

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
    QAction *mShowServerAction;

    void createActions();
    void createMenu();

    SocketServer *mServer;
    QMap<QString, Client *> *mClients;
    DatabaseComm *dbComm;
    bool isClientExist(Client *c);
    void recordNewClient(Client *c);
    void getClientsFromDB();

protected:
     void closeEvent(QCloseEvent *event);

private:
    Ui::RootWin *ui;

    bool isServerOpen;
    QTimer *mCommTimer;
    Log *log;

    void monitorClient();

private slots:
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_exitAppAction();
    void on_showServerAction();
    void setServerClosed();
    void OnReceive(QByteArray bytes);
    void validAuthorization();
    void addNewClient(QTcpSocket *client);
    void handleClientMessage(Client *c);
};

#endif // ROOTWIN_H
