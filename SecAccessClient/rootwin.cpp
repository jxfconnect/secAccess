#include "rootwin.h"
#include "ui_rootwin.h"

#include <QJsonObject>
#include <QJsonDocument>

RootWin::RootWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RootWin)
{
    ui->setupUi(this);

    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
//    QIcon icon = QIcon("D:\\IT\\workplace\\SecAccessClient\\img\\sec_lock.png");
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

    sc = new SocketComm;
    connect(sc, SIGNAL(readyToPump(QByteArray)), this, SLOT(OnReceive(QByteArray)));
    as = new AuthStatus;

    commWithServer();

    isClientOpen = false;
    on_showClientAction();
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
        this->show();
        break;
    default:
        break;
    }
}

void RootWin::createActions()
{
    mShowClientAction = new QAction(QObject::tr("主界面"),this);
    connect(mShowClientAction,SIGNAL(triggered()),this,SLOT(on_showClientAction()));

    mExitAppAction = new QAction(QObject::tr("退出"),this);
    connect(mExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));

}

void RootWin::createMenu()
{
    mMenu = new QMenu(this);
    mMenu->addAction(mShowClientAction);
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

void RootWin::on_showClientAction()
{
    if(isClientOpen)
        return;

    SecClient *w = new SecClient;
    w->setAttribute(Qt::WA_DeleteOnClose);
    connect(w, SIGNAL(destroyed()),this,SLOT(setClientClosed()));

    w->setServerStatus(sc, as);
    isClientOpen = true;
    w->show();
}

void RootWin::setClientClosed()
{
    isClientOpen = false;
}

void RootWin::commWithServer()
{
    sc->connectToServer(as->serverName, as->port);

    mCommTimer = new QTimer(this);
    mCommTimer->start(5000);
    connect(mCommTimer,SIGNAL(timeout()),this,SLOT(pingToServer()));
}

void RootWin::pingToServer()
{
    if(as->authKey.isEmpty())
    {
        qDebug() << "Client is not authorized.";
        return;
    }
    if(!sc->isConnected())
    {
        if(!sc->connectToServer(as->serverName,as->port))
        {
            return;
        }
    }
    if(mLocalIP.isEmpty())
        mLocalIP = sc->getLocalIPfromConnection();
    if(mLocalMAC.isEmpty())
        mLocalMAC = sc->getLocalMACfromConnection();
    QJsonObject pingMessage;
    pingMessage.insert("CMD", "ping");
    pingMessage.insert("IP", mLocalIP);
    pingMessage.insert("MAC",mLocalMAC);
    pingMessage.insert("AUTH-KEY", as->authKey);
    QJsonDocument doc(pingMessage);
    QByteArray bytes = doc.toJson();

    sc->writeBytes(bytes);
    qDebug() << "ping to server: " << bytes;
}

void RootWin::OnReceive(QByteArray bytes)
{
    QJsonParseError err;
    QJsonObject message = QJsonDocument::fromJson(bytes, &err).object();
    qDebug() << err.errorString();
    QString cmd = message.take("CMD").toString();
    if(cmd == "hello")
    {
        //do nothing
    } else if(cmd == "ping-response") {
        validAuthorization(bytes);
    }
}

void RootWin::validAuthorization(QByteArray bytes)
{
    as->setAuthStatus(bytes);
}
