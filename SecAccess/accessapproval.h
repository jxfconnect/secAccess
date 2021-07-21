#ifndef ACCESSAPPROVAL_H
#define ACCESSAPPROVAL_H

#include "socketserver.h"
#include "databasecomm.h"
#include "client.h"
#include "firewallrule.h"

#include <QWidget>

namespace Ui {
class AccessApproval;
}

class AccessApproval : public QWidget
{
    Q_OBJECT

public:
    explicit AccessApproval(QWidget *parent = nullptr);
    ~AccessApproval();

    void setApprovalInfo(Client *client, DatabaseComm *dbcomm);

private slots:
    void on_pushButton_clicked();
    void onDisplayMessage(QString remoteIP, int remotePort, QByteArray message);

private:
    Ui::AccessApproval *ui;

    SocketServer *mServer;
    DatabaseComm *dbComm;
    Client *mClient;
};

#endif // ACCESSAPPROVAL_H
