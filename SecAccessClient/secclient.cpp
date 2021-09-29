#include "secclient.h"
#include "ui_secclient.h"

#include "wmiccommand.h"
#include "systrayreader.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFile>

SecClient::SecClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SecClient)
{
    ui->setupUi(this);

    loadStylesheet("./qss/client.qss");

    ui->lineEdit->setDisabled(true);
    ui->lineEdit_2->setDisabled(true);
    ui->lineEdit_3->setDisabled(true);
    ui->label_3->setVisible(false);
    ui->lineEdit_3->setVisible(false);
    ui->textEdit->setVisible(false);

    log = new Log;
    log->loadConfiguration("logger.ini");
}

SecClient::~SecClient()
{
    delete ui;
}

void SecClient::loadStylesheet(const QString &fileName)
{
    QFile file(fileName);
    if(!file.exists())
        qDebug() << "stylesheet file not exsit.";

    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet = filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();
}

//request for register
void SecClient::on_pushButton_clicked()
{
    if(as==nullptr)
        return;
    QString servIP = as->serverName;
    int servPort = as->port;
    bool connStatus = sc->isConnected();
    if(!connStatus)
        sc->connectToServer(servIP,servPort);
    if(connStatus)
    {
        QString localIP = sc->getLocalIPfromConnection();
        QString localMAC = sc->getLocalMACfromConnection();
        ui->lineEdit_2->setText(localIP);
        ui->lineEdit_3->setText(localMAC);
        QJsonObject pingMessage;
        pingMessage.insert("CMD", "register");
        pingMessage.insert("IP", localIP);
        pingMessage.insert("MAC",localMAC);
        pingMessage.insert("AUTH-KEY", as->authKey);
        pingMessage.insert("AUTH-KEY2",as->devCode);
        pingMessage.insert("SEC-STATUS",as->isCryptClientRuning());
        QJsonDocument doc(pingMessage);
        QByteArray bytes = doc.toJson();
        sc->writeBytes(bytes);
    } else {
        qDebug() << "Fail to connect to server.";
        log->write(Log::TYPE_DEBUG, "SecClient::RequestForRegister", "Fail to connect to server.");
    }
}

void SecClient::onReceiveMessage(QByteArray message)
{
    ui->textEdit->insertPlainText(message);
    QString registerStatus;
    QJsonObject jsonMessage = QJsonDocument::fromJson(message).object();
    QString cmd = jsonMessage.take("CMD").toString();
    if(cmd == "register-response") {
        QString result = jsonMessage.take("RESULT").toString();
        if(result == "Y") {
            //update cfg file
            QString authKey = jsonMessage.take("AUTH-KEY").toString();
            as->setAuthKey(authKey);
            registerStatus = "Registered";
            runStatusChecking();
        } else {
            registerStatus = "Not Registered";
        }
        ui->label_HasAuthKey->setText(registerStatus);
        ui->pushButton->setDisabled(result=="Y");
    }
}

//for test
void SecClient::on_pushButton_2_clicked()
{
//    sc->getInterfaces();

//    WMICCommand wmic;
//    QString deviceCode = wmic.getDevCode();
//    ui->textEdit->append(deviceCode);

    WMICCommand wmic;
    QString procStr = "watchctrl.exe,CdgTwin64.exe,CdgTwin32.exe,CDGRegedit.exe";
    QStringList procList = procStr.split(",");
    QString cmdResult = wmic.execCmd("cmd","tasklist");
    ui->textEdit->append(cmdResult);
    for(int i=0; i<procList.size(); i++)
    {
         if(!cmdResult.contains(procList.at(i)))
             log->write(Log::TYPE_DEBUG, "SecClient::on_pushButton_2_clicked", "Proc "+procList.at(i)+" not found.");
    }

    SysTrayReader tray;
    // 解决控制台中文 '？'
    setlocale(LC_ALL, "chs");

    // 获取托盘句柄
    HWND h_tray = tray.FindTrayWnd();
    HWND h_tray_fold = tray.FindNotifyIconOverflowWindow();

    // 遍历托盘窗口
    QList<WindowInfo *> *infoList = new QList<WindowInfo *>;
    tray.EnumNotifyWindow(h_tray, infoList);
    foreach(WindowInfo *info, *infoList)
    {
        QString procPath = info->getProcPath();
        QString winTitle = info->getWinTitle();
        QString strInfo = "\nWndID: "+info->getMainWndID()+"\nProcPath: "+procPath+"\nWinTitle: "+winTitle;
        ui->textEdit->append(strInfo);
        if(procPath.contains("EsafeNet\\Cobra DocGuard Client\\CDGRegedit.exe") && ((winTitle.contains("联机登录") && winTitle.contains("密文模式"))
                || winTitle.contains("您有未处理消息")))
            ui->textEdit->append("已登录");
    }
    QList<WindowInfo *> *infoListFold = new QList<WindowInfo *>;
    tray.EnumNotifyWindow(h_tray_fold, infoListFold);
    foreach(WindowInfo *info, *infoListFold)
    {
        QString procPath = info->getProcPath();
        QString winTitle = info->getWinTitle();
        QString strInfo = "\nWndID: "+info->getMainWndID()+"\nProcPath: "+procPath+"\nWinTitle: "+winTitle;
        ui->textEdit->append(strInfo);
        if(procPath.contains("EsafeNet\\Cobra DocGuard Client\\CDGRegedit.exe") && ((winTitle.contains("联机登录") && winTitle.contains("密文模式"))
                || winTitle.contains("您有未处理消息")))
            ui->textEdit->append("已登录");
    }
}


void SecClient::setServerStatus(SocketComm *sc, AuthStatus *as)
{
    this->sc = sc;
    connect(sc, SIGNAL(readyToPump(QByteArray)), this, SLOT(onReceiveMessage(QByteArray)));

    this->as = as;
    if(!as->authKey.isEmpty())
    {
        ui->label_HasAuthKey->setText("Yes");
        runStatusChecking();
        ui->pushButton->setDisabled(true);
    } else {
        ui->label_HasAuthKey->setText("No");
        ui->pushButton->setDisabled(false);
    }
    ui->lineEdit->setText(as->serverName);
}

void SecClient::checkConnectionStatus()
{
    if(sc->isConnected())
    {
        ui->lineEdit->setText(sc->getServerName());
        ui->lineEdit_2->setText(sc->getLocalIPfromConnection());
        ui->lineEdit_3->setText(sc->getLocalMACfromConnection());
        ui->label_ConnectionStatus->setText("已经连接到服务器！");
    } else {
        ui->label_ConnectionStatus->setText("没有连接到服务器！");
    }
}

void SecClient::checkAuthStatus()
{
    checkConnectionStatus();
    if(as->isClientAuthoried())
    {
        ui->label_AuthStatus->setText("已授权");
    } else {
        ui->label_AuthStatus->setText("未授权");
    }
}

void SecClient::runStatusChecking()
{
    mCommTimer = new QTimer(this);
    mCommTimer->start(15000);
    connect(mCommTimer,SIGNAL(timeout()),this,SLOT(checkAuthStatus()));
}

//query for register result
void SecClient::on_pushButton_ApprovalResult_clicked()
{
    if(!sc->isConnected())
    {
        if(!sc->connectToServer(as->serverName, as->port))
        {
            QMessageBox::critical(NULL, "critical", "Not connected to server.",
                               QMessageBox::Close, QMessageBox::Close);
            return;
        }
    }
    QString localIP = sc->getLocalIPfromConnection();
    QString localMAC = sc->getLocalMACfromConnection();
    QJsonObject pingMessage;
    pingMessage.insert("CMD", "query-register-status");
    pingMessage.insert("IP", localIP);
    pingMessage.insert("MAC",localMAC);
    QJsonDocument doc(pingMessage);
    QByteArray bytes = doc.toJson();
    sc->writeBytes(bytes);
}
