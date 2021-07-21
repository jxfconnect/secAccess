#include "accessapproval.h"
#include "ui_accessapproval.h"

#include <QTextCodec>
#include <QProcess>

AccessApproval::AccessApproval(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccessApproval)
{
    ui->setupUi(this);

    mServer = new SocketServer(this);
    connect(mServer, SIGNAL(readyToDisplay(QString, int, QByteArray)), this, SLOT(onDisplayMessage(QString, int, QByteArray)));
}

AccessApproval::~AccessApproval()
{
    delete ui;
}

void AccessApproval::setApprovalInfo(Client *client, DatabaseComm *dbcomm)
{
    this->dbComm = dbcomm;
    mClient = client;
    ui->lineEdit->setText(client->mac);
    ui->lineEdit_2->setText(client->ip);
}

void AccessApproval::on_pushButton_clicked()
{
    QString ip = ui->lineEdit_2->text();
    QString ruleName = QString("100sec_ip%1").arg(ip);

//    QProcess process(this);
//    process.setProgram("cmd");
//    QStringList argument;
//    argument <<"/c" << ruleArgs;
//    process.setArguments(argument);
//    process.start();
//    process.waitForStarted(); //等待程序启动
//    process.waitForFinished();//等待程序关闭
//    cmdlineResult=QString::fromLocal8Bit(process.readAllStandardOutput()); //程序输出信息

    FirewallRule rule(this);
    rule.createRule(ip);

    if(mClient->authKey.isEmpty())
    {
        mClient->authKey = mClient->getRandomString(12);
    }
    QString sql = QString("update client set rule_name='%1', auth_status='Yes', auth_key='%3', user_name='%4' where ip='%2'").arg(ruleName).arg(ip).arg(mClient->authKey).arg(ui->lineEdit_UserFullName->text());
    dbComm->update(sql);

    mClient->registerStatus = "Yes";

//    mClient->ApprovalResponse(true);
}

void AccessApproval::onDisplayMessage(QString remoteIP, int remotePort, QByteArray message)
{
//    ui->textEdit->append(QString("MSG-SIZE: %1").arg(message.size()));

    QTextCodec *tc = QTextCodec::codecForName("GBK");
    QString tmpQStr = tc->toUnicode(message);
    ui->textEdit->insertPlainText(tmpQStr);
    ui->lineEdit->setText(tmpQStr);
    ui->lineEdit_2->setText(remoteIP);
}
