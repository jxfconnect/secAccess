#include "secserver.h"
#include "ui_secserver.h"
#include "accessapproval.h"

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

void SecServer::init()
{
    showClientList();
}

void SecServer::showClientList()
{
    QMap<QString, Client *>::iterator it;
    ui->tableWidget_ClientList->setRowCount(0);
    ui->tableWidget_ClientList->clearContents();
    int row = 0;
    for(it = clientList->begin(); it!=clientList->end(); ++it)
    {
        Client *c = it.value();
        ui->tableWidget_ClientList->insertRow(row);
        ui->tableWidget_ClientList->setItem(row, 0, new QTableWidgetItem(c->ip));
        ui->tableWidget_ClientList->setItem(row, 1, new QTableWidgetItem(c->mac));
        ui->tableWidget_ClientList->setItem(row, 2, new QTableWidgetItem(c->registerStatus));
        if(c->mSocket != nullptr)
        {
            QString connStatus = c->mSocket->state() == QAbstractSocket::ConnectedState?"Yes":"No";
            QString authStatus = c->isAuthoried()?"Yes":"No";
            ui->tableWidget_ClientList->setItem(row, 3, new QTableWidgetItem(connStatus));
            ui->tableWidget_ClientList->setItem(row, 4, new QTableWidgetItem(authStatus));
        }
        if(c->registerStatus != "Yes")
        {
            QPushButton *btnComm = new QPushButton(this);
            btnComm->setText("审批");
            connect(btnComm, SIGNAL(clicked()),this,SLOT(openApproval()));
            ui->tableWidget_ClientList->setCellWidget(row,5,btnComm);
        }
        row++;
    }
}


void SecServer::on_pushButton_OpenApproval_clicked()
{
    QList<QTableWidgetItem*> items = ui->tableWidget_ClientList->selectedItems();
    if(items.isEmpty())
        return;
    QString ip = items.at(0)->text();
    if(!clientList->contains(ip))
        return;
    Client *pendingClient = clientList->value(ip);

    AccessApproval *w = new AccessApproval;
    w->setApprovalInfo(pendingClient, dbcomm);

    w->show();
}

void SecServer::openApproval()
{
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr)
        return;
    QModelIndex idx = ui->tableWidget_ClientList->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));

    QString clientIP = ui->tableWidget_ClientList->item(idx.row(),0)->text();
    Client *pendingClient = clientList->value(clientIP);
    AccessApproval *w = new AccessApproval;
    w->setApprovalInfo(pendingClient, dbcomm);
    w->show();
}

void SecServer::on_pushButton_Refresh_clicked()
{
    showClientList();
}
