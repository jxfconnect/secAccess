#ifndef SECCLIENT_H
#define SECCLIENT_H

#include "socketcomm.h"
#include "authstatus.h"

#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class SecClient; }
QT_END_NAMESPACE

class SecClient : public QWidget
{
    Q_OBJECT

public:
    SecClient(QWidget *parent = nullptr);
    ~SecClient();

    QTimer *mCommTimer;
    void setServerStatus(SocketComm *sc, AuthStatus *as);
    void runStatusChecking();

private slots:
    void on_pushButton_clicked();
    void onReceiveMessage(QByteArray message);

    void on_pushButton_2_clicked();
    void checkConnectionStatus();
    void checkAuthStatus();

    void on_pushButton_ApprovalResult_clicked();

private:
    Ui::SecClient *ui;
    SocketComm *sc;
    AuthStatus *as;
};
#endif // SECCLIENT_H
