#ifndef SECSERVER_H
#define SECSERVER_H

#include "socketserver.h"
#include "databasecomm.h"
#include "client.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SecServer; }
QT_END_NAMESPACE

class SecServer : public QWidget
{
    Q_OBJECT

public:
    SecServer(QWidget *parent = nullptr);
    ~SecServer();

    DatabaseComm *dbcomm;
    QMap<QString, Client *> *clientList;

    void init();
    void showClientList();

    QMap<QString, Client *> *getClientList() const;
    void setClientList(QMap<QString, Client *> *value);

private slots:
    void on_pushButton_Refresh_clicked();
    void openApproval();
    void onDeleteClient();

    void on_pushButton_UpgradeClient_clicked();

private:
    Ui::SecServer *ui;
};
#endif // SECSERVER_H
