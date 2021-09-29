#include "secserver.h"
#include "ui_secserver.h"
#include "accessapproval.h"

#include <QHBoxLayout>
#include <QCheckBox>

SecServer::SecServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SecServer)
{
    ui->setupUi(this);
}

SecServer::~SecServer()
{
    delete ui;
}

QMap<QString, Client *> *SecServer::getClientList() const
{
    return clientList;
}

void SecServer::setClientList(QMap<QString, Client *> *value)
{
    clientList = value;
}

void SecServer::init()
{
    showClientList();
}

void SecServer::showClientList()
{
    QMap<QString, Client *>::iterator it;
    ui->tableWidget_ClientList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_ClientList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget_ClientList->setRowCount(0);
    ui->tableWidget_ClientList->clearContents();
    int row = 0;
    for(it = clientList->begin(); it!=clientList->end(); ++it)
    {
        Client *c = it.value();
        ui->tableWidget_ClientList->insertRow(row);
        QWidget *checkWidget = new QWidget;
        QHBoxLayout *checkLayout = new QHBoxLayout(checkWidget);
        QCheckBox *checkbox = new QCheckBox;
        checkLayout->addWidget(checkbox);
        checkLayout->setMargin(0);
        checkLayout->setAlignment(checkbox, Qt::AlignCenter);
        ui->tableWidget_ClientList->setCellWidget(row,0,checkWidget);
        ui->tableWidget_ClientList->setItem(row, 1, new QTableWidgetItem(c->cid));
        ui->tableWidget_ClientList->setItem(row, 2, new QTableWidgetItem(c->ip));
        ui->tableWidget_ClientList->setItem(row, 3, new QTableWidgetItem(c->mac));
        ui->tableWidget_ClientList->setItem(row, 4, new QTableWidgetItem(c->userFullName));
        ui->tableWidget_ClientList->setItem(row, 5, new QTableWidgetItem(c->registerStatus));

        QWidget *buttonWidget = new QWidget();
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
        if(c->mSocket != nullptr)
        {
            QString connStatus = c->mSocket->state() == QAbstractSocket::ConnectedState?"Yes":"No";
            QString authStatus = c->isAuthoried()?"Yes":"No";
            ui->tableWidget_ClientList->setItem(row, 6, new QTableWidgetItem(connStatus));
            ui->tableWidget_ClientList->setItem(row, 7, new QTableWidgetItem(authStatus));
        }
        ui->tableWidget_ClientList->setItem(row, 8, new QTableWidgetItem(QString::number(c->appVersion)));

        if(c->registerStatus != "Yes")
        {
            QPushButton *btnComm = new QPushButton(this);
            btnComm->setText("审批");
            connect(btnComm, SIGNAL(clicked()),this,SLOT(openApproval()));
            buttonLayout->addWidget(btnComm);
        }
        QPushButton *btnComm2 = new QPushButton(this);
        btnComm2->setText("删除");
        connect(btnComm2, SIGNAL(clicked()),this,SLOT(onDeleteClient()));
        buttonLayout->addWidget(btnComm2);
        buttonLayout->setMargin(0);
        ui->tableWidget_ClientList->setCellWidget(row,9,buttonWidget);
        row++;
    }
}

void SecServer::openApproval()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr)
        return;
    QWidget *buttonBox = (QWidget *)senderObj->parent();
    QModelIndex idx = ui->tableWidget_ClientList->indexAt(QPoint(buttonBox->frameGeometry().x(),buttonBox->frameGeometry().y()));

    QString clientIP = ui->tableWidget_ClientList->item(idx.row(),2)->text();
    Client *pendingClient = clientList->value(clientIP);
    AccessApproval *w = new AccessApproval;
    w->setApprovalInfo(pendingClient, dbcomm);
    w->show();
}

void SecServer::onDeleteClient()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr)
        return;
    QWidget *buttonBox = (QWidget *)senderObj->parent();
    QModelIndex idx = ui->tableWidget_ClientList->indexAt(QPoint(buttonBox->frameGeometry().x(),buttonBox->frameGeometry().y()));
//    QString cid = ui->tableWidget_ClientList->item(idx.row(),1)->text();
    QString clientIP = ui->tableWidget_ClientList->item(idx.row(),2)->text();
    Client *pendingClient = clientList->value(clientIP);
//    qDebug() << clientIP << idx.row() << idx.column();
    pendingClient->remove();
    clientList->remove(clientIP);
    delete pendingClient;
    showClientList();
}

void SecServer::on_pushButton_Refresh_clicked()
{
    showClientList();
}

void SecServer::on_pushButton_UpgradeClient_clicked()
{
    for(int i=0; i<ui->tableWidget_ClientList->rowCount(); i++)
    {
        QCheckBox *checkbox;
        QWidget *checkWidget = ui->tableWidget_ClientList->cellWidget(i, 0);
        QList<QCheckBox *> allCheckBoxs =  checkWidget->findChildren<QCheckBox *>();
        if(allCheckBoxs.size() <= 0)
            continue;
        checkbox = allCheckBoxs.first();
        QString clientIP = ui->tableWidget_ClientList->item(i, 2)->text();
        if(checkbox->isChecked())
        {
            clientList->value(clientIP)->requestForUpgrade();
        }
    }
}
