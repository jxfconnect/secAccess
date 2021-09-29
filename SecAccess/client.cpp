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
    deviceCode = nullptr;
    ruleEnableStatus = false;
    rules = new QList<FirewallRule *>;
    cCryptStatus = false;
    appVersion = 0;

    mSocket = nullptr;

    argR1 = "program=\"D:\\DigiWin PLM\\Server\\jre\\bin\\java.exe\"";
    argR2 = "program=\"D:\\DigiWin PLM\\Tomcat\\jre\\bin\\java.exe\"";
//    argR1 = "program=\"D:\\app\\ERP\\Client\\Digiwin.Mars.Deployment.Client.exe\"";
//    argR2 = "program=\"D:\\app\\ERP\\Client\\Digiwin.Mars.Deployment.Client.exe\"";

    log = new Log;
    log->loadConfiguration("logger.ini");
}

QTcpSocket *Client::getSocket() const
{
    return mSocket;
}

void Client::setSocket(QTcpSocket *socket)
{
    mSocket = socket;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReceive()));
//    connect(c, SIGNAL(clientReady(Client *)), this, SLOT(handleClientMessage(Client *)));
}

void Client::init()
{
    initFirewallRule();
}

bool Client::initFirewallRule()
{
    if(ip == nullptr)
        return false;
    FirewallRule *ruleR1 = new FirewallRule;
    ruleR1->setClientIP(ip);
    ruleR1->setTypeName("R1");
    ruleR1->initRule();
    rules->append(ruleR1);
    FirewallRule *ruleR2 = new FirewallRule;
    ruleR2->setClientIP(ip);
    ruleR2->setTypeName("R2");
    ruleR2->initRule();
    rules->append(ruleR2);

    return true;
}

//not use anymore
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
    cAuthKey = message.value("AUTH-KEY").toString();
    cDeviceCode = message.value("AUTH-KEY2").toString();
    cCryptStatus = message.value("SEC-STATUS").toBool();
//    log->write(Log::TYPE_DEBUG, "Cient::onReceive", QString("IP:%1; MAC:%2; CMD:%3; AUTH-KEY:%4; DEVCODE:%5; SEC-STATUS:%6").arg(cIP).arg(cMac)
//               .arg(cmd).arg(cAuthKey).arg(cDeviceCode).arg(cCryptStatus));
    if(cmd != "ping")
    {
        qDebug() << message;
        log->write(Log::TYPE_DEBUG, "Cient::onReceive", bytes);
    }
    if(cmd == "register")
    {
        decodeRegister(message);
    } else if(cmd == "ping")
    {
        decodePing(message);
    } else if(cmd == "query-register-status") {
        decodeQueryRegister(message);
    }
}

void Client::decodePing(QJsonObject message)
{
    lastValidTime = QDateTime::currentDateTime();
    cAuthKey = message.value("AUTH-KEY").toString();
    cDeviceCode = message.value("AUTH-KEY2").toString();
    cCryptStatus = message.value("SEC-STATUS").toBool();
    appVersion = message.value("APP-VERSION").toInt();
    pingResponse();
}

void Client::decodeRegister(QJsonObject message)
{
    if(isClientExist())
    {
        log->write(Log::TYPE_DEBUG, "Client::decodeRegister", "Failed to add new client. Client already exists: "+cIP);
        return;
    }
    if(cIP.isEmpty())
    {
        log->write(Log::TYPE_DEBUG, "Client::decodeRegister", "Failed to add new client. Client IP is empty.");
        return;
    }
    bool dbResult = recordNewClient();
    if(dbResult)
    {
        getInfoFromDB();
        emit clientReady(this);
    }else{
        log->write(Log::TYPE_DEBUG, "Client::decodeRegister", "Failed to add new client. database record creation failed.");
    }
}

void Client::decodeQueryRegister(QJsonObject message)
{
    bool result = registerStatus == "Yes"?true:false;
    ApprovalResponse(result);
}

bool Client::isValid()
{
    if(authKey==nullptr || authKey != cAuthKey)
        return false;
    if(mac==nullptr || mac != cMac)
        return false;
    if(deviceCode== nullptr || deviceCode != cDeviceCode)
        return false;
    return true;
}

bool Client::isCrypted()
{
    return cCryptStatus;
}

bool Client::isAuthoried()
{
    qDebug() << "Elapsed time: " <<lastValidTime.secsTo(QDateTime::currentDateTime());
    if(!isValid())
        return false;
    if(!isCrypted())
        return false;
    if(lastValidTime.secsTo(QDateTime::currentDateTime())>30)
    {
        return false;
    }
    return true;
}

void Client::getInfoFromDB()
{
    QString sql = QString("select cid, ip,mac,auth_key,auth_status,rule_name,user_name, device_code from client where ip='%1'").arg(cIP);
    QList<QMap<QString, QString>> clients = dbcomm->read(sql);
    if(clients.size() <= 0)
        return;
    QMap<QString, QString> clientRecord = clients.at(0);
    cid = clientRecord.value("cid");
    ip = clientRecord.value("ip");
    mac = clientRecord.value("mac");
    authKey = clientRecord.value("auth_key");
    userFullName = clientRecord.value("user_name");
    deviceCode = clientRecord.value("device_code");
    registerStatus = clientRecord.value("auth_status");
//    rule->setRuleName(clientRecord.value("rule_name"));
}

void Client::pingResponse()
{
//    log->write(Log::TYPE_DEBUG, "Client::pingResponse", "cIP: "+cIP+"; authkey:"+authKey+"; cauthkey:"+cAuthKey+"; mac:"+mac+"; cMac:"+cMac+"; dCode:"+deviceCode
//               +"; cDcode:"+cDeviceCode+"; cryptStatus:"+cCryptStatus);
    QJsonObject ResponseMessage;
    ResponseMessage.insert("CMD", "ping-response");
    QString strStatus;
    if(isValid() && isCrypted())
    {
        strStatus = "Y";
    } else {
        strStatus = "N";
    }
    ResponseMessage.insert("STATUS", strStatus);
    QJsonDocument doc(ResponseMessage);
    QByteArray bytes = doc.toJson();
    mSocket->write(bytes);
//    log->write(Log::TYPE_DEBUG, "Client::pingResponse", bytes);
}

void Client::ApprovalResponse(bool registerStatus)
{
    QJsonObject ResponseMessage;
    ResponseMessage.insert("CMD", "register-response");
    QString result;
    result = registerStatus?"Y":"N";
    ResponseMessage.insert("RESULT", result);
    qDebug() << "AUTH-KEY:" <<authKey;
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

bool Client::recordNewClient()
{
    log->write(Log::TYPE_DEBUG, "Client::recordNewClient", "Start to create new record in database.");
    bool retval = false;
    QMap<QString, QString> clientRecord;
    clientRecord.insert("ip",cIP);
    clientRecord.insert("mac", cMac);
    clientRecord.insert("device_code", cDeviceCode);
    clientRecord.insert("auth_status", "No");
    retval = dbcomm->insert("client", clientRecord);
    return retval;
}

bool Client::isClientExist()
{
    QString sql = QString("select ip from client where ip='%1'").arg(cIP);
    QList<QMap<QString, QString>> clients = dbcomm->read(sql);
    if(clients.size()>0)
        return true;
    return false;
}

void Client::approveAuthorization()
{
    FirewallRule *ruleR1 = new FirewallRule;
    ruleR1->setClientIP(ip);
    ruleR1->setTypeName("R1");
    ruleR1->setArgs(argR1);
    ruleR1->createRule();
    ruleR1->initRule();
    rules->append(ruleR1);
    FirewallRule *ruleR2 = new FirewallRule;
    ruleR2->setClientIP(ip);
    ruleR2->setTypeName("R2");
    ruleR2->setArgs(argR2);
    ruleR2->createRule();
    ruleR2->initRule();
    rules->append(ruleR2);

    if(authKey.isEmpty())
    {
        authKey = getRandomString(12);
    }
    QString sql = QString("update client set auth_status='Yes', auth_key='%2', user_name='%3' where ip='%1'").arg(ip).arg(authKey).arg(userFullName);
    dbcomm->update(sql);

    registerStatus = "Yes";
}

void Client::removeAuthorization()
{

}

void Client::remove()
{
    for(int i=0; i<rules->size(); i++)
    {
        rules->at(i)->deleteRule();
    }
    ruleEnableStatus = false;
    registerStatus = false;
    authKey = nullptr;
    if(mSocket) {
        if(mSocket->state() == QAbstractSocket::ConnectedState)
        {
            mSocket->disconnectFromHost();
        }
        delete mSocket;
        mSocket = nullptr;
    }

    QString sql = tr("delete from client where cid=%1").arg(cid);
    dbcomm->deleteRecord(sql);
}

void Client::updateAuthorization()
{
    log->write(Log::TYPE_DEBUG, "Client::updateAuthorization", "Client IP is: "+ip);
    if(isAuthoried())
    {
        if(!ruleEnableStatus)
        {
            enableAllowRules();
            ruleEnableStatus = true;
            log->write(Log::TYPE_DEBUG, "Client::updateAuthorization", ip + "'s rules enabled");
        }
    } else {
        cCryptStatus = false;
        if(ruleEnableStatus)
        {
            disableAllowRules();
            ruleEnableStatus = false;
            log->write(Log::TYPE_DEBUG, "Client::updateAuthorization", ip + "'s rules disabled");
        }
    }
}

bool Client::enableAllowRules()
{
    for(int i=0; i<rules->size(); i++)
    {
        rules->at(i)->enableRule();
    }
    return true;
}

bool Client::disableAllowRules()
{
    for(int i=0; i<rules->size(); i++)
    {
        rules->at(i)->disableRule();
    }
    return true;
}

bool Client::requestForUpgrade()
{
    if(mSocket->state() != QAbstractSocket::ConnectedState)
        return false;

    QJsonObject message;
    message.insert("CMD", "request-for-app-upgrade");
    QJsonDocument doc(message);
    QByteArray bytes = doc.toJson();
    if(mSocket->write(bytes))
    {
        return true;
    }else{
        return false;
    }
}
