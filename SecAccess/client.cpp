#include "client.h"

#include <QHostAddress>

Client::Client(QObject *parent) : QObject(parent)
{
    cIP = nullptr;
    cMac = nullptr;
    cAuthKey = nullptr;

    ip = nullptr;
    mac = nullptr;
    authKey = nullptr;
    ruleEnableStatus = false;

    mSocket = nullptr;

    log = new Log;
    log->loadConfiguration("logger.ini");
}

void Client::setInfo(DatabaseComm *dbcomm, QTcpSocket *socket)
{
    this->dbcomm = dbcomm;
    mSocket = socket;

//    cIP = mSocket->peerAddress().toString();
//    getInfoFromDB();
}

void Client::onReceive()
{
    QByteArray bytes = mSocket->readAll();
    QJsonObject message = QJsonDocument::fromJson(bytes).object();
    cmd = message.take("CMD").toString();
    cMac = message.take("MAC").toString();
    if(cmd == "register")
    {
        decodeRegister(message);
    } else if(cmd == "ping")
    {
        lastValidTime = QDateTime::currentDateTime();
        decodePing(message);
    } else if(cmd == "query-register-status") {
        decodeQueryRegister(message);
    }
    emit clientReady(this);
}

void Client::decodePing(QJsonObject message)
{
    cAuthKey = message.take("AUTH-KEY").toString();
}

void Client::decodeRegister(QJsonObject message)
{

}

void Client::decodeQueryRegister(QJsonObject message)
{

}

bool Client::isRegistered()
{
    if(authKey==nullptr || authKey != cAuthKey)
        return false;
    if(mac==nullptr || mac != cMac)
        return false;
    return true;
}

bool Client::isAuthoried()
{
    qDebug() << "Time: " <<lastValidTime.secsTo(QDateTime::currentDateTime());
    if(!isRegistered())
        return false;
    if(lastValidTime.secsTo(QDateTime::currentDateTime())>30)
        return false;
    return true;
}

void Client::getInfoFromDB()
{
    QString sql = QString("select ip,mac,auth_key,auth_status,rule_name,user_name from client where ip='%1'").arg(cIP);
    QList<QMap<QString, QString>> clients = dbcomm->read(sql);
    if(clients.size() <= 0)
        return;
    QMap<QString, QString> clientRecord = clients.at(0);
    ip = clientRecord.value("ip");
    mac = clientRecord.value("mac");
    authKey = clientRecord.value("auth_key");
    registerStatus = clientRecord.value("auth_status");
    ruleName = clientRecord.value("rule_name");
}

void Client::pingResponse()
{
    QJsonObject ResponseMessage;
    ResponseMessage.insert("CMD", "ping-response");
    QString strStatus;
    if(isRegistered())
    {
        strStatus = "Y";
    } else {
        strStatus = "N";
    }
    ResponseMessage.insert("STATUS", strStatus);
    QJsonDocument doc(ResponseMessage);
    QByteArray bytes = doc.toJson();
    mSocket->write(bytes);
    log->write(Log::TYPE_DEBUG, "Client", bytes);
}

void Client::ApprovalResponse(bool registerStatus)
{
    QJsonObject ResponseMessage;
    ResponseMessage.insert("CMD", "register-response");
    QString result;
    result = registerStatus?"Y":"N";
    ResponseMessage.insert("RESULT", result);
    qDebug() << authKey;
    ResponseMessage.insert("AUTH-KEY", authKey);
    QJsonDocument doc(ResponseMessage);
    QByteArray bytes = doc.toJson();
    mSocket->write(bytes);
}

QString Client::getRandomString(int length)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());//为随机值设定一个seed

    const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int chrs_size = sizeof(chrs);

    char* ch = new char[length + 1];
    memset(ch, 0, length + 1);
    int randomx = 0;
    for (int i = 0; i < length; ++i)
    {
        randomx= rand() % (chrs_size - 1);
        ch[i] = chrs[randomx];
    }

    QString ret(ch);
    delete[] ch;
    return ret;
}
