#ifndef CLIENT_H
#define CLIENT_H

#include "databasecomm.h"
#include "log.h"
#include "firewallrule.h"

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);

    QString cid;
    QString ip;
    QString mac;
    QString authKey;

    QString cIP;
    QString cMac;
    QString cAuthKey;
    QString cDeviceCode;
    QTcpSocket *mSocket;
    DatabaseComm *dbcomm;
    QDateTime lastValidTime;
    QString cmd;
    QString registerStatus;
    bool ruleEnableStatus;
    QList<FirewallRule *> *rules;
    QString userFullName;
    QString deviceCode;
    int appVersion;
    bool cCryptStatus;

    QString argR1;
    QString argR2;

    bool isValid();
    bool isCrypted();
    bool isAuthoried();
    bool addAllowRules();
    bool enableAllowRules();
    bool disableAllowRules();
    void decodeRegister(QJsonObject message);
    void decodePing(QJsonObject message);
    void decodeQueryRegister(QJsonObject message);
    void getInfoFromDB();
    void setInfo(DatabaseComm *dbcomm, QTcpSocket *socket);
    void pingResponse();
    void ApprovalResponse(bool registerStatus);
    QString getRandomString(int length);
    bool isClientExist();
    bool recordNewClient();
    void approveAuthorization();
    void removeAuthorization();
    void updateAuthorization();
    void init();
    bool initFirewallRule();
    void remove();
    bool requestForUpgrade();

    QTcpSocket *getSocket() const;
    void setSocket(QTcpSocket *socket);

signals:
    void clientReady(Client *);

private:
    void validAuth(QJsonObject *message);

    Log *log;

private slots:
    void onReceive();

};

#endif // CLIENT_H
