#include "secclient.h"
#include "ui_secclient.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

SecClient::SecClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SecClient)
{
    ui->setupUi(this);
}

SecClient::~SecClient()
{
    delete ui;
}

void SecClient::on_pushButton_clicked()
{
    if(as==nullptr)
        return;
    QString servIP = as->serverName;
    int servPort = as->port;
    if(sc->connectToServer(servIP,servPort))
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
        QJsonDocument doc(pingMessage);
        QByteArray bytes = doc.toJson();
        sc->writeBytes(bytes);
    } else {
        qDebug() << "Fail to connect to server.";
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

void SecClient::on_pushButton_2_clicked()
{
    sc->getInterfaces();
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

void SecClient::on_pushButton_ApprovalResult_clicked()
{
    if(!sc->isConnected())
    {
        QMessageBox::critical(NULL, "critical", "Not connected to server.",
                           QMessageBox::Close, QMessageBox::Close);
        return;
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
