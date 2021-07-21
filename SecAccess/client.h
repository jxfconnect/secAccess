#ifndef CLIENT_H
#define CLIENT_H

#include "databasecomm.h"
#include "log.h"

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

    QString ip;
    QString mac;
    QString authKey;

    QString cIP;
    QString cMac;
    QString cAuthKey;
    QTcpSocket *mSocket;
    DatabaseComm *dbcomm;
    QDateTime lastValidTime;
    QString ruleName;
    QString cmd;
    QString registerStatus;
    bool ruleEnableStatus;

    bool isRegistered();
    bool isAuthoried();
    bool addAllowRule();
    bool enableAllowRule();
    bool disableAllowRule();
    void decodeRegister(QJsonObject message);
    void decodePing(QJsonObject message);
    void decodeQueryRegister(QJsonObject message);
    void getInfoFromDB();
    void setInfo(DatabaseComm *dbcomm, QTcpSocket *socket);
    void pingResponse();
    void ApprovalResponse(bool registerStatus);
    QString getRandomString(int length);

signals:
    void clientReady(Client *);

private:
    void validAuth(QJsonObject *message);

    Log *log;

private slots:
    void onReceive();

};

#endif // CLIENT_H
