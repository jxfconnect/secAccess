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

private slots:
    void on_pushButton_OpenApproval_clicked();

    void on_pushButton_Refresh_clicked();
    void openApproval();

private:
    Ui::SecServer *ui;
};
#endif // SECSERVER_H
