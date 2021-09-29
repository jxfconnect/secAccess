#include "rootwin.h"
#include "ui_rootwin.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMap>

RootWin::RootWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RootWin)
{
    ui->setupUi(this);

    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon icon = QIcon(":/img/sec_lock.png");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QObject::tr("测试系统托盘图标"));
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,
            SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    //建立托盘操作的菜单
    createActions();
    createMenu();
    //在系统托盘显示此对象
    mSysTrayIcon->show();

    log = new Log;
    log->loadConfiguration("logger.ini");
    mServer = new SocketServer;
    mClients = new QMap<QString, Client *>;
    connect(mServer, SIGNAL(newClient(QTcpSocket *)), this, SLOT(addNewClientConnection(QTcpSocket *)));

    dbComm = new DatabaseComm;
    dbComm->loadConfiguration("mysql.ini");
    if(!dbComm->open())
    {
        QMessageBox::critical(NULL, "critical", "Cannot connect to database.",
                           QMessageBox::Close, QMessageBox::Close);
        on_exitAppAction();
    }

    getClientsFromDB();
    monitorClient();

    isServerOpen = false;
    on_showServerAction();
}

RootWin::~RootWin()
{
    delete ui;
}

void RootWin::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        mSysTrayIcon->showMessage(QObject::tr("Message Title"),
                                  QObject::tr("欢迎使用此程序"),
                                  QSystemTrayIcon::Information,
                                  1000);
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
//        this->show();
        on_showServerAction();
        break;
    default:
        break;
    }
}

void RootWin::createActions()
{
    mShowServerAction = new QAction(QObject::tr("主界面"),this);
    connect(mShowServerAction,SIGNAL(triggered()),this,SLOT(on_showServerAction()));

    mExitAppAction = new QAction(QObject::tr("退出"),this);
    connect(mExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));

}

void RootWin::createMenu()
{
    mMenu = new QMenu(this);
    mMenu->addAction(mShowServerAction);
    mMenu->addSeparator();
    mMenu->addAction(mExitAppAction);

    mSysTrayIcon->setContextMenu(mMenu);
}

void RootWin::on_exitAppAction()
{
    mSysTrayIcon->hide();
    exit(0);
}

//unused
void RootWin::closeEvent(QCloseEvent *event)
{

    if(mSysTrayIcon->isVisible())
    {
        hide(); //隐藏窗口
        event->ignore(); //忽略事件
    }
}

void RootWin::on_showServerAction()
{
    if(isServerOpen)
        return;

    SecServer *w = new SecServer;
    w->setAttribute(Qt::WA_DeleteOnClose);
    connect(w, SIGNAL(destroyed()),this,SLOT(setServerClosed()));

    w->dbcomm = this->dbComm;
    w->setClientList(mClients);
    w->init();

    w->show();
}

void RootWin::setServerClosed()
{
    isServerOpen = false;
}

void RootWin::OnReceive(QByteArray bytes)
{

}

void RootWin::getClientsFromDB()
{
    FirewallRule rule(this);

    QString sql = "select cid, ip, mac, auth_status, auth_key,rule_name, user_name, device_code from client where ip <> ''";
    QList<QMap<QString, QString>> clientList = dbComm->read(sql);
    for(int i=0; i<clientList.size(); i++)
    {
        QMap<QString, QString> clientRecord = clientList.at(i);
        Client *c = new Client;
        c->dbcomm = dbComm;
        c->cid = clientRecord.value("cid");
        c->ip = clientRecord.value("ip");
        c->mac = clientRecord.value("mac");
        c->userFullName = clientRecord.value("user_name");
        c->authKey = clientRecord.value("auth_key");
        c->deviceCode = clientRecord.value("device_code");
//        c->appVersion = clientRecord.value("app_version").toInt();
        c->registerStatus = clientRecord.value("auth_status");
        //client connection IP is automatically set to ip stored in database.
//        c->cIP = c->ip;
//        c->cMac = c->mac;
        //init firewall rule object.
        if(c->registerStatus == "Yes")
            c->init();

        mClients->insert(clientRecord.value("ip"), c);
    }
}

void RootWin::validAuthorization()
{
    QMap<QString, Client*>::iterator it = mClients->begin();
    while(it != mClients->end())
    {
        Client *c = it.value();
        c->updateAuthorization();
        it++;
    }
}

void RootWin::monitorClient()
{
    mCommTimer = new QTimer(this);
    mCommTimer->start(5000);
    connect(mCommTimer,SIGNAL(timeout()),this,SLOT(validAuthorization()));
}

void RootWin::addNewClientConnection(QTcpSocket *client)
{
    QString cIP = client->peerAddress().toString();
    Client *c;
    if(mClients->contains(cIP))
    {
        c = mClients->value(cIP);
        c->cIP = cIP;
    } else {
        c = new Client;
        c->dbcomm = dbComm;
        c->cIP = cIP;
    }
    c->setSocket(client);

//    connect(client, SIGNAL(readyRead()), c, SLOT(onReceive()));
    connect(c, SIGNAL(clientReady(Client *)), this, SLOT(handleClientMessage(Client *)));
}

void RootWin::handleClientMessage(Client *c)
{
    log->write(Log::TYPE_DEBUG, "Root Win::handleClientMessage", c->cmd);
    if(c->cmd == "register")
    {
        mClients->insert(c->cIP, c);
    }
}

//not use anymore
void RootWin::recordNewClient(Client *c)
{
    if(!isClientExist(c))
    {
        QMap<QString, QString> clientRecord;
        clientRecord.insert("ip",c->cIP);
        clientRecord.insert("mac", c->cMac);
        clientRecord.insert("auth_status", "N");
        dbComm->insert("client", clientRecord);
        c->ip = c->cIP;
        c->mac = c->cMac;
        mClients->insert(c->cIP, c);
    }
}

//not use anymore
bool RootWin::isClientExist(Client *c)
{
    QString sql = QString("select ip from client where ip='%1'").arg(c->cIP);
    QList<QMap<QString, QString>> clients = dbComm->read(sql);
    if(clients.size()>0)
        return true;
    return false;
}
